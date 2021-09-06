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