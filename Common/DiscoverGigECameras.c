/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        DiscoverGigECameras.c

  Description: Discover GigE cameras if GigE TL is present.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

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
            err = VmbFeatureIntSet( gVimbaHandle, "GeVDiscoveryAllDuration", 250 ); // Set the waiting duration for discovery packets to return. If not set the default of 150 ms is used.
            if ( VmbErrorSuccess == err )
            {
                err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce" );  // Send discovery packets to GigE cameras and wait 250 ms until they are answered
                if ( VmbErrorSuccess != err )
                {
                    printf( "Could not ping GigE cameras over the network. Reason: %d\n", err );
                }
            }
            else
            {
                printf( "Could not set the discovery waiting duration. Reason: %d\n", err );
            }
        }
    }
    else
    {
        printf( "Could not query Vimba for the presence of a GigE transport layer. Reason: %d\n\n", err );
    }

    return err;
}
