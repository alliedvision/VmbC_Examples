/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListCameras.c

  Description: The ListCameras example will list all available cameras that
               are found by VimbaC.

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

#include <ListCameras.h>

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/PrintVimbaVersion.h>
#include <../../Common/DiscoverGigECameras.h>

//
// Starts Vimba
// Gets all connected cameras
// And prints out information about the camera name, model name, serial number, ID and the corresponding interface ID
//
void ListCameras()
{
    VmbError_t          err             = VmbErrorSuccess;
    VmbCameraInfo_t *   pCameras        = NULL;
    VmbUint32_t         i               = 0;
    VmbUint32_t         nCount          = 0;
    VmbUint32_t         nFoundCount     = 0;
    
    err = VmbStartup();                                                                     // Initialize the Vimba API
    PrintVimbaVersion();                                                                    // Print Vimba Version

    if ( VmbErrorSuccess == err )
    {
        DiscoverGigECameras();

        err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );                         // Get the amount of known cameras
        if (    VmbErrorSuccess == err
             && nCount          != 0 )
        {
            printf( "Cameras found: %d\n\n", nCount );
        
            pCameras = (VmbCameraInfo_t*)malloc( sizeof *pCameras * nCount );
            if ( NULL != pCameras )
            {
                err = VmbCamerasList( pCameras, nCount, &nFoundCount, sizeof *pCameras );   // Query all static details of all known cameras
                                                                                            // Without having to open the cameras
                if(     VmbErrorSuccess == err
                    ||  VmbErrorMoreData == err )
                {
                    if( nFoundCount < nCount)                                               // If a new camera was connected since we queried
                    {                                                                       // the amount of cameras, we can ignore that one
                        nCount = nFoundCount;
                    }
                    for ( i=0; i<nCount; ++i )                                              // And print them out
                    {
                        printf( "/// Camera Name: %s\n/// Model Name: %s\n/// Camera ID: %s\n/// Serial Number: %s\n/// @ Interface ID: %s\n\n\n",
                                pCameras[i].cameraName,
                                pCameras[i].modelName,
                                pCameras[i].cameraIdString,
                                pCameras[i].serialString,
                                pCameras[i].interfaceIdString );
                    }
                }
                else
                {
                    printf( "Could not retrieve camera list. Error code: %d\n", err );
                }
                free( pCameras );
                pCameras = NULL;
            }
            else
            {
                printf( "Could not allocate camera list.\n" );
            }
        }
        else
        {
            printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
        }
        
        VmbShutdown();                                                                      // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
}
