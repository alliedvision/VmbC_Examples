/**
 * \date 2021-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Implementation of ::VmbC::Examples::VmbLibraryLifetime
 */

#include "VmbException.h"
#include "VmbLibraryLifetime.h"

namespace VmbC
{
    namespace Examples
    {

        VmbLibraryLifetime::VmbLibraryLifetime()
        {
            VmbError_t startupError = VmbStartup(nullptr);
            if (startupError != VmbErrorSuccess)
            {
                throw VmbException::ForOperation(startupError, "VmbStartup");
            }
        }

        VmbLibraryLifetime::~VmbLibraryLifetime()
        {
            VmbShutdown();
        }
    }
}
