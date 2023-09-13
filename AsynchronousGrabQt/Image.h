/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of a class responsible for accessing VmbImageTransform
 *        functionality
 */

#ifndef ASYNCHRONOUSGRAB_C_IMAGE_H
#define ASYNCHRONOUSGRAB_C_IMAGE_H

#include <VmbC/VmbC.h>
#include <VmbImageTransform/VmbTransformTypes.h>

namespace VmbC
{
    namespace Examples
    {

        /**
         * \brief Image data that can be used as source and target for image
         *        transformations via VmbImageTransform library
         */
        class Image
        {
        public:
            /**
             * \brief creates an image with a given pixel format that has
             *        capacity 0
             */
            Image(VmbPixelFormat_t pixelFormat = VmbPixelFormatLast) noexcept;

            /**
             * \brief initializes the image with frame data received from VmbC;
             *        does not take ownership of the data
             */
            Image(VmbFrame_t const& frame);

            ~Image();

            Image(Image const&) = delete;
            Image& operator=(Image const&) = delete;

            Image(Image const&&) = delete;
            Image& operator=(Image const&&) = delete;

            int GetWidth() const noexcept { return m_image.ImageInfo.Width; }

            int GetHeight() const noexcept { return m_image.ImageInfo.Height; }

            /**
             * \brief gets the bytes used for one image line for use in the transformation target/QImage constructor.
             *
             * \warning This function does not work properly for images using a number of bits per pixel that is not divisible by 8.
             *          We only use it to determine the size of RGBA/BGRA images in this example.
             */
            int GetBytesPerLine() const noexcept
            {
                return m_image.ImageInfo.Stride * (m_image.ImageInfo.PixelInfo.BitsPerPixel / 8);
            }

            /**
             * \brief gets readonly access to the raw image data
             */
            unsigned char const* GetData() const noexcept
            {
                return static_cast<unsigned char const*>(m_image.Data);
            }

            /**
             * \brief convert the data of conversionImage to the pixel format of this image
             */
            void Convert(Image const& conversionSource);
        private:
            bool m_dataOwned{true};
            VmbImage m_image;
            VmbPixelFormat_t m_pixelFormat;

            /**
             * \brief the size of the currently owned buffer in bytes
             */
            size_t m_capacity { 0 };

        };
    }
}

#endif
