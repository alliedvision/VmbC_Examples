/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListFeatures.cpp

  Description: The ListFeatures example will list all available features of a
               camera that are found by VimbaC.

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

#include <ListFeatures.h>

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/PrintVimbaVersion.h>
#include <../../Common/DiscoverGigECameras.h>

//
// Prints out all features and their values and details of a given camera.
// If no camera ID is provided, the first camera will be used.
// Starts and stops the API
// Opens and closes the camera
//
// Parameters:
//  [in]    pStrID          The ID of the camera to work
//
void ListFeatures( const char *pStrID )
{
    VmbError_t          err         = VmbErrorSuccess;
    VmbHandle_t         hCamera     = NULL;                                             // A handle to our camera
    VmbCameraInfo_t*    pCameras    = NULL;                                             // A list of camera infos
    VmbUint32_t         nCount      = 0;                                                // The number of found cameras	
    VmbFeatureInfo_t    *pFeatures  = NULL;                                             // A list of static details of camera features
    // The changeable value of a feature
    VmbInt64_t          nValue      = 0;                                                // An int value
    double              fValue      = 0.0;                                              // A float value
    char                *pStrValue  = NULL;                                             // A string value
    VmbBool_t           bValue      = VmbBoolFalse;                                     // A bool value
    VmbUint32_t         i           = 0;

    err = VmbStartup();                                                                 // Initialize the Vimba API
    PrintVimbaVersion();                                                                // Print Vimba Version
    if ( VmbErrorSuccess == err )
    {
        DiscoverGigECameras();

        if ( NULL == pStrID )                                                           // If no ID was provided use the first camera
        {
            err = VmbCamerasList(   NULL,                                               // Get the amount of known cameras
                                    0,
                                    &nCount,
                                    sizeof *pCameras );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pCameras = (VmbCameraInfo_t*)malloc( nCount * sizeof *pCameras );
                if ( NULL != pCameras )
                {
                    err = VmbCamerasList(   pCameras,                                   // Get all known cameras
                                            nCount,
                                            &nCount,
                                            sizeof *pCameras );
                    if (    VmbErrorSuccess == err
                         || VmbErrorMoreData == err )                                   // If a new camera was connected since we queried
                    {                                                                   // for the amount of cameras, we can ignore that one
                        if( 0 < nCount )
                        {
                            err = VmbCameraOpen(    pCameras[0].cameraIdString,         // Finally open the first one
                                                    VmbAccessModeFull,
                                                    &hCamera );
                        }
                        else
                        {
                            printf( "Camera lost.\n" );
                        }
                    }
                    else
                    {
                        printf( "Could not list cameras. Error code: %d\n", err );
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
        }
        else
        {
            err = VmbCameraOpen(    pStrID,                                             // Open the camera with the given ID
                                    VmbAccessModeFull,
                                    &hCamera );
        }

        // Query all static details as well as the value of all fetched features and print them out.
        if ( VmbErrorSuccess == err )
        {
            err = VmbFeaturesList(  hCamera,                                            // Get the amount of features
                                    NULL,
                                    0,
                                    &nCount,
                                    sizeof *pFeatures );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pFeatures = (VmbFeatureInfo_t *)malloc( nCount * sizeof *pFeatures );
                if ( NULL != pFeatures )
                {
                    err = VmbFeaturesList(  hCamera,                                    // Get the features
                                            pFeatures,
                                            nCount,
                                            &nCount,
                                            sizeof *pFeatures );
                    if ( VmbErrorSuccess == err )
                    {
                        for ( i=0; i<nCount; ++i )
                        {
                            printf( "/// Feature Name: %s\n", ( NULL == pFeatures[i].name ? "" : pFeatures[i].name ));
                            printf( "/// Display Name: %s\n", ( NULL == pFeatures[i].displayName ? "" : pFeatures[i].displayName ));
                            printf( "/// Tooltip: %s\n", ( NULL == pFeatures[i].tooltip ? "" : pFeatures[i].tooltip ));
                            printf( "/// Description: %s\n", ( NULL == pFeatures[i].description ? "" : pFeatures[i].description ));
                            printf( "/// SNFC Namespace: %s\n", ( NULL == pFeatures[i].sfncNamespace ? "" : pFeatures[i].sfncNamespace ));
                            printf( "/// Value: " );

                            switch ( pFeatures[i].featureDataType )
                            {
                                case VmbFeatureDataBool:
                                    err = VmbFeatureBoolGet( hCamera, pFeatures[i].name, &bValue );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        printf( "%d\n", bValue );
                                    }
                                    break;
                                case VmbFeatureDataEnum:
                                    err = VmbFeatureEnumGet( hCamera, pFeatures[i].name, (const char**)&pStrValue );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        printf( "%s\n", pStrValue );
                                    }
                                    break;
                                case VmbFeatureDataFloat:
                                    err = VmbFeatureFloatGet( hCamera, pFeatures[i].name, &fValue );
                                    {
                                        printf( "%f\n", fValue );
                                    }
                                    break;
                                case VmbFeatureDataInt:
                                    err = VmbFeatureIntGet( hCamera, pFeatures[i].name, &nValue );
                                    {
                                        printf( "%lld\n", nValue );
                                    }
                                    break;
                                case VmbFeatureDataString:
                                    {
                                    VmbUint32_t nSize = 0;
                                    err = VmbFeatureStringGet( hCamera, pFeatures[i].name, NULL, 0, &nSize );
                                    if (    VmbErrorSuccess == err
                                         && 0 < nSize )
                                    {
                                        pStrValue = (char*)malloc( sizeof *pStrValue * nSize );
                                        if ( NULL != pStrValue )
                                        {
                                            err = VmbFeatureStringGet( hCamera, pFeatures[i].name, pStrValue, nSize, &nSize );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                printf( "%s\n", pStrValue );
                                            }
                                            free( pStrValue );
                                            pStrValue = NULL;
                                        }
                                        else
                                        {
                                            printf( "Could not allocate string.\n" );
                                        }
                                    }
                                    }
                                    break;
                                case VmbFeatureDataCommand:
                                default:
                                    printf( "[None]\n" );
                                    break;
                            }
                            
                            if ( VmbErrorSuccess != err )
                            {
                                printf( "Could not get feature value. Error code: %d\n", err );
                            }
                            
                            printf( "\n" );
                        }
                    }
                    else
                    {
                        printf( "Could not get features. Error code: %d\n", err );
                    }

                    free(pFeatures);
                    pFeatures = NULL;
                }
                else
                {
                    printf( "Could not allocate feature list.\n" );
                }
            }
            else
            {
                printf( "Could not get features or the camera does not provide any. Error code: %d\n", err );
            }

            VmbCameraClose( hCamera );                                                  // Close the camera
        }
        else
        {
            printf( "Could not open camera. Error code: %d\n", err );
        }
        
        VmbShutdown();                                                                  // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
}
