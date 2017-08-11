/*=============================================================================
  Copyright (C) 2012 - 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        SynchronousGrab.cpp

  Description: The SynchronousGrab example will grab a single image
               synchronously and save it to a file using VimbaC.

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
#endif

#include <VimbaC/Include/VimbaC.h>
#include "../../Common/PrintVimbaVersion.h"
#include "../../Common/DiscoverGigECameras.h"
#include <SynchronousGrab.h>
#include <Bitmap.h>

//
// Starts Vimba API
// Opens the given camera, if no camera ID was provided, the first camera found will be used
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Acquires one image synchronously
// Writes the image as bitmap to file
// Closes the camera in case of failure
//
// Parameters:
//  [in]    pCameraID           The ID of the camera to work on. Can be NULL.
//  [in]    pFileName           The path of the bitmap where the image is saved to
//
// Returns:
//  An API status code
//
VmbError_t SynchronousGrab( const char* pCameraID, const char* pFileName )
{
    VmbError_t          err                 = VmbStartup();     // Initialize the Vimba API
    VmbCameraInfo_t     *pCameras           = NULL;             // A list of camera details
    VmbUint32_t         nCount              = 0;                // Number of found cameras
    VmbUint32_t         nFoundCount         = 0;                // Change of found cameras
    const VmbUint32_t   nTimeout            = 2000;             // Timeout for Grab
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeFull;// We open the camera with full access
    VmbHandle_t         cameraHandle        = NULL;             // A handle to our camera
    VmbBool_t           bIsCommandDone      = VmbBoolFalse;     // Has a command finished execution
    VmbFrame_t          frame;                                  // The frame we capture
    const char*         pPixelFormat        = NULL;             // The pixel format we use for acquisition
    VmbInt64_t          nPayloadSize        = 0;                // The size of one frame
    AVTBitmap           bitmap;                                 // The bitmap we create
    
    PrintVimbaVersion();

    if ( VmbErrorSuccess == err )
    {
        // Is Vimba connected to a GigE transport layer?
        DiscoverGigECameras();
        
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
                    // Actually query all static details of all known cameras without having to open the cameras
                    // If a new camera was connected since we queried the amount of cameras (nFoundCount > nCount) we can ignore that one
                    err = VmbCamerasList( pCameras, nCount, &nFoundCount, sizeof *pCameras );
                    if (    VmbErrorSuccess != err
                         && VmbErrorMoreData != err )
                    {
                        printf( "Could not list cameras. Error code: %d\n", err );
                    }
                    else
                    {
                        // Use the first camera
                        if( nFoundCount != 0)
                        {
                            pCameraID = pCameras[0].cameraIdString;
                        }
                        else
                        {
                            pCameraID = NULL;
                            err = VmbErrorNotFound;
                            printf( "Camera lost.\n" );
                        }
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
                            err = VmbFrameAnnounce( cameraHandle, &frame, (VmbUint32_t)sizeof( VmbFrame_t ));
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
                                                printf( "Could not capture frame. Error code: %d\n", err );
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
                                            printf( "Could not start acquisition. Error code: %d\n", err );
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
