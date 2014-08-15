/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:         ListAncillaryDataFeatures.cpp

  Description:  The ListAncillaryDataFeatures example will list all available
                features of the ancillary data that are found by VimbaC.

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

#include <ListAncillaryDataFeatures.h>

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/PrintVimbaVersion.h>
#include <../../Common/DiscoverGigECameras.h>

void ListAncillaryDataFeatures( const char *pStrID )
{
    VmbError_t          err             = VmbErrorSuccess;
    VmbHandle_t         hCamera         = NULL;                                                     // A handle to our camera
    VmbHandle_t         hAncillaryData  = NULL;                                                     // A handle to ancillary data
    VmbCameraInfo_t*    pCameras        = NULL;                                                     // A list of camera infos
    VmbUint32_t         nCount          = 0;                                                        // The number of found cameras
    VmbFeatureInfo_t*   pFeatures       = NULL;                                                     // A list of ancillary data features
    VmbFrame_t          Frame;                                                                      // A single frame
    VmbInt64_t          nPLS            = 0;                                                        // The payload size of a frame
    // The volatile value of a feature
    VmbInt64_t          nValue          = 0;                                                        // An int value
    double              fValue          = 0.0;                                                      // A float value
    char                *pStrValue      = NULL;                                                     // A string value
    VmbBool_t           bValue          = VmbBoolFalse;                                             // A bool value
    VmbUint32_t         i               = 0;

    err = VmbStartup();                                                                             // Initialize the Vimba API
    PrintVimbaVersion();                                                                            // Print Vimba Version
    if ( VmbErrorSuccess == err )
    {
        DiscoverGigECameras();                                                                      // Seek for GigE cameras on the network

        if ( NULL == pStrID )                                                                       // If no ID was provided use the first camera
        {
            err = VmbCamerasList(   NULL,                                                           // Get the amount of known cameras
                                    0,
                                    &nCount,
                                    sizeof *pCameras );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pCameras = (VmbCameraInfo_t*)malloc( nCount * sizeof *pCameras );
                if ( NULL != pCameras )
                {
                    err = VmbCamerasList(   pCameras,                                               // Get all known cameras
                                            nCount,
                                            &nCount,
                                            sizeof *pCameras );
                    if (    VmbErrorSuccess == err
                         || VmbErrorMoreData == err )                                               // If a new camera was connected since we queried
                    {                                                                               // for the amount of cameras, we can ignore that one
                        if( 0 < nCount )
                        {
                            err = VmbCameraOpen(    pCameras[0].cameraIdString,                     // Finally open the first one
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
            err = VmbCameraOpen(    pStrID,                                                         // Open the camera with the given ID
                                    VmbAccessModeFull,
                                    &hCamera );
        }

        if ( VmbErrorSuccess == err )
        {
            err = VmbFeatureBoolSet( hCamera, "ChunkModeActive", VmbBoolTrue );                     // Enable ancillary data
            if ( VmbErrorSuccess == err )
            {
                // In order to fill the ancillary data we need to fill frame
                printf( "Capture a single frame\n" );

                err = VmbFeatureIntGet( hCamera, "PayloadSize", &nPLS );
                if ( VmbErrorSuccess == err )
                {
                    Frame.bufferSize = (VmbUint32_t)nPLS;
                    Frame.buffer = malloc( nPLS * sizeof( unsigned char ) );
                    if ( NULL != Frame.buffer )
                    {
                        err = VmbCaptureStart( hCamera );
                        if ( VmbErrorSuccess == err )
                        {
                            err = VmbCaptureFrameQueue( hCamera, &Frame, NULL );
                            if ( VmbErrorSuccess == err )
                            {
                                err = VmbFeatureCommandRun( hCamera, "AcquisitionStart" );
                                if ( VmbErrorSuccess == err )
                                {
                                    err = VmbCaptureFrameWait( hCamera, &Frame, 1000 );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        if ( VmbFrameStatusComplete == Frame.receiveStatus )
                                        {
                                            err = VmbAncillaryDataOpen( &Frame, &hAncillaryData );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                // Query all static details as well as the value of all fetched ancillary data features and print them out.
                                                err = VmbFeaturesList(  hAncillaryData,             // Get the amount of features
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
                                                        err = VmbFeaturesList(  hAncillaryData,     // Get the features
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
                                                                    err = VmbFeatureBoolGet( hAncillaryData, pFeatures[i].name, &bValue );
                                                                    if ( VmbErrorSuccess == err )
                                                                    {
                                                                        printf( "%d\n", bValue );
                                                                    }
                                                                    break;
                                                                case VmbFeatureDataEnum:
                                                                    err = VmbFeatureEnumGet( hAncillaryData, pFeatures[i].name, (const char**)&pStrValue );
                                                                    if ( VmbErrorSuccess == err )
                                                                    {
                                                                        printf( "%s\n", pStrValue );
                                                                    }
                                                                    break;
                                                                case VmbFeatureDataFloat:
                                                                    err = VmbFeatureFloatGet( hAncillaryData, pFeatures[i].name, &fValue );
                                                                    {
                                                                        printf( "%f\n", fValue );
                                                                    }
                                                                    break;
                                                                case VmbFeatureDataInt:
                                                                    err = VmbFeatureIntGet( hAncillaryData, pFeatures[i].name, &nValue );
                                                                    {
                                                                        printf( "%lld\n", nValue );
                                                                    }
                                                                    break;
                                                                case VmbFeatureDataString:
                                                                    {
                                                                        VmbUint32_t nSize = 0;
                                                                        err = VmbFeatureStringGet( hAncillaryData, pFeatures[i].name, NULL, 0, &nSize );
                                                                        if (    VmbErrorSuccess == err
                                                                             && 0 < nSize )
                                                                        {
                                                                            pStrValue = (char*)malloc( sizeof *pStrValue * nSize );
                                                                            if ( NULL != pStrValue )
                                                                            {
                                                                                err = VmbFeatureStringGet( hAncillaryData, pFeatures[i].name, pStrValue, nSize, &nSize );
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
                                                    printf( "Could not get features or the ancillary data does not provide any. Error code: %d\n", err );
                                                }
                                            }
                                            else
                                            {
                                                printf( "Could not open ancillary data. Error code: %d\n", err );
                                            }
                                        }
                                        else
                                        {
                                            printf( "Captured frame is not complete. Cannot access ancillary data. Frame status: %d\n", err );
                                        }
                                    }
                                    else
                                    {
                                        printf( "Could not capture frame. Error code: %d\n", err );
                                    }
                                    free( Frame.buffer );
                                    Frame.buffer = NULL;
                                }
                                else
                                {
                                    printf( "Could not start acquisition on the camera. Error code: %d\n", err );
                                }
                            }
                            else
                            {
                                printf( "Could not queue frame. Error code: %d\n", err );
                            }
                        }
                        else
                        {
                            printf( "Could not start capture engine on the host. Error code: %d\n", err );
                        }
                    }
                    else
                    {
                        printf( "Could not allocate image buffer.\n" );
                    }
                }
                else
                {
                    printf( "Could not get payload size. Cannot acquire frame. Error code: %d\n", err );
                }
            }
            else if ( VmbErrorNotFound == err )
            {
                printf( "The camera does not provide ancillary data. Error code: %d\n", err );
            }
            else
            {
                printf( "Could not query for the presence of ancillary data. Error code: %d\n", err );
            }

            VmbCameraClose( hCamera );                                                              // Close the camera
        }
        else
        {
            printf( "Could not open camera. Error code: %d\n", err );
        }
        
        VmbShutdown();                                                                              // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
}
