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
