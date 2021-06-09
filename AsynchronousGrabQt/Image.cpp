#include <cstdlib>

#include "Image.h"
#include "VmbException.h"

namespace VmbC
{
    namespace Examples
    {
        Image::~Image()
        {
            std::free(m_data);
        }

        void Image::Resize(int newWidth, int newHeight)
        {
            if (newWidth < 0 || newHeight < 0)
            {
                throw VmbException("Negative width or height provided");
            }
            size_t requiredCapacity = static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight);
            if (requiredCapacity > m_capacity)
            {
                if (m_data == nullptr)
                {
                    m_data = static_cast<unsigned char*>(std::malloc(requiredCapacity));
                    if (m_data == nullptr)
                    {
                        throw std::bad_alloc();
                    }
                }
                else
                {
                    void* newData = std::realloc(m_data, requiredCapacity);
                    if (newData == nullptr)
                    {
                        throw std::bad_alloc();
                    }
                    m_data = static_cast<unsigned char*>(newData);
                }
                m_capacity = requiredCapacity;
            }
            m_width = newWidth;
            m_height = newHeight;
        }
    }
}