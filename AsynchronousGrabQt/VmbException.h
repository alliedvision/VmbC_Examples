#ifndef ASYNCHRONOUSGRAB_C_VMB_EXCEPTION_H
#define ASYNCHRONOUSGRAB_C_VMB_EXCEPTION_H

#include <exception>
#include <string>

#include "VimbaC/Include/VimbaC.h"

namespace VmbC
{
    namespace Examples
    {
        class VmbException : public std::exception
        {
        public:
            VmbException(std::string const& message, VmbError_t exitCode = VmbErrorOther);

            static VmbException ForOperation(VmbError_t exitCode, std::string const& operation);

            const char* what() const noexcept override
            {
                return m_errorMessage.c_str();
            }

            VmbError_t GetExitCode() const noexcept
            {
                return m_exitCode;
            }
        private:
            VmbError_t m_exitCode;
            std::string m_errorMessage;
        };
    }
}

#endif