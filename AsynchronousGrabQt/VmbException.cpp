/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Implementation of ::VmbC::Examples::VmbException
 */

#include <cassert>

#include "VmbException.h"

namespace VmbC
{
    namespace Examples
    {

        VmbException::VmbException(std::string const& message, VmbError_t exitCode)
            : m_exitCode(exitCode),
            m_errorMessage(message)
        {
            assert(exitCode != VmbErrorSuccess);
        }

        VmbException VmbException::ForOperation(VmbError_t exitCode, std::string const& operation)
        {
            return VmbException(operation + " call unsuccessful; exit code " + std::to_string(exitCode), exitCode);
        }
    }
}
