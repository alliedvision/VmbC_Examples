/*=============================================================================
  Copyright (C) 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ForceIP.h

  Description: The ForceIP example will assign an IP address to a camera
               that is identified by its MAC address. It utilizes only
               GenTL features. This approach is useful when a camera has
               an invalid IP configuration and cannot be accessed through
               the network anymore.

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

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
#endif

#include <ForceIP.h>

#include <VimbaC/Include/VimbaC.h>


// Converts a hexadecimal MAC address into its decimal representation.
// Leaves decimal addresses untouched.
unsigned long long mac_addr( const char* strMAC )
{
    unsigned long long nMAC;

    if (    sscanf( strMAC, "0x%llx", &nMAC )
        || sscanf( strMAC, "%lld", &nMAC ))
    {
        return nMAC;
    }
    else
    {
        return 0;
    }
}

void ForceIP( char* strMAC, char* strIP, char* strSubnet, char* strGateway )
{
    VmbError_t err = VmbStartup();                                                                                          // Initialize the Vimba API
    VmbBool_t bIsGigE = 0;                                                                                                  // GigE transport layer present
    VmbHandle_t hCam = NULL;                                                                                                // A camera handle
    unsigned long long nMAC = mac_addr( strMAC );                                                                           // The MAC address of the camera
    unsigned long nIP = inet_addr( strIP );                                                                                 // The future IP address of the camera
    unsigned long nSubnet = inet_addr( strSubnet );                                                                         // The future subnet mask of the camera
    unsigned long nGateway = strGateway != NULL ? inet_addr( strGateway) : 0;                                               // A possible gateway
    char* strMACPadded = (char*)malloc( 13 * sizeof( *strMACPadded ));                                                      // The MAC address with a fixed length of 12

    if ( VmbErrorSuccess == err )
    {
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );                                                // Is Vimba connected to a GigE transport layer?
        if ( VmbErrorSuccess == err )
        {
            if( bIsGigE )
            {
                err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");                                           // Send discovery packets to GigE cameras
                if ( VmbErrorSuccess == err )
                {
#ifdef WIN32                                                                                                                // And wait for them to return
                    Sleep( 200 );
#else
                    usleep( 200 * 1000 );
#endif
                    if ( sprintf( strMACPadded, "%012llx", nMAC ))                                                          // Opening the cam via MAC address requires a fixed format
                    {
                        err = VmbCameraOpen( strMACPadded, VmbAccessModeFull, &hCam );                                      // Check whether the camera is opened already
                        if ( VmbErrorSuccess == err )
                        {
                            VmbCameraClose( hCam );
                            err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressMAC", nMAC );                           // Send MAC address to TL
                            if ( VmbErrorSuccess == err )
                            {
                                err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressIP", nIP );                         // Send new IP address to TL
                                if ( VmbErrorSuccess == err )
                                {
                                    err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressSubnetMask", nSubnet );         // Send new subnet mask to TL
                                    if ( VmbErrorSuccess == err )
                                    {
                                        if( 0 != nGateway )
                                        {
                                            err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressGateway", nGateway );   // Send gateway address to TL
                                            if ( VmbErrorSuccess != err )
                                            {
                                                printf( "Could not prepare the gateway settings. Reason: %d\n\n", err );
                                            }
                                        }

                                        if ( VmbErrorSuccess == err )
                                        {
                                            err = VmbFeatureCommandRun( gVimbaHandle, "GeVForceIPAddressSend" );            // Finally execute the command to write all settings to cam
                                            if ( VmbErrorSuccess == err )
                                            {
                                                printf( "IP address successfully changed\n\n" );
                                            }
                                            else
                                            {
                                                printf( "Could not set a new IP address. Reason: %d\n\n", err );
                                            }
                                        }
                                    }
                                    else
                                    {
                                        printf( "Could not prepare the subnet settings. Reason: %d\n\n", err );
                                    }
                                }
                                else
                                {
                                    printf( "Could not prepare the IP address settings. Reason: %d\n\n", err );
                                }
                            }
                            else
                            {
                                printf( "Could not prepare the MAC address settings. Reason: %d\n\n", err );
                            }
                        }
                        else if ( VmbErrorInvalidAccess == err )
                        {
                            printf( "Cannot set IP address because camera is already opened.\n\n" );
                        }
                        else
                        {
                            printf( "Cannot access camera.\n\n" );
                        }
                    }
                    else
                    {
                        printf( "Malformed MAC address.\n\n" );
                    }
                }
                else
                {
                    printf( "Could not ping GigE cameras over the network. Reason: %d\n", err );
                }
            }
            else
            {
                printf( "No GigE transport layer present.\n\n" );
            }
        }
        else
        {
            printf( "Could not query Vimba for the presence of a GigE transport layer. Reason: %d\n\n", err );
        }

        VmbShutdown();                                                                                                      // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n\n", err );
    }

    free (strMACPadded);
}
