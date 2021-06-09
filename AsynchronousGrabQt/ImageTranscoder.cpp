#include <algorithm>
#include <limits>
#include <thread>
#include <type_traits>

#include "AcquisitionManager.h"
#include "Image.h"
#include "ImageTranscoder.h"
#include "VmbException.h"

#include "VimbaImageTransform/Include/VmbTransform.h"

namespace VmbC
{
    namespace Examples
    {
        ImageTranscoder::ImageTranscoder(AcquisitionManager& manager)
            : m_acquisitionManager(manager)
        {
            m_thread = std::thread(&ImageTranscoder::TranscodeLoop, std::ref(*this));
        }

        void ImageTranscoder::PostImage(VmbHandle_t const cameraHandle, VmbFrameCallback callback, VmbFrame_t const* frame)
        {
            bool notify = false;

            if (frame != nullptr && frame->receiveStatus == VmbFrameStatusComplete && (frame->receiveFlags & VmbFrameFlagsDimension) == VmbFrameFlagsDimension)
            {

                auto message = std::make_unique<TransformationTask>(cameraHandle, callback, *frame);

                std::lock_guard<std::mutex> lock(m_inputMutex);
                if (m_terminated)
                {
                    message->m_canceled = true;
                }
                else
                {
                    m_task = std::move(message);
                    notify = true;
                }
            }

            if (notify)
            {
                m_inputCondition.notify_one();
            }
        }

        ImageTranscoder::~ImageTranscoder()
        {
            // tell the thread about the shutdown
            {
                std::lock_guard<std::mutex> lock(m_inputMutex);
                m_terminated = true;
            }
            m_inputCondition.notify_all();
            m_thread.join();
        }

        void ImageTranscoder::TranscodeLoopMember()
        {
            std::unique_lock<std::mutex> lock(m_inputMutex);

            while (true)
            {
                if (!m_terminated && !m_task)
                {
                    m_inputCondition.wait(lock, [this]() { return m_terminated || m_task; }); // wait for frame/termination
                }

                if (m_terminated)
                {
                    return;
                }

                {
                    // get task
                    decltype(m_task) task;
                    std::swap(task, m_task);

                    lock.unlock();

                    if (task)
                    {
                        TranscodeImage(*task);
                    }

                    lock.lock();

                    if (m_terminated)
                    {
                        // got terminated during conversion -> don't reenqueue frames
                        task->m_canceled = true;
                        return;
                    }
                }
            }

        }

        void ImageTranscoder::TranscodeImage(TransformationTask& task)
        {
            VmbFrame_t const& frame = task.m_frame;
            VmbImage imageIn;
            if (VmbSetImageInfoFromPixelFormat(frame.pixelFormat, frame.width, frame.height, &imageIn) != VmbErrorSuccess)
            {
                return;
            }
            imageIn.Data = frame.buffer;

            // allocate new image, if necessary
            if (!m_transformTarget)
            {
                m_transformTarget = std::make_unique<Image>();
            }
            try
            {
                m_transformTarget->Resize(static_cast<int>(frame.width), static_cast<int>(frame.height));
            }
            catch (VmbException const&)
            {
                // todo ?
                return;
            }
            catch (std::bad_alloc const&)
            {
                // todo ?
                return;
            }

            VmbImage imageOut;
            if (VmbSetImageInfoFromPixelFormat(VmbPixelFormatRgb8, frame.width, frame.height, &imageOut) != VmbErrorSuccess)
            {
                return;
            }
            imageOut.Data = m_transformTarget->GetMutableData();

            if (VmbImageTransform(&imageIn, &imageOut, nullptr, 0) == VmbErrorSuccess)
            {
                m_acquisitionManager.ConvertedFrameReceived(m_transformTarget);
            }
            else
            {
                // todo ?
            }
        }

        void ImageTranscoder::TranscodeLoop(ImageTranscoder& transcoder)
        {
            transcoder.TranscodeLoopMember();
        }

        ImageTranscoder::TransformationTask::TransformationTask(VmbHandle_t const cameraHandle, VmbFrameCallback callback, VmbFrame_t const& frame)
            : m_cameraHandle(cameraHandle),
            m_callback(callback),
            m_frame(frame)
        {
        }

        ImageTranscoder::TransformationTask::~TransformationTask()
        {
            if (!m_canceled)
            {
                VmbCaptureFrameQueue(m_cameraHandle, &m_frame, m_callback);
            }
        }
    }
}