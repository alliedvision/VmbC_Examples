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