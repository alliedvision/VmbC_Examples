/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
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
            error = VmbCameraInfoQueryByHandle(m_cameraHandle, &refreshedCameraInfo, sizeof(refreshedCameraInfo));
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
                if (VmbErrorSuccess == VmbFeatureCommandRun(refreshedCameraInfo.streamHandles[0], AdjustPackageSizeCommand))
                {
                    VmbBool_t isCommandDone = VmbBoolFalse;
                    do
                    {
                        if (VmbErrorSuccess != VmbFeatureCommandIsDone(refreshedCameraInfo.streamHandles[0],
                            AdjustPackageSizeCommand,
                            &isCommandDone))
                        {
                            break;
                        }
                    } while (VmbBoolFalse == isCommandDone);
                    VmbInt64_t packetSize = 0;
                    VmbFeatureIntGet(refreshedCameraInfo.streamHandles[0], "GVSPPacketSize", &packetSize);
                    printf("GVSPAdjustPacketSize: %lld\n", packetSize);
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
            // Evaluate required alignment for frame buffer in case announce frame method is used
            VmbInt64_t nStreamBufferAlignment = 1;  // Required alignment of the frame buffer
            if (VmbErrorSuccess != VmbFeatureIntGet(streamHandle, "StreamBufferAlignment", &nStreamBufferAlignment))
                nStreamBufferAlignment = 1;

            if (nStreamBufferAlignment < 1)
                nStreamBufferAlignment = 1;

            m_payloadSize = static_cast<size_t>(value);
            size_t bufferAlignment = static_cast<size_t>(nStreamBufferAlignment);
            m_acquisitionLife.reset(new AcquisitionLifetime(cameraHandle, m_payloadSize, bufferAlignment, acquisitionManager));
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

        AcquisitionManager::AcquisitionLifetime::AcquisitionLifetime(VmbHandle_t const camHandle, size_t payloadSize, size_t nBufferAlignment, AcquisitionManager& acquisitionManager)
            : m_camHandle(camHandle)
        {
            m_frames.reserve(BufferCount);
            for (size_t count = BufferCount; count > 0; --count)
            {
                auto frame = std::unique_ptr<Frame>(new Frame(payloadSize, nBufferAlignment));
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
            VmbCaptureQueueFlush(m_camHandle);
            VmbFrameRevokeAll(m_camHandle);
        }

        AcquisitionManager::Frame::Frame(size_t payloadSize, size_t bufferAlignment)
        {
            if (payloadSize > (std::numeric_limits<VmbUint32_t>::max)())
            {
                throw VmbException("payload size outside of allowed range");
            }

            const size_t requestedAlignment = (bufferAlignment > 0) ? bufferAlignment : 1;
            const size_t requestedMask = (requestedAlignment - 1);

            // Alignment must be power of 2
            assert(((requestedAlignment & requestedMask) == 0));

            // Pointer size will always be a power of 2
            // We enforce an alignment of sizeof(void*) since aligned_alloc for macOS does not accept 1 as alignment value
            const size_t mask = (sizeof(void *) - 1) | requestedMask;
            const size_t alignment = mask + 1;
            const size_t offset = payloadSize & mask;
            const size_t offsetToNext = (alignment - offset) & mask;
            const size_t alignedPayloadSize = payloadSize + offsetToNext;
#ifdef _WIN32
            m_frame.buffer = (unsigned char*)_aligned_malloc(alignedPayloadSize, alignment);
#else
            m_frame.buffer = (unsigned char*)aligned_alloc(alignment, alignedPayloadSize);
#endif
            if (m_frame.buffer == nullptr)
            {
                throw VmbException("Unable to allocate memory for frame", VmbErrorResources);
            }
            m_frame.bufferSize = VmbUint32_t(requestedAlignment > 1 ? alignedPayloadSize : payloadSize);
        }

        AcquisitionManager::Frame::~Frame()
        {
#ifdef _WIN32
            _aligned_free(m_frame.buffer);
#else
            std::free(m_frame.buffer);
#endif
        }

    }
}
