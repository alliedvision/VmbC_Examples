/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of an entry of the event log view
 */

#ifndef ASYNCHRONOUSGRAB_C_LOG_ENTRY_H
#define ASYNCHRONOUSGRAB_C_LOG_ENTRY_H

#include <VmbC/VmbC.h>

#include <string>

namespace VmbC
{
    namespace Examples
    {

        class LogEntry
        {
        public:
            LogEntry(std::string const& message, VmbError_t error = VmbErrorSuccess)
                : m_frame(message), m_errorCode(error)
            {
            }

            std::string const& GetMessage() const noexcept
            {
                return m_frame;
            }

            VmbError_t GetErrorCode() const noexcept
            {
                return m_errorCode;
            }
        private:
            std::string m_frame;
            VmbError_t m_errorCode;
        };
    } // namespace Examples
} // namespace VmbC

#endif
