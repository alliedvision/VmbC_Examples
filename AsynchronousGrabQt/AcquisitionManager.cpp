/**
 * \date 2021
 * \copyright Allied Vision Technologies.  All Rights Reserved.
 *
 * \copyright Redistribution of this file, in original or modified form, without
 *            prior written consent of Allied Vision Technologies is prohibited.
 *
 * \warning THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 * NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \brief Implementation of ::VmbC::Examples::AcquisitionManager
 */

#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

#include "AcquisitionManager.h"
#include "VmbException.h"

#include "UI/MainWindow.h"

namespace VmbC
{
    namespace Examples
    {

        namespace
        {
            /**
             * \brief feature name of custom command for choosing the packet size provided by AVT GigE cameras
             */
            constexpr char const* AdjustPackageSizeCommand = "GVSPAdjustPacketSize";

            struct AcquisitionContext
            {
                AcquisitionManager* m_acquisitionManager;

                AcquisitionContext(AcquisitionManager* acquisitionManager) noexcept
                    : m_acquisitionManager(acquisitionManager)
                {
                }

                AcquisitionContext(VmbFrame_t const& frame) noexcept
                    : m_acquisitionManager(static_cast<AcquisitionManager*>(frame.context[0]))
                {
                }

                void FillFrame(VmbFrame_t& frame) const noexcept
                {
                    frame.context[0] = m_acquisitionManager;
                }

            };
        };

        void AcquisitionManager::StartAcquisition(VmbCameraInfo_t const& cameraInfo)
        {
            StopAcquisition(); // if a camera is open, close it first
            m_openCamera.reset(new CameraAccessLifetime(cameraInfo, *this));
            m_imageTranscoder.Start();
        }

        void AcquisitionManager::StopAcquisition() noexcept
        {
            m_imageTranscoder.Stop();
            m_openCamera.reset();
        }

        AcquisitionManager::AcquisitionManager(MainWindow& renderWindow)
            : m_renderWindow(renderWindow),
            m_imageTranscoder(*this)
        {
        }

        AcquisitionManager::~AcquisitionManager()
        {
            StopAcquisition();
            m_openCamera.reset();
        }

        void AcquisitionManager::ConvertedFrameReceived(QPixmap image)
        {
            m_renderWindow.RenderImage(image);
        }

        void AcquisitionManager::SetOutputSize(QSize size)
        {
            m_imageTranscoder.SetOutputSize(size);
        }

        void VMB_CALL AcquisitionManager::FrameCallback(VmbHandle_t /* cameraHandle */, VmbHandle_t const streamHandle, VmbFrame_t* frame)
        {
            if (frame != nullptr)
            {
                AcquisitionContext context(*frame);
                if (context.m_acquisitionManager != nullptr)
                {
                    context.m_acquisitionManager->FrameReceived(streamHandle, frame);
                }
            }
        }

        void AcquisitionManager::FrameReceived(VmbHandle_t const streamHandle, VmbFrame_t const* frame)
        {
            m_imageTranscoder.PostImage(streamHandle, &AcquisitionManager::FrameCallback, frame);
        }

        AcquisitionManager::CameraAccessLifetime::CameraAccessLifetime(VmbCameraInfo_t const& camInfo, AcquisitionManager& acquisitionManager)
        {
            VmbError_t error = VmbCameraOpen(camInfo.cameraIdString, VmbAccessModeFull, &m_cameraHandle);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbCameraOpen");
            }

            // refresh camera info to get streams
            VmbCameraInfo_t refreshedCameraInfo;
            bool errorHappened = false;
            VmbException ex;
            error = VmbCameraInfoQuery(camInfo.cameraIdString, &refreshedCameraInfo, sizeof(refreshedCameraInfo)); // TODO use extended id?
            if (error != VmbErrorSuccess)
            {
                errorHappened = true;
                ex = VmbException::ForOperation(error, "VmbCameraOpen");
            }

            if (!errorHappened)
            {
                errorHappened = true;
                if (refreshedCameraInfo.localDeviceHandle == nullptr)
                {
                    ex = VmbException("The id could not be used to query the info of the correct camera");
                }
                else if (refreshedCameraInfo.streamCount == 0)
                {
                    ex = VmbException("The camera does not provide a stream");
                }
                else
                {
                    errorHappened = false;
                }
            }

            if (!errorHappened)
            {
                // execute packet size adjustment, if this is a AVT GigE camera
                if (VmbErrorSuccess == VmbFeatureCommandRun(m_cameraHandle, AdjustPackageSizeCommand))
                {

                    VmbBool_t isCommandDone = VmbBoolFalse;
                    do
                    {
                        if (VmbErrorSuccess != VmbFeatureCommandIsDone(m_cameraHandle,
                            AdjustPackageSizeCommand,
                            &isCommandDone))
                        {
                            break;
                        }
                    } while (VmbBoolFalse == isCommandDone);
                }

                try
                {
                    m_streamLife.reset(new StreamLifetime(refreshedCameraInfo.streamHandles[0], m_cameraHandle, acquisitionManager));
                }
                catch (...)
                {
                    VmbCameraClose(m_cameraHandle);
                    throw;
                }
            }
            else
            {
                VmbCameraClose(m_cameraHandle);
                throw ex;
            }
        }

