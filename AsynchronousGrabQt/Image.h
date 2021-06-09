#ifndef ASYNCHRONOUSGRAB_C_IMAGE_H
#define ASYNCHRONOUSGRAB_C_IMAGE_H

namespace VmbC
{
    namespace Examples
    {
        class ImageTranscoder;

        class Image
        {
        public:
            Image() = default;
            ~Image();

            Image(Image const&) = delete;
            Image& operator=(Image const&) = delete;

            Image(Image const&&) = delete;
            Image& operator=(Image const&&) = delete;

            int GetWidth() const noexcept { return m_width; }
            int GetHeight() const noexcept { return m_height; }
            int GetBytesPerLine() const noexcept { return m_width * 3; }

            unsigned char const* GetData() const noexcept { return m_data; }
            
            void Resize(int newWidth, int newHeight);

        private:
            unsigned char* GetMutableData() const noexcept { return m_data; }

            friend class ImageTranscoder;

            unsigned char* m_data { nullptr };
            size_t m_capacity { 0 };
            int m_width{ 0 };
            int m_height{ 0 };

        };
    }
}

#endif