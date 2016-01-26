/*=============================================================================
  Copyright (C) 2013 - 2016 Allied Vision Technologies.  All Rights Reserved.

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
#include "../../Common/PrintVimbaVersion.h"

//
// Converts a hexadecimal MAC address into its decimal representation.
// Leaves decimal addresses untouched.
//
// Parameters:
//  [in]    strMAC          The hexadecimal (with preceding 0x) or decimal MAC 
//                          address to be converted to decimal. As string.
//
// Returns:
//  0 in case of error
//  The decimal representation of the MAC address on success as integer
//
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

//
// Starts Vimba API
// Seeks a GigE camera by its MAC address on the network
// Sets the found camera's 
// - IP address
// - subnet mask
// - gateway
//
// Parameters:
//  [in]    strMAC          The MAC address of the camera to work on in decimal
//                          or hex (with preceding 0x) representation
//  [in]    strIP           The desired IP address for the camera
//  [in]    strSubnet       The desired subnet mask of the IP address
//  [in]    strGateway      The desired gateway. Optional, can be 0
//
void ForceIP( char* strMAC, char* strIP, char* strSubnet, char* strGateway )
{
    VmbError_t          err             = VmbErrorSuccess;
    VmbBool_t           bIsGigE         = 0;
    VmbHandle_t         hCam            = NULL;
    unsigned long long  nMAC            = 0;
    unsigned long       nIP             = 0;
    unsigned long       nSubnet         = 0;
    unsigned long       nGateway        = 0;
    
    err = VmbStartup();                                                                                                     // Initialize the Vimba API
    PrintVimbaVersion();                                                                                                    // Print Vimba Version
    nMAC            = mac_addr( strMAC );                                                                                   // The MAC address of the camera
    nIP             = inet_addr( strIP );                                                                                   // The future IP address of the camera
    nSubnet         = inet_addr( strSubnet );                                                                               // The future subnet mask of the camera
    nGateway        = strGateway != NULL ? inet_addr( strGateway ) : 0;                                                     // A possible gateway

    if ( VmbErrorSuccess == err )
    {
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );                                                // Is Vimba connected to a GigE transport layer?
        if ( VmbErrorSuccess == err )
        {
            if( bIsGigE )
            {
                if ( 0 != nMAC )
                {
                    err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressMAC", nMAC );                                   // Send MAC address to TL
                    if ( VmbErrorSuccess == err )
                    {
                        err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressIP", nIP );                                 // Send new IP address to TL
                        if ( VmbErrorSuccess == err )
                        {
                            err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressSubnetMask", nSubnet );                 // Send new subnet mask to TL
                            if ( VmbErrorSuccess == err )
                            {
                                if( 0 != nGateway )
                                {
                                    err = VmbFeatureIntSet( gVimbaHandle, "GeVForceIPAddressGateway", nGateway );           // Send gateway address to TL
                                    if ( VmbErrorSuccess != err )
                                    {
                                        printf( "Could not prepare the gateway settings. Reason: %d\n\n", err );
                                    }
                                }

                                if ( VmbErrorSuccess == err )
                                {
                                    err = VmbFeatureCommandRun( gVimbaHandle, "GeVForceIPAddressSend" );                    // Finally execute the command to write all settings to cam
                                    if ( VmbErrorSuccess == err )
                                    {
                                        printf( "Command to change IP address to %s (%s) sent to camera.\n\n", strIP, strSubnet );
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
                else
                {
                    printf( "Malformed MAC address.\n\n" );
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
}
