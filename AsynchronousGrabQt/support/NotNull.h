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
 * \brief Definition of a wrapper class for pointer indicating non-null values
 */

#ifndef ASYNCHRONOUSGRAB_C_SUPPORT_NOTNULL_H
#define ASYNCHRONOUSGRAB_C_SUPPORT_NOTNULL_H

#include <cassert>
#include <type_traits>

namespace VmbC
{
    namespace Examples
    {
        inline namespace Support
        {

            /**
             * \brief a pointer where 0 is not a valid value
             * \tparam T the type the pointer points to
             */
            template<typename T>
            class NotNull
            {
            public:
                NotNull(std::nullptr_t) = delete;

                inline NotNull(T* pointer) noexcept
                    : m_ptr(pointer)
                {
                    assert(pointer != nullptr);
                }

                template<typename U>
                inline NotNull(NotNull<U> const& other) noexcept
                    : m_ptr(static_cast<U*>(other))
                {
                    static_assert(std::is_assignable<decltype(m_ptr)&, U*>::value, "Incompatible pointer types");
                }

                template<typename U>
                NotNull<T>& operator=(NotNull<U> const& other) noexcept
                {
                    static_assert(std::is_assignable<decltype(m_ptr)&, U*>::value, "Incompatible pointer types");
                    m_ptr = static_cast<U*>(other);
                    return *this;
                }

                inline NotNull(NotNull<T> const& other) noexcept = default;
                inline NotNull& operator=(NotNull<T> const& other) noexcept = default;

                NotNull(NotNull<T>&&) = delete;
                NotNull<T>& operator=(NotNull<T>&&) = delete;

                inline operator T* () const noexcept
                {
                    return m_ptr;
                }

                inline T* operator->() const noexcept
                {
                    return m_ptr;
                }

                inline bool operator==(T* pointer) const noexcept
                {
                    return (pointer == m_ptr);
                }

                inline bool operator!=(T* pointer) const noexcept
                {
                    return (pointer != m_ptr);
                }
            private:
                T* m_ptr;
            };
        }
    }
}

#endif