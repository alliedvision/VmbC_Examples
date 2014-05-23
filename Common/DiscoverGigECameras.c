#include "DiscoverGigECameras.h"
#include <stdio.h>
#include <VimbaC/Include/VimbaC.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

/**Discover GigE cameras if GigE TL is present.
*/

VmbError_t DiscoverGigECameras()
{
    VmbError_t  err     = VmbErrorSuccess;
    VmbBool_t   bIsGigE = 0;

    err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );            // Is Vimba connected to a GigE transport layer?
    if ( VmbErrorSuccess == err )
    {
        if( bIsGigE )
        {
            err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnceWait");   // Send discovery packets to GigE cameras
            if ( VmbErrorSuccess != err )
            {
                printf( "Could not ping GigE cameras over the network. Reason: %d\n", err );
            }
        }
    }
    else
    {
        printf( "Could not query Vimba for the presence of a GigE transport layer. Reason: %d\n\n", err);
    }

    return err;
}
