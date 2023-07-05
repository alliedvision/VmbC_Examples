/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
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
