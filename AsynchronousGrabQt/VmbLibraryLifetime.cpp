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