        AcquisitionManager::CameraAccessLifetime::~CameraAccessLifetime()
        {
            m_streamLife.reset(); // close stream first
            VmbCameraClose(m_cameraHandle);
        }

        AcquisitionManager::StreamLifetime::StreamLifetime(VmbHandle_t const streamHandle, VmbHandle_t const cameraHandle, AcquisitionManager& acquisitionManager)
        {
            VmbUint32_t value;
            VmbError_t error = VmbPayloadSizeGet(streamHandle, &value);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbPayloadSizeGet");
            }

            if (value == 0)
            {
                throw VmbException("Non-zero payload size required");
            }

            m_payloadSize = static_cast<size_t>(value);

            m_acquisitionLife.reset(new AcquisitionLifetime(cameraHandle, m_payloadSize, acquisitionManager));
        }

        AcquisitionManager::StreamLifetime::~StreamLifetime()
        {
        }

        namespace
        {
            void RunCommand(VmbHandle_t const camHandle, std::string const& command)
            {
                auto error = VmbFeatureCommandRun(camHandle, command.c_str());

                if (error != VmbErrorSuccess)
                {
                    throw VmbException::ForOperation(error, "VmbFeatureCommandRun");
                }

                VmbBool_t done = false;
                while (!done)
                {
                    error = VmbFeatureCommandIsDone(camHandle, command.c_str(), &done);
                    if (error != VmbErrorSuccess)
                    {
                        throw VmbException::ForOperation(error, "VmbFeatureCommandIsDone");
                    }
                }
            }
        }

        AcquisitionManager::AcquisitionLifetime::AcquisitionLifetime(VmbHandle_t const camHandle, size_t payloadSize, AcquisitionManager& acquisitionManager)
            : m_camHandle(camHandle)
        {
            m_frames.reserve(BufferCount);
            for (size_t count = BufferCount; count > 0; --count)
            {
                auto frame = std::unique_ptr<Frame>(new Frame(payloadSize));
                m_frames.emplace_back(std::move(frame));
            }

            VmbError_t error = VmbErrorSuccess;
            for (auto& frame : m_frames)
            {
                AcquisitionContext context(&acquisitionManager);
                context.FillFrame(frame->m_frame);

                error = VmbFrameAnnounce(camHandle, &(frame->m_frame), sizeof(frame->m_frame));
                if (error != VmbErrorSuccess)
                {
                    break;
                }
            }

            if (error != VmbErrorSuccess)
            {
                VmbFrameRevokeAll(camHandle); // error ignored on purpose
                throw VmbException::ForOperation(error, "VmbFrameAnnounce");
            }

            error = VmbCaptureStart(camHandle);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbCaptureStart");
            }

            size_t numberEnqueued = 0;

            for (auto& frame : m_frames)
            {
                error = VmbCaptureFrameQueue(camHandle, &(frame->m_frame), &AcquisitionManager::FrameCallback);
                if (error == VmbErrorSuccess)
                {
                    ++numberEnqueued;
                }
            }

            if (numberEnqueued == 0)
            {
                VmbCaptureEnd(camHandle);
                throw VmbException("Non of the frames could be enqueued using VmbCaptureFrameQueue", error);
            }

            try
            {
                RunCommand(camHandle, "AcquisitionStart");
            }
            catch (VmbException const&)
            {
                VmbCaptureEnd(camHandle);
                throw;
            }
        }

        AcquisitionManager::AcquisitionLifetime::~AcquisitionLifetime()
        {
            try
            {
                RunCommand(m_camHandle, "AcquisitionStop");
            }
            catch (VmbException const&)
            {
            }

            VmbCaptureEnd(m_camHandle);
            VmbFrameRevokeAll(m_camHandle);
        }

        AcquisitionManager::Frame::Frame(size_t payloadSize)
        {
            if (payloadSize > (std::numeric_limits<VmbUint32_t>::max)())
            {
                throw VmbException("payload size outside of allowed range");
            }
            m_frame.buffer = std::malloc(payloadSize);
            if (m_frame.buffer == nullptr)
            {
                throw VmbException("Unable to allocate memory for frame", VmbErrorResources);
            }
            m_frame.bufferSize = static_cast<VmbUint32_t>(payloadSize);
        }

        AcquisitionManager::Frame::~Frame()
        {
            std::free(m_frame.buffer);
        }

    }
}
