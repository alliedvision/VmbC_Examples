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
            return VmbException(operation + " call unsuccessful; exit code " = std::to_string(exitCode), exitCode);
        }
    }
}
