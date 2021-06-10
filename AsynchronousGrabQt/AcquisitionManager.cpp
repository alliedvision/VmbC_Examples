#include <cstdlib>
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
            m_openCamera = std::make_unique<CameraAccessLifetime>(cameraInfo, *this);
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

        void VMB_CALL AcquisitionManager::FrameCallback(VmbHandle_t const cameraHandle, VmbFrame_t* frame)
        {
            if (frame != nullptr)
            {
                AcquisitionContext context(*frame);
                if (context.m_acquisitionManager != nullptr)
                {
                    context.m_acquisitionManager->FrameReceived(cameraHandle, frame);
                }
            }
        }

        void AcquisitionManager::FrameReceived(VmbHandle_t const cameraHandle, VmbFrame_t const* frame)
        {
            m_imageTranscoder.PostImage(cameraHandle, &AcquisitionManager::FrameCallback, frame);
        }

        AcquisitionManager::CameraAccessLifetime::CameraAccessLifetime(VmbCameraInfo_t const& camInfo, AcquisitionManager& acquisitionManager)
        {
            VmbError_t error = VmbCameraOpen(camInfo.cameraIdString, VmbAccessModeFull, &m_cameraHandle);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbCameraOpen");
            }

            try
            {
                m_streamLife = std::make_unique<StreamLifetime>(m_cameraHandle, acquisitionManager);
            }
            catch (...)
            {
                VmbCameraClose(m_cameraHandle);
                throw;
            }
        }

        AcquisitionManager::CameraAccessLifetime::~CameraAccessLifetime()
        {
            m_streamLife.reset(); // close stream first
            VmbCameraClose(m_cameraHandle);
        }

        AcquisitionManager::StreamLifetime::StreamLifetime(VmbHandle_t const camHandle, AcquisitionManager& acquisitionManager)
        {
            // todo: get from stream
            VmbInt64_t value;
            VmbError_t error = VmbFeatureIntGet(camHandle, "PayloadSize", &value);
            if (error != VmbErrorSuccess)
            {
                throw VmbException("Could not retrieve PayloadSize camera feature", error);
            }

            if (value < 0 || (static_cast<uint64_t>(value) > (std::numeric_limits<size_t>::max)()))
            {
                throw VmbException("Payload size " + std::to_string(value) + " out of valid range");
            }

            m_payloadSize = static_cast<size_t>(value);

            m_acquisitionLife = std::make_unique<AcquisitionLifetime>(camHandle, m_payloadSize, acquisitionManager);
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
                m_frames.emplace_back(payloadSize);
            }

            VmbError_t error = VmbErrorSuccess;
            for (auto& frame : m_frames)
            {
                AcquisitionContext context(&acquisitionManager);
                context.FillFrame(frame.m_frame);

                error = VmbFrameAnnounce(camHandle, &(frame.m_frame), sizeof(frame.m_frame));
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
                error = VmbCaptureFrameQueue(camHandle, &(frame.m_frame), &AcquisitionManager::FrameCallback);
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

        AcquisitionManager::Frame::Frame()
        {
            m_frame.buffer = nullptr;
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

        AcquisitionManager::Frame::Frame(Frame&& other) noexcept
        {
            std::memcpy(&m_frame, &(other.m_frame), sizeof(m_frame));
            other.m_frame.buffer = nullptr;
        }

        AcquisitionManager::Frame& AcquisitionManager::Frame::operator=(Frame&& other) noexcept
        {
            // free existing buffer
            std::free(m_frame.buffer);

            // get info from other and take ownership of buffer
            std::memcpy(&m_frame, &(other.m_frame), sizeof(m_frame));

            // remove ownership of buffer from other
            other.m_frame.buffer = nullptr;

            return *this;
        }
    }
}