#ifndef ASYNCHRONOUSGRAB_C_ACQUISITION_MANAGER_H
#define ASYNCHRONOUSGRAB_C_ACQUISITION_MANAGER_H

#include <memory>
#include <vector>

#include "VimbaC/Include/VimbaC.h"

#include "ImageTranscoder.h"

class MainWindow;

namespace VmbC
{
    namespace Examples
    {
        class Image;

        class AcquisitionManager
        {
        public:
            static constexpr VmbUint32_t BufferCount = 10;

            bool IsAcquisitionActive() const noexcept
            {
                return static_cast<bool>(m_openCamera);
            }

            void StartAcquisition(VmbCameraInfo_t const& cameraInfo);

            void StopAcquisition();

            AcquisitionManager(MainWindow& renderWindow);

            ~AcquisitionManager();

            void ConvertedFrameReceived(std::unique_ptr<Image>& image);

        private:
            MainWindow& m_renderWindow;

            class StreamLifetime;

            class CameraAccessLifetime
            {
            public:
                CameraAccessLifetime(VmbCameraInfo_t const& camInfo, AcquisitionManager& acquisitionManager);
                ~CameraAccessLifetime();
            private:
                VmbHandle_t m_cameraHandle {};
                std::unique_ptr<StreamLifetime> m_streamLife;
            };

            class AcquisitionLifetime;

            class StreamLifetime
            {
            public:
                StreamLifetime(VmbHandle_t const camHandle, AcquisitionManager& acquisitionManager);
                ~StreamLifetime();

            private:
                std::unique_ptr<AcquisitionLifetime> m_acquisitionLife;
                size_t m_payloadSize;
            };

            struct Frame
            {
                Frame();
                Frame(size_t payloadSize);
                ~Frame();

                Frame(Frame const&) = delete;
                Frame& operator=(Frame const&) = delete;

                Frame(Frame&& other) noexcept;
                Frame& operator=(Frame&& other) noexcept;

                VmbFrame_t m_frame;
            };

            class AcquisitionLifetime
            {
            public:
                AcquisitionLifetime(VmbHandle_t const camHandle, size_t payloadSize, AcquisitionManager& acquisitionManager);
                ~AcquisitionLifetime();

            private:
                std::vector<Frame> m_frames;
                VmbHandle_t m_camHandle;
            };

            std::unique_ptr<CameraAccessLifetime> m_openCamera;
            ImageTranscoder m_imageTranscoder;

            static void VMB_CALL FrameCallback(VmbHandle_t const cameraHandle, VmbFrame_t* frame);

            void FrameReceived(VmbHandle_t const cameraHandle, VmbFrame_t const* frame);
        };
    }
}

#endif