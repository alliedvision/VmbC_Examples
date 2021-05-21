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
 * \brief Define a class containing the result of API calls.
 * \author Fabian Klein
 */

#ifndef ASYNCHRONOUSGRAB_C_API_CALL_RESULT_H
#define ASYNCHRONOUSGRAB_C_API_CALL_RESULT_H

#include <string>

#include "VimbaC/Include/VimbaC.h"

namespace VmbC
{
    namespace Examples
    {

        /**
         * \brief describes the result of an API call including an error message
         *
         * Class containing the result of a operation of the VmbC API. Wraps the message
         * and the return code. This class is convertible to a bool indicating success (true)
         * or failure.
         */
        class ApiCallResult
        {
        public:
            ApiCallResult(VmbError_t returnCode, const std::string& message = "")
                : m_message(message), m_returnCode(returnCode)
            {
            }

            ApiCallResult()
                : m_message(""), m_returnCode(0)
            {
            }

            ApiCallResult(const ApiCallResult&) = delete;
            ApiCallResult& operator=(const ApiCallResult&) = delete;

            ApiCallResult(ApiCallResult&&) = default;

            ApiCallResult& operator=(ApiCallResult&&) = default;

            /**
             * \brief getter for the error message
             */
            inline std::string const& GetMessage() const
            {
                return m_message;
            }

            /**
             * \brief getter for the error code
             */
            inline VmbError_t GetErrorCode() const
            {
                return m_returnCode;
            }

            inline operator bool() const
            {
                return m_returnCode == VmbErrorSuccess;
            }

        private:
            std::string const m_message;

            VmbError_t const m_returnCode;
        };

    } // namespace Examples
} // namespace VmbC

#endif
