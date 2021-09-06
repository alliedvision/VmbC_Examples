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
 * \brief Definition of a class responsible converting VmbC image data to
 *        QPixmap in a background thread
 */

#ifndef ASYNCHRONOUSGRAB_C_IMAGE_TRANSCODER_H
#define ASYNCHRONOUSGRAB_C_IMAGE_TRANSCODER_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <QSize>

#include <VmbC/VmbC.h>

namespace VmbC
{
    namespace Examples
    {
        class AcquisitionManager;
        class Image;

        /**
         * \brief Class responsible converting VmbC image data to
         *        QPixmap in a background thread
         */
        class ImageTranscoder
        {
        public:
            ImageTranscoder(AcquisitionManager& manager);
            ~ImageTranscoder();

            /**
             * \brief Asynchronously schedule the conversion of a frame 
             * \param callback the callback to use the old frame that is reenqueued
             */
            void PostImage(VmbHandle_t streamHandle, VmbFrameCallback callback, VmbFrame_t const* frame);

            /**
             * \brief start with the conversion process 
             */
            void Start();

            /**
             * \brief stop the conversion process
             */
            void Stop() noexcept;

            /**
             * \brief update the size of the QPixmaps to produce
             */
            void SetOutputSize(QSize size);
        private:
            /**
             * \brief size of QPixmaps to produce 
             */
            QSize m_outputSize;

            /**
             * \brief mutex for guarding access to m_outputSize 
             */
            std::mutex m_sizeMutex;

            /**
             * \brief object holding all required info about a desired
             *        conversion 
             */
            struct TransformationTask
            {
                VmbHandle_t m_streamHandle;
                VmbFrameCallback m_callback;
                VmbFrame_t const& m_frame;

                /**
                 * \brief set to true to prevent reenqueuing the frame after
                 *        the conversion 
                 */
                bool m_canceled{ false };
                
                TransformationTask(VmbHandle_t const streamHandle, VmbFrameCallback callback, VmbFrame_t const& frame);

                ~TransformationTask();
            };

            /**
             * \brief the function to use with std::thread 
             */
            static void TranscodeLoop(ImageTranscoder& transcoder);

            /**
             * \brief contains the actual logic used for executing the
             *        conversions 
             */
            void TranscodeLoopMember();

            /**
             * \brief execute the conversion of a single image
             */ 
            void TranscodeImage(TransformationTask& task);

            /**
             * \brief the object to notify about the conversion results 
             */
            AcquisitionManager& m_acquisitionManager;

            /**
             * \brief mutex guarding the frame data received 
             */
            std::mutex m_inputMutex;

            /**
             * \brief condition variable used to notify the background thread
             *        about new frames; guarded by m_inputMutex  
             */
            std::condition_variable m_inputCondition;

            /**
             * \brief info about the next conversion to do 
             */
            std::unique_ptr<TransformationTask> m_task;

            /**
             * \brief target image for the conversion 
             */
            std::unique_ptr<Image> m_transformTarget;

            /**
             * \brief true, if the background thread should terminate; guarded
             *        by m_inputMutex 
             */
            bool m_terminated { true };

            /**
             * \brief the background thread 
             */
            std::thread m_thread;
        };
    }
}

#endif