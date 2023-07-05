/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Implementation of ::VmbC::Examples::Image
 */

#include <cstdlib>

#include <VmbImageTransform/VmbTransform.h>

#include "Image.h"
#include "VmbException.h"

namespace VmbC
{
    namespace Examples
    {

        Image::Image(VmbPixelFormat_t pixelFormat) noexcept
            : m_pixelFormat(pixelFormat)
        {
            m_image.Size = sizeof(m_image);
            m_image.Data = nullptr;
        }

        Image::Image(VmbFrame_t const& frame)
            : m_dataOwned(false),
            m_pixelFormat(frame.pixelFormat)
        {
            m_image.Size = sizeof(m_image);
            m_image.Data = frame.imageData;

            auto error = VmbSetImageInfoFromPixelFormat(frame.pixelFormat, frame.width, frame.height, &m_image);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbSetImageInfoFromPixelFormat");
            }
        }

        Image::~Image()
        {
            if (m_dataOwned)
            {
                std::free(m_image.Data);
            }
        }

        void Image::Convert(Image const& conversionSource)
        {
            if (&conversionSource == this)
            {
                return;
            }
            auto error = VmbSetImageInfoFromPixelFormat(m_pixelFormat, conversionSource.GetWidth(), conversionSource.GetHeight(), &m_image);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbSetImageInfoFromPixelFormat");
            }

            size_t requiredCapacity = GetBytesPerLine() * GetHeight();
            if (requiredCapacity > m_capacity)
            {
                void* newData;
                if (m_image.Data == nullptr)
                {
                    newData = std::malloc(requiredCapacity);
                }
                else
                {
                    newData = std::realloc(m_image.Data, requiredCapacity);
                }

                if (newData == nullptr)
                {
                    throw std::bad_alloc();
                }

                m_image.Data = newData;
                m_capacity = requiredCapacity;
            }

            error = VmbImageTransform(&conversionSource.m_image, &m_image, nullptr, 0);
            if (error != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(error, "VmbImageTransform");
            }
        }
    }
}
