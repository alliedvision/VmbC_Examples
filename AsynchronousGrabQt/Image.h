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