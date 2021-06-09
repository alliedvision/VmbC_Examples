#include "VmbException.h"
#include "VmbLibraryLifetime.h"

namespace VmbC
{
    namespace Examples
    {

        VmbLibraryLifetime::VmbLibraryLifetime()
        {
            // TODO change startup parameter to nullptr
            VmbError_t startupError = VmbStartup(L"C:/Users/Fabian Klein/Desktop/Deliverables/run/sv.gentl.x64.cti");
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
