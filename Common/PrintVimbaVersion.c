#include "PrintVimbaVersion.h"
#include <stdio.h>
#include <VimbaC/Include/VimbaC.h>

/**print VIMBA version information.
* \note if reading the version info fails it is only printed no diagnostics done
*/
void PrintVimbaVersion()
{
    VmbVersionInfo_t version_info;
    VmbError_t result = VmbVersionQuery( &version_info, sizeof(version_info) );
    if( VmbErrorSuccess == result)
    {
        printf("Vimba Version Major: %u Minor: %u Patch: %u\n", version_info.major, version_info.minor,version_info.patch);
    }
    else
    {
        printf("VmbVersionQuery failed with Reason: %x",result);
    }
}


