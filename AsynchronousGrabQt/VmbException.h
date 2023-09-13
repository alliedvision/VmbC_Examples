/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of a custom exception type used in this example
 */

#ifndef ASYNCHRONOUSGRAB_C_VMB_EXCEPTION_H
#define ASYNCHRONOUSGRAB_C_VMB_EXCEPTION_H

#include <exception>
#include <string>

#include <VmbC/VmbC.h>

namespace VmbC
{
    namespace Examples
    {
        /**
         * \brief A custom exception type used in this example
         */
        class VmbException : public std::exception
        {
        public:
            VmbException() = default;
            VmbException(VmbException&&) = default;
            VmbException& operator=(VmbException&&) = default;

            /**
             * \brief create an exception with a custom message and a given VmbC error code
             */
            VmbException(std::string const& message, VmbError_t exitCode = VmbErrorOther);

            /**
             * \brief create an exception with a standardized message for
             *        failure of a VmbC API function
             * \param[in] errorCode the error code returned by the function
             * \param[in] functionName the name of the function returning the error
             */
            static VmbException ForOperation(VmbError_t errorCode, std::string const& functionName);

            const char* what() const noexcept override
            {
                return m_errorMessage.c_str();
            }

            VmbError_t GetExitCode() const noexcept
            {
                return m_exitCode;
            }
        private:
            VmbError_t m_exitCode{ VmbErrorSuccess };
            std::string m_errorMessage;
        };
    }
}

#endif
