#ifndef ASYNCHRONOUSGRAB_C_IMAGE_TRANSCODER_H
#define ASYNCHRONOUSGRAB_C_IMAGE_TRANSCODER_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "VimbaC/Include/VimbaC.h"

namespace VmbC
{
    namespace Examples
    {
        class AcquisitionManager;
        class Image;

        class ImageTranscoder
        {
        public:
            ImageTranscoder(AcquisitionManager& manager);
            ~ImageTranscoder();

            /**
             * \brief Asynchronously schedule the conversion of a frame 
             * \param callback the callback to use the old frame that is reenqueued
             */
            void PostImage(VmbHandle_t const cameraHandle, VmbFrameCallback callback, VmbFrame_t const* frame);
        private:
            struct TransformationTask
            {
                VmbHandle_t m_cameraHandle;
                VmbFrameCallback m_callback;
                VmbFrame_t const& m_frame;
                bool m_canceled{ false };
                
                TransformationTask(VmbHandle_t const cameraHandle, VmbFrameCallback callback, VmbFrame_t const& frame);

                ~TransformationTask();
            };

            static void TranscodeLoop(ImageTranscoder& transcoder);
            void TranscodeLoopMember();
            void TranscodeImage(TransformationTask& task);

            AcquisitionManager& m_acquisitionManager;

            std::mutex m_inputMutex;
            std::condition_variable m_inputCondition;
            std::unique_ptr<TransformationTask> m_task;
            std::unique_ptr<Image> m_transformTarget;
            bool m_terminated { false };
            bool m_running{ false };

            std::thread m_thread;
        };
    }
}

#endif