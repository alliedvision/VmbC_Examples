/*=============================================================================
  Copyright (C) 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        AsynchronousGrab.cpp

  Description: The AsynchronousGrab example will grab images asynchronously
               using VimbaC.

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
#include <string.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <time.h>
#endif

#include <VimbaC/Include/VimbaC.h>

#include <AsynchronousGrab.h>

#define NUM_FRAMES 3 

VmbBool_t       g_bVimbaStarted = VmbBoolFalse;     // Remember if vimba is started
VmbBool_t       g_bStreaming = VmbBoolFalse;        // Remember if vimba is streaming
VmbBool_t       g_bAcquiring = VmbBoolFalse;        // Remember if vimba is acquiring
VmbHandle_t     g_CameraHandle = NULL;              // A handle to our camera
VmbFrame_t      g_Frames[NUM_FRAMES];               // The frames we capture into
FrameInfos      g_eFrameInfos = FrameInfos_Off;     // Remember if we should print out frame infos
double          g_dFrameTime = 0.0;                 // Timestamp of last frame
VmbBool_t       g_bFrameTimeValid = VmbBoolFalse;   // Remember if there was a last timestamp
VmbUint64_t     g_nFrameID = 0;                     // ID of last frame
VmbBool_t       g_bFrameIDValid = VmbBoolFalse;     // Remember if there was a last ID
#ifdef WIN32
double          g_dFrequency = 0.0;                 //Frequency of tick counter in Win32
#endif //WIN32

double GetTime()
{
#ifdef WIN32
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter(&nCounter);
    return ((double)nCounter.QuadPart) / g_dFrequency;
#else
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return ((double)now.tv_sec) + ((double)now.tv_nsec) / 1000000000.0;
#endif //WIN32
}

void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, VmbFrame_t* pFrame)
{
    VmbBool_t bShowFrameInfos = VmbBoolFalse;
    VmbErrorType res = VmbErrorSuccess;
    double dFPS = 0.0;
    VmbBool_t bFPSValid = VmbBoolFalse;
    double dFrameTime = 0.0;
    double dTimeDiff = 0.0;
    VmbUint64_t nFramesMissing = 0;

    if( FrameInfos_Off != g_eFrameInfos )
    {
        if( FrameInfos_Show == g_eFrameInfos )
        {
            bShowFrameInfos = VmbBoolTrue;
        }

        if( VmbFrameFlagsFrameID & pFrame->receiveFlags )
        {
            if( g_bFrameIDValid )
            {
                if( pFrame->frameID != ( g_nFrameID + 1 ) )
                {
                    nFramesMissing = pFrame->frameID - g_nFrameID - 1;
                    if( 1 == nFramesMissing )
                    {
                        printf("1 missing frame detected\n");
                    }
                    else
                    {
                        printf("%llu missing frames detected\n", nFramesMissing);
                    }
                }
            }

            g_nFrameID = pFrame->frameID;
            g_bFrameIDValid = VmbBoolTrue;

            dFrameTime = GetTime();
            if(     ( g_bFrameTimeValid )
                &&  ( 0 == nFramesMissing ) )
            {
                dTimeDiff = dFrameTime - g_dFrameTime;
                if(dTimeDiff > 0.0)
                {
                    dFPS = 1.0 / dTimeDiff;
                    bFPSValid = VmbBoolTrue;
                }
                else
                {
                    bShowFrameInfos = VmbBoolTrue;
                }
            }

            g_dFrameTime = dFrameTime;
            g_bFrameTimeValid = VmbBoolTrue;
        }
        else
        {
            bShowFrameInfos = VmbBoolTrue;
            g_bFrameIDValid = VmbBoolFalse;
            g_bFrameTimeValid = VmbBoolFalse;
        }

        if( VmbFrameStatusComplete != pFrame->receiveStatus )
        {
            bShowFrameInfos = VmbBoolTrue;
        }
    }

    if( bShowFrameInfos )
    {
        printf("Frame ID:");
        if( VmbFrameFlagsFrameID & pFrame->receiveFlags )
        {

            printf("%llu", pFrame->frameID);
        }
        else
        {
            printf("?");
        }

        printf(" Status:");
        switch(pFrame->receiveStatus)
        {
        case VmbFrameStatusComplete:
            printf("Complete");
            break;

        case VmbFrameStatusIncomplete:
            printf("Incomplete");
            break;

        case VmbFrameStatusTooSmall:  
            printf("Too small");
            break;

        case VmbFrameStatusInvalid:
            printf("Invalid");
            break;

        default:
            printf("?");
            break;
        }

        printf(" Size:");
        if( VmbFrameFlagsDimension & pFrame->receiveFlags )
        {
            printf("%ux%u", pFrame->width, pFrame->height);
        }
        else
        {
            printf("?x?");
        }

        printf(" Format:0x%08X", pFrame->pixelFormat);

        printf(" FPS:");
        if( bFPSValid )
        {
            printf("%.2f", dFPS);
        }
        else
        {
            printf("?");
        }

        printf("\n");
    }
    else
    {
        printf(".");
    }
    
    fflush(stdout);

    VmbCaptureFrameQueue( cameraHandle, pFrame, &FrameCallback );
}

VmbError_t StartContinuousImageAcquisition( const char* pCameraID, FrameInfos eFrameInfos )
{
    VmbError_t          err = VmbErrorSuccess;                  // The function result
    VmbCameraInfo_t     *pCameras = NULL;                       // A list of camera details
    VmbUint32_t         nCount = 0;                             // Number of found cameras
    VmbBool_t           bIsGigE = VmbBoolFalse;                 // GigE transport layer present
    const VmbUint32_t   nTimeout = 2000;                        // Timeout for Grab
    VmbAccessMode_t     cameraAccessMode = VmbAccessModeFull;   // We open the camera with full access
    VmbBool_t           bIsCommandDone = VmbBoolFalse;          // Has a command finished execution
    VmbInt64_t          nPayloadSize;                           // The size of one frame
    int                 i = 0;                                  // Counting variable
#ifdef WIN32
    LARGE_INTEGER nFrequency;
#endif //WIN32

    if( !g_bVimbaStarted )
    {
        g_bStreaming = VmbBoolFalse;
        g_bAcquiring = VmbBoolFalse;
        g_CameraHandle = NULL;
        memset(g_Frames, 0, sizeof(g_Frames));
        g_dFrameTime = 0.0;              
        g_bFrameTimeValid = VmbBoolFalse;
        g_nFrameID = 0;
        g_bFrameIDValid = VmbBoolFalse;
        g_eFrameInfos = eFrameInfos;

#ifdef WIN32
        QueryPerformanceFrequency(&nFrequency);
        g_dFrequency = (double)nFrequency.QuadPart;
#endif //WIN32

        err = VmbStartup();
        if ( VmbErrorSuccess == err )
        {
            g_bVimbaStarted = VmbBoolTrue;

            // Is Vimba connected to a GigE transport layer?
            err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );
            if ( VmbErrorSuccess == err )
            {
                if( VmbBoolTrue == bIsGigE )
                {
                    // Send discovery packets to GigE cameras
                    err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");
                    if ( VmbErrorSuccess == err )
                    {
                        // And wait for them to return
    #ifdef WIN32
                        Sleep( 200 );
    #else
                        usleep( 200 * 1000 );
    #endif
                    }
                    else
                    {
                        printf( "Could not ping GigE cameras over the network. Reason: %d\n\n", err );
                    }
                }
            }
            else
            {
                printf( "Could not query Vimba for the presence of a GigE transport layer. Reason: %d\n\n", err );
            }

            // If no camera ID was provided use the first camera found
            if ( NULL == pCameraID )
            {
                // Get the amount of known cameras
                err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );
                if (    VmbErrorSuccess == err
                     && 0 < nCount )
                {
                    pCameras = (VmbCameraInfo_t*)malloc( nCount * sizeof( *pCameras ));
                    if ( NULL != pCameras )
                    {
                        // Query all static details of all known cameras without having to open the cameras
                        err = VmbCamerasList( pCameras, nCount, &nCount, sizeof *pCameras );
                        if ( VmbErrorSuccess != err )
                        {
                            printf( "Could not list cameras. Error code: %d\n", err );
                        }
                        else
                        {
                            // Use the first camera
                            pCameraID = pCameras[0].cameraIdString;
                        }
                    }
                    else
                    {
                        printf( "Could not allocate camera list.\n" );
                    }
                    free( pCameras );
                    pCameras = NULL;
                }
                else
                {
                    printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
                }
            }

            if ( NULL != pCameraID )
            {
                // Open camera
                err = VmbCameraOpen( pCameraID, cameraAccessMode, &g_CameraHandle );
                if ( VmbErrorSuccess == err )
                {
                    printf("Opening camera with ID: %s\n", pCameraID);

                    // Set the GeV packet size to the highest possible value
                    // (In this example we do not test whether this cam actually is a GigE cam)
                    if ( VmbErrorSuccess == VmbFeatureCommandRun( g_CameraHandle, "GVSPAdjustPacketSize" ))
                    {
                        do
                        {
                            if ( VmbErrorSuccess != VmbFeatureCommandIsDone(    g_CameraHandle,
                                                                                "GVSPAdjustPacketSize",
                                                                                &bIsCommandDone ))
                            {
                                break;
                            }
                        } while ( VmbBoolFalse == bIsCommandDone );
                    }

                    if ( VmbErrorSuccess == err )
                    {
                        // Evaluate frame size
                        err = VmbFeatureIntGet( g_CameraHandle, "PayloadSize", &nPayloadSize );
                        if ( VmbErrorSuccess == err )
                        {
                            for(i = 0; i < NUM_FRAMES; i++)
                            {
                                g_Frames[i].buffer        = (unsigned char*)malloc( (VmbUint32_t)nPayloadSize );
                                if(NULL == g_Frames[i].buffer)
                                {
                                    err = VmbErrorResources;
                                    break;
                                }
                                g_Frames[i].bufferSize    = (VmbUint32_t)nPayloadSize;

                                // Announce Frame
                                err = VmbFrameAnnounce( g_CameraHandle, &g_Frames[i], (VmbUint32_t)sizeof( VmbFrame_t ) );
                                if ( VmbErrorSuccess != err )
                                {
                                    free(g_Frames[i].buffer);
                                    memset(&g_Frames[i], 0, sizeof(VmbFrame_t));
                                    break;
                                }
                            }

                            if ( VmbErrorSuccess == err )
                            {
                                // Start Capture Engine
                                err = VmbCaptureStart( g_CameraHandle );
                                if ( VmbErrorSuccess == err )
                                {
                                    g_bStreaming = VmbBoolTrue;
                                    for(i = 0; i < NUM_FRAMES; i++)
                                    {
                                        // Queue Frame
                                        err = VmbCaptureFrameQueue( g_CameraHandle, &g_Frames[i], &FrameCallback );
                                        if ( VmbErrorSuccess != err )
                                        {
                                            break;
                                        }
                                    }

                                    if ( VmbErrorSuccess == err )
                                    {
                                        // Start Acquisition
                                        err = VmbFeatureCommandRun( g_CameraHandle,"AcquisitionStart" );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            g_bAcquiring = VmbBoolTrue;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if( VmbErrorSuccess != err )
            {
                StopContinuousImageAcquisition();
            }
        }
    }
    else
    {
        err = VmbErrorOther;
    }

    return err;
}

void StopContinuousImageAcquisition()
{
    int i = 0;

    if( g_bVimbaStarted )
    {
        if( NULL != g_CameraHandle )
        {
            if( g_bAcquiring )
            {
                // Stop Acquisition
                VmbFeatureCommandRun( g_CameraHandle, "AcquisitionStop" );
                g_bAcquiring = VmbBoolFalse;
            }

            if( g_bStreaming )
            {
                // Stop Capture Engine
                VmbCaptureEnd( g_CameraHandle );
                g_bStreaming = VmbBoolFalse;
            }

            // Flush the capture queue
            VmbCaptureQueueFlush( g_CameraHandle );

            for(i = 0; i < NUM_FRAMES; i++)
            {
                if( NULL != g_Frames[i].buffer )
                {
                    VmbFrameRevoke( g_CameraHandle, &g_Frames[i] );
                    free(g_Frames[i].buffer);
                    memset(&g_Frames[i], 0, sizeof(VmbFrame_t));
                }
            }

            // Close camera
            VmbCameraClose ( g_CameraHandle );
            g_CameraHandle = NULL;
        }
        VmbShutdown();
        g_bVimbaStarted = VmbBoolFalse;
    }
}

char* ErrorCodeToMessage( VmbError_t eError )
{
    char *strError = NULL;

    switch(eError)
    {
    case VmbErrorSuccess:           strError = "Success."; break;
    case VmbErrorInternalFault:     strError = "Unexpected fault in VmbApi or driver."; break;
    case VmbErrorApiNotStarted:     strError = "API not started."; break; 
    case VmbErrorNotFound:          strError = "Not found."; break;
    case VmbErrorBadHandle:         strError = "Invalid handle "; break;
    case VmbErrorDeviceNotOpen:     strError = "Device not open."; break;
    case VmbErrorInvalidAccess:     strError = "Invalid access."; break;
    case VmbErrorBadParameter:      strError = "Bad parameter."; break;
    case VmbErrorStructSize:        strError = "Wrong DLL version."; break;
    case VmbErrorMoreData:          strError = "More data returned than memory provided."; break;
    case VmbErrorWrongType:         strError = "Wrong type."; break;
    case VmbErrorInvalidValue:      strError = "Invalid value."; break;
    case VmbErrorTimeout:           strError = "Timeout."; break;
    case VmbErrorOther:             strError = "TL error."; break;
    case VmbErrorResources:         strError = "Resource not available."; break;
    case VmbErrorInvalidCall:       strError = "Invalid call."; break;
    case VmbErrorNoTL:              strError = "TL not loaded."; break;
    case VmbErrorNotImplemented:    strError = "Not implemented."; break;
    case VmbErrorNotSupported:      strError = "Not supported."; break;
    default:                        strError = "Unknown"; break;
    }

    return strError;
}

/*
VmbError_t SynchronousGrab( const char* pCameraID, const char* pFileName )
{
    VmbError_t          err = VmbStartup();                     // Initialize the Vimba API
    VmbCameraInfo_t     *pCameras = NULL;                       // A list of camera details
    VmbUint32_t         nCount = 0;                             // Number of found cameras
    VmbBool_t           bIsGigE = VmbBoolFalse;                 // GigE transport layer present
    const VmbUint32_t   nTimeout = 2000;                        // Timeout for Grab
    VmbAccessMode_t     cameraAccessMode = VmbAccessModeFull;   // We open the camera with full access
    VmbHandle_t         cameraHandle = NULL;                    // A handle to our camera
    VmbBool_t           bIsCommandDone = VmbBoolFalse;          // Has a command finished execution
    VmbFrame_t          frame;                                  // The frame we capture
    const char*         pPixelFormat;                           // The pixel format we use for acquisition
    VmbInt64_t          nPayloadSize;                           // The size of one frame
    AVTBitmap           bitmap;                                 // The bitmap we create


    if ( VmbErrorSuccess == err )
    {
        // Is Vimba connected to a GigE transport layer?
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );
        if ( VmbErrorSuccess == err )
        {
            if( VmbBoolTrue == bIsGigE )
            {
                // Send discovery packets to GigE cameras
                err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");
                if ( VmbErrorSuccess == err )
                {
                    // And wait for them to return
#ifdef WIN32
                    Sleep( 200 );
#else
                    usleep( 200 * 1000 );
#endif
                }
                else
                {
                    printf( "Could not ping GigE cameras over the network. Reason: %d\n\n", err );
                }
            }
        }
        else
        {
            printf( "Could not query Vimba for the presence of a GigE transport layer. Reason: %d\n\n", err );
        }

        // If no camera ID was provided use the first camera found
        if ( NULL == pCameraID )
        {
            // Get the amount of known cameras
            err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pCameras = (VmbCameraInfo_t*)malloc( nCount * sizeof( *pCameras ));
                if ( NULL != pCameras )
                {
                    // Query all static details of all known cameras without having to open the cameras
                    err = VmbCamerasList( pCameras, nCount, &nCount, sizeof *pCameras );
                    if ( VmbErrorSuccess != err )
                    {
                        printf( "Could not list cameras. Error code: %d\n", err );
                    }
                    else
                    {
                        // Use the first camera
                        pCameraID = pCameras[0].cameraIdString;
                    }
                }
                else
                {
                    printf( "Could not allocate camera list.\n" );
                }
                free( pCameras );
                pCameras = NULL;
            }
            else
            {
                printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
            }
        }

        if ( NULL != pCameraID )
        {
            // Open camera
            err = VmbCameraOpen( pCameraID, cameraAccessMode, &cameraHandle );
            if ( VmbErrorSuccess == err )
            {
                printf( "Camera ID: %s\n\n", pCameraID );

                // Set the GeV packet size to the highest possible value
                // (In this example we do not test whether this cam actually is a GigE cam)
                if ( VmbErrorSuccess == VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" ))
                {
                    do
                    {
                        if ( VmbErrorSuccess != VmbFeatureCommandIsDone(    cameraHandle,
                                                                            "GVSPAdjustPacketSize",
                                                                            &bIsCommandDone ))
                        {
                            break;
                        }
                    } while ( VmbBoolFalse == bIsCommandDone );
                }

                if ( VmbErrorSuccess == err )
                {
                    // Set pixel format. For the sake of simplicity we only support Mono and RGB in this example.
                    err = VmbFeatureEnumSet( cameraHandle, "PixelFormat", "RGB8Packed" );
                    if ( VmbErrorSuccess != err )
                    {
                        // Fall back to Mono
                        err = VmbFeatureEnumSet( cameraHandle, "PixelFormat", "Mono8" );
                    }
                    // Read back pixel format
                    VmbFeatureEnumGet( cameraHandle, "PixelFormat", &pPixelFormat );

                    if ( VmbErrorSuccess == err )
                    {
                        // Evaluate frame size
                        err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );
                        if ( VmbErrorSuccess == err )
                        {
                            frame.buffer        = (unsigned char*)malloc( (VmbUint32_t)nPayloadSize );
                            frame.bufferSize    = (VmbUint32_t)nPayloadSize;

                            // Announce Frame
                            err = VmbFrameAnnounce( cameraHandle, &frame, (VmbUint32_t)sizeof( VmbFrame_t ) );
                            if ( VmbErrorSuccess == err )
                            {
                                // Start Capture Engine
                                err = VmbCaptureStart( cameraHandle );
                                if ( VmbErrorSuccess == err )
                                {
                                    // Queue Frame
                                    err = VmbCaptureFrameQueue( cameraHandle, &frame, NULL );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        // Start Acquisition
                                        err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            // Capture one frame synchronously
                                            err = VmbCaptureFrameWait( cameraHandle, &frame, nTimeout );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                // Convert the captured frame to a bitmap and save to disk
                                                if ( VmbFrameStatusComplete == frame.receiveStatus )
                                                {
                                                    bitmap.bufferSize = frame.imageSize;
                                                    bitmap.width = frame.width;
                                                    bitmap.height = frame.height;
                                                    // We only support Mono and RGB in this example
                                                    if ( 0 == strcmp( "RGB8Packed", pPixelFormat ))
                                                    {
                                                        bitmap.colorCode = ColorCodeRGB24;
                                                    }
                                                    else
                                                    {
                                                        bitmap.colorCode = ColorCodeMono8;
                                                    }

                                                    // Create the bitmap
                                                    if ( 0 == AVTCreateBitmap( &bitmap, frame.buffer ))
                                                    {
                                                        printf( "Could not create bitmap.\n" );
                                                    }
                                                    else
                                                    {
                                                        // Save the bitmap
                                                        if ( 0 == AVTWriteBitmapToFile( &bitmap, pFileName ))
                                                        {
                                                            printf( "Could not write bitmap to file.\n" );
                                                        }
                                                        else
                                                        {
                                                            printf( "Bitmap successfully written to file \"%s\"\n", pFileName );
                                                            // Release the bitmap's buffer
                                                            if ( 0 == AVTReleaseBitmap( &bitmap ))
                                                            {
                                                                printf( "Could not release the bitmap.\n" );
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    printf( "Frame not successfully received. Error code: %d\n", frame.receiveStatus );
                                                }
                                            }
                                            else
                                            {
                                                printf( "Could not capture frame. Error code: %d", err );
                                            }

                                            // Stop Acquisition
                                            err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStop" );
                                            if ( VmbErrorSuccess != err )
                                            {
                                                printf( "Could not stop acquisition. Error code: %d\n", err );
                                            }
                                        }
                                        else
                                        {
                                            printf( "Could not start acquisition. Error code: %d", err );
                                        }
                                    }
                                    else
                                    {
                                        printf( "Could not queue frame. Error code: %d\n", err );
                                    }

                                    // Stop Capture Engine
                                    err = VmbCaptureEnd( cameraHandle );
                                    if ( VmbErrorSuccess != err )
                                    {
                                        printf( "Could not end capture. Error code: %d\n", err );
                                    }
                                }
                                else
                                {
                                    printf( "Could not start capture. Error code: %d\n", err );
                                }

                                // Revoke frame
                                err = VmbFrameRevoke( cameraHandle, &frame );
                                if ( VmbErrorSuccess != err )
                                {
                                    printf( "Could not revoke frame. Error code: %d\n", err );
                                }
                            }
                            else
                            {
                                printf( "Could not announce frame. Error code: %d\n", err );
                            }

                            free( frame.buffer );
                            frame.buffer = NULL;
                        }
                    }
                    else
                    {
                        printf( "Could not set pixel format to either RGB or Mono. Error code: %d\n", err );
                    }
                }
                else
                {
                    printf( "Could not adjust packet size. Error code: %d\n", err );
                }

                err = VmbCameraClose ( cameraHandle );
                if ( VmbErrorSuccess != err )
                {
                    printf( "Could not close camera. Error code: %d\n", err );
                }
            }
            else
            {
                printf( "Could not open camera. Error code: %d\n", err );
            }
        }
        VmbShutdown();
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }

    return err;
}
*/