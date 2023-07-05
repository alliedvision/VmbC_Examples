/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of a class responsible for managing the acquisition and
 *        scheduling transformation and transfering converted frames to the
 *        gui.
 */

#ifndef ASYNCHRONOUSGRAB_C_ACQUISITION_MANAGER_H
#define ASYNCHRONOUSGRAB_C_ACQUISITION_MANAGER_H

#include <memory>
#include <vector>

#include <QPixmap>
#include <QSize>

#include <VmbC/VmbC.h>

#include "ImageTranscoder.h"

class MainWindow;

namespace VmbC
{
    namespace Examples
    {
        class Image;

        /**
         * \brief Responsible for starting/stoping the acquisition, scheduling
         *        the transformation of frames received during the acquisition
         *        and transfering the results to the qt window
         */
        class AcquisitionManager
        {
        public:
            static constexpr VmbUint32_t BufferCount = 10;

            /**
             * \return true, if currently an acquisition is running
             */
            bool IsAcquisitionActive() const noexcept
            {
                return static_cast<bool>(m_openCamera);
            }

            /**
             * \brief start the acquistion for a given camera
             */
            void StartAcquisition(VmbCameraInfo_t const& cameraInfo);

            /**
             * \brief stop the acquistion that is currently running
             */
            void StopAcquisition() noexcept;

            AcquisitionManager(MainWindow& renderWindow);

            ~AcquisitionManager();

            /**
             * \brief notifies this object about a frame available for rendering
             */
            void ConvertedFrameReceived(QPixmap image);

            /**
             * \brief informs this object about the change of the desired output
             *        size
             */
            void SetOutputSize(QSize size);

        private:
            MainWindow& m_renderWindow;

            class StreamLifetime;

            /**
             * \brief class responsible for opening/closing a camera
             */
            class CameraAccessLifetime
            {
            public:
                /**
                 * \brief opens the camera and starts the acquisition immediately
                 */
                CameraAccessLifetime(VmbCameraInfo_t const& camInfo, AcquisitionManager& acquisitionManager);

                /**
                 * \brief stops acquistion and closes the camera
                 */
                ~CameraAccessLifetime();
            private:
                /**
                 * \brief stores the remote device handle
                 */
                VmbHandle_t m_cameraHandle {};
                std::unique_ptr<StreamLifetime> m_streamLife;
            };

            class AcquisitionLifetime;

            /**
             * \brief handles acquiring any crucial information for a stream required for the acquistion
             */
            class StreamLifetime
            {
            public:
                StreamLifetime(VmbHandle_t streamHandle, VmbHandle_t cameraHandle, AcquisitionManager& acquisitionManager);
                ~StreamLifetime();

            private:
                std::unique_ptr<AcquisitionLifetime> m_acquisitionLife;
                size_t m_payloadSize;
            };

            /**
             * \brief manages a VmbFrame_t and its memory
             */
            struct Frame
            {
                Frame(size_t payloadSize, size_t bufferAlignment);
                ~Frame();

                Frame(Frame const&) = delete;
                Frame& operator=(Frame const&) = delete;

                Frame(Frame&& other) = delete;
                Frame& operator=(Frame&& other) = delete;

                VmbFrame_t m_frame;
            };

            /**
             * \brief handles starting/stopping of a stream
             */
            class AcquisitionLifetime
            {
            public:
                AcquisitionLifetime(VmbHandle_t const camHandle, size_t payloadSize, size_t bufferAlignment, AcquisitionManager& acquisitionManager);
                ~AcquisitionLifetime();

            private:
                std::vector<std::unique_ptr<Frame>> m_frames;
                VmbHandle_t m_camHandle;
            };

            /**
             * \brief stores the camera currently acquiring
             */
            std::unique_ptr<CameraAccessLifetime> m_openCamera;

            /**
             * \brief Object used for transforming frames to QPixmaps
             */
            ImageTranscoder m_imageTranscoder;

            /**
             * \brief callback to receive the notification about new frames from VmbC
             */
            static void VMB_CALL FrameCallback(VmbHandle_t cameraHandle, VmbHandle_t streamHandle, VmbFrame_t* frame);

            /**
             * \brief member function that receives the notification new frames from VmbC
             */
            void FrameReceived(VmbHandle_t const cameraHandle, VmbFrame_t const* frame);
        };
    }
}

#endif
