/*=============================================================================
  Copyright (C) 2014 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:         ListAncillaryDataFeatures.c

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
#ifdef WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

#include <ListAncillaryDataFeatures.h>

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/PrintVimbaVersion.h>
#include <../../Common/DiscoverGigECameras.h>

// Purpose: Fetches features from the ancillary data. Ancillary data is part of a frame,
//          therefore we need to capture a single frame beforehand.
//          If no camera ID string was passed we use the first camera found.
//
// Parameter:
// [in ]    const char* pStrID          The ID of the camera to use. May be NULL.
void ListAncillaryDataFeatures( const char* pStrID )
{
    VmbError_t          err                 = VmbErrorSuccess;
    VmbHandle_t         cameraHandle        = NULL;                                                             // A handle to our camera
    VmbHandle_t         ancillaryDataHandle = NULL;                                                             // A handle to ancillary data
    VmbCameraInfo_t*    pCameras            = NULL;                                                             // A list of camera infos
    VmbUint32_t         count               = 0;                                                                // The number of found cameras
    VmbFeatureInfo_t*   pFeatures           = NULL;                                                             // A list of ancillary data features
    VmbFrame_t          frame;                                                                                  // A single frame
    VmbInt64_t          pls                 = 0;                                                                // The payload size of a frame
    VmbCameraInfo_t     info;                                                                                   // The name and other information of our camera
    VmbBool_t           bIsCommandDone      = VmbBoolFalse;                                                     // Has a command finished execution
    // The volatile value of a feature
    double              doubleValue         = 0.0;                                                              // A float value
    char*               pStringValue        = NULL;                                                             // A string value
    VmbBool_t           boolValue           = VmbBoolFalse;                                                     // A bool value
    VmbInt64_t          intValue            = 0;                                                                // An int value
    VmbUint32_t         i                   = 0;

    err = VmbStartup();                                                                                         // Initialize the Vimba API
    PrintVimbaVersion();                                                                                        // Print Vimba Version
    if (VmbErrorSuccess == err)
    {
        DiscoverGigECameras();                                                                                  // Seek for GigE cameras on the network

        if( NULL == pStrID )                                                                                    // If no ID was provided use the first camera
        {
            err = VmbCamerasList (  NULL,                                                                       // Get the amount of known cameras
                                    0,
                                    &count,
                                    sizeof *pCameras );
            if(     VmbErrorSuccess == err
                &&  0 < count )
            {
                pCameras = (VmbCameraInfo_t*)malloc( count * sizeof *pCameras );
                if( NULL != pCameras )
                {
                    err = VmbCamerasList(   pCameras,                                                           // Get all known cameras
                                            count,
                                            &count,
                                            sizeof *pCameras );
                    if(     VmbErrorSuccess == err
                        ||  VmbErrorMoreData == err )                                                           // If a new camera was connected since we queried
                    {                                                                                           // for the amount of cameras, we can ignore that one
                        if( 0 < count )
                        {
                            info = pCameras[0];                                                                 // Copy the camera info of the camera we will use
                            err = VmbCameraOpen(    pCameras[0].cameraIdString,                                 // Finally open the first one
                                                    VmbAccessModeFull,
                                                    &cameraHandle );
                        }
                        else
                        {
                            printf( "Camera lost. Error code: %d\n", err );
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
            err = VmbCameraOpen(    pStrID,                                                                     // Open the camera with the given ID
                                    VmbAccessModeFull,
                                    &cameraHandle );
        }

        if ( NULL != cameraHandle )
        {
            if( VmbErrorSuccess == err )
            {
                if ( NULL != pStrID )
                {
                    err = VmbCameraInfoQuery( pStrID, &info, sizeof( VmbCameraInfo_t ) );                       // If we haven't queried the camera info yet, we do it now to display the camera name
                }

                if ( VmbErrorSuccess == err )
                {
                    printf( "Using camera %s\n", info.cameraName );

                    err = VmbFeatureBoolSet( cameraHandle, "ChunkModeActive", VmbBoolTrue );                    // Enable ancillary data
                    if( VmbErrorSuccess == err )
                    {
                        // Set the GeV packet size to the highest possible value
                        // (In this example we do not test whether this cam actually is a GigE cam)
                        if ( VmbErrorSuccess == VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" ))
                        {
                            printf( "Adjusting packet size " );                                                 // Set the highest possible packet size
                            do
                            {
                                if ( VmbErrorSuccess != VmbFeatureCommandIsDone(    cameraHandle,
                                                                                    "GVSPAdjustPacketSize",
                                                                                    &bIsCommandDone ))
                                {
                                    break;
                                }
#ifdef WIN32
                                Sleep(200);
#else
                                usleep(200000);
#endif
                                printf( "." );
                            } while ( VmbBoolFalse == bIsCommandDone );
                        }

                        printf( "\nCapture a single frame\n" );                                                 // In order to fill the ancillary data we need to fill a frame

                        err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &pls );
                        if( VmbErrorSuccess == err )
                        {
                            frame.bufferSize = (VmbUint32_t)pls;
                            frame.buffer = malloc( (VmbUint32_t)pls * sizeof(unsigned char) );
                            if ( NULL != frame.buffer )
                            {
                                err = VmbCaptureStart ( cameraHandle );
                                if( VmbErrorSuccess == err )
                                {
                                    err = VmbCaptureFrameQueue( cameraHandle, &frame, NULL );
                                    if( VmbErrorSuccess == err )
                                    {
                                        err = VmbFeatureCommandRun( cameraHandle, "AcquisitionStart" );
                                        if( VmbErrorSuccess == err )
                                        {
                                            err = VmbCaptureFrameWait( cameraHandle, &frame, 1000 );
                                            if( VmbErrorSuccess == err )
                                            {
                                                if( VmbFrameStatusComplete == frame.receiveStatus )
                                                {
                                                    err = VmbAncillaryDataOpen( &frame, &ancillaryDataHandle );
                                                    if( VmbErrorSuccess == err )
                                                    {
                                                        // Query all static details as well as the value of all fetched ancillary data features and print them out.
                                                        err = VmbFeaturesList(  ancillaryDataHandle,            // Get the amount of features
                                                                                NULL,
                                                                                0,
                                                                                &count,
                                                                                sizeof *pFeatures );
                                                        if (    VmbErrorSuccess == err
                                                             && 0 < count )
                                                        {
                                                            pFeatures = (VmbFeatureInfo_t *)malloc( count * sizeof *pFeatures );
                                                            if( NULL != pFeatures )
                                                            {
                                                                err = VmbFeaturesList(  ancillaryDataHandle,    // Get the features
                                                                                        pFeatures,
                                                                                        count,
                                                                                        &count,
                                                                                        sizeof *pFeatures   );
                                                                if( VmbErrorSuccess == err )
                                                                {
                                                                    for( i=0; i<count; ++i )
                                                                    {
                                                                        printf( "/// Feature Name: %s\n", ( NULL == pFeatures[i].name ? "" : pFeatures[i].name ));
                                                                        printf( "/// Display Name: %s\n", ( NULL == pFeatures[i].displayName ? "" : pFeatures[i].displayName ));
                                                                        printf( "/// Tooltip: %s\n", ( NULL == pFeatures[i].tooltip ? "" : pFeatures[i].tooltip ));
                                                                        printf( "/// Description: %s\n", ( NULL == pFeatures[i].description ? "" : pFeatures[i].description ));
                                                                        printf( "/// SNFC Namespace: %s\n", ( NULL == pFeatures[i].sfncNamespace ? "" : pFeatures[i].sfncNamespace ));
                                                                        printf( "/// Value: " );

                                                                        switch( pFeatures[i].featureDataType )
                                                                        {
                                                                        case VmbFeatureDataBool:
                                                                            err = VmbFeatureBoolGet( ancillaryDataHandle, pFeatures[i].name, &boolValue );
                                                                            if( VmbErrorSuccess == err )
                                                                            {
                                                                                printf( "%d\n", boolValue );
                                                                            }
                                                                            break;
                                                                        case VmbFeatureDataEnum:
                                                                            err = VmbFeatureEnumGet( ancillaryDataHandle, pFeatures[i].name, (const char**)&pStringValue );
                                                                            if ( VmbErrorSuccess == err )
                                                                            {
                                                                                printf( "%s\n", pStringValue );
                                                                            }
                                                                            break;
                                                                        case VmbFeatureDataFloat:
                                                                            err = VmbFeatureFloatGet( ancillaryDataHandle, pFeatures[i].name, &doubleValue );
                                                                            {
                                                                                printf( "%f\n", doubleValue );
                                                                            }
                                                                            break;
                                                                        case VmbFeatureDataInt:
                                                                            err = VmbFeatureIntGet( ancillaryDataHandle, pFeatures[i].name, &intValue );
                                                                            {
                                                                                printf( "%lld\n", intValue );
                                                                            }
                                                                            break;
                                                                        case VmbFeatureDataString:
                                                                            {
                                                                                VmbUint32_t size = 0;
                                                                                err = VmbFeatureStringGet( ancillaryDataHandle, pFeatures[i].name, NULL, 0, &size );
                                                                                if(     VmbErrorSuccess == err
                                                                                    &&  0 < size )
                                                                                {
                                                                                    pStringValue = (char*)malloc( sizeof *pStringValue * (VmbUint32_t)intValue );
                                                                                    if( NULL != pStringValue )
                                                                                    {
                                                                                        err = VmbFeatureStringGet( ancillaryDataHandle, pFeatures[i].name, pStringValue, size, &size );
                                                                                        if( VmbErrorSuccess == err )
                                                                                        {
                                                                                            printf( "%s\n", pStringValue );
                                                                                        }
                                                                                        free( pStringValue );
                                                                                        pStringValue = NULL;
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

                                                                        if( VmbErrorSuccess != err )
                                                                        {
                                                                            printf( "Could not get feature value. Error code: %d\n", err );
                                                                        }

                                                                        printf ( "\n" );
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    printf( "Could not get features. Error code: %d\n", err );
                                                                }

                                                                free( pFeatures );
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
                                                    printf( "Captured frame is not complete. Cannot access ancillary data. Frame status: %d.\n(GigE: Are jumbo packets enabled?)\n", frame.receiveStatus );
                                                }
                                            }
                                            else
                                            {
                                                printf( "Could not capture frame. Error code: %d\n", err );
                                            }
                                            free( frame.buffer );
                                            frame.buffer = NULL;
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
                    else if( VmbErrorNotFound == err )
                    {
                        printf( "The camera does not provide ancillary data. Error code: %d\n", err );
                    }
                    else
                    {
                        printf( "Could not query for the presence of ancillary data. Error code: %d\n", err );
                    }
                }
                else
                {
                    printf( "Could not query the camera's name. Error code: %d\n", err );
                }

                VmbCameraClose( cameraHandle );                                                                 // Close the camera
            }
            else
            {
                printf( "Could not open camera. Error code: %d\n", err );
            }
        }
        else
        {
            printf( "No cameras present.\n" );
        }
        
        VmbShutdown();                                                                                          // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
}
