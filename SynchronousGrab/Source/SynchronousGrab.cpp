/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

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

#include <iostream>
#include <fstream>
#include <cstring>
#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <SynchronousGrab.h>
#include <Bitmap.h>

#include <VimbaC/Include/VimbaC.h>

VmbError_t SaveBitmapToFile( VmbFrame_t* pFrame, const char *pPixelFormat, const char* pFileName );

using namespace std;

VmbError_t SynchronousGrab( const char* pCameraID, const char* pFileName )
{
    VmbError_t          err = VmbStartup(); // Initialize the Vimba API
    VmbCameraInfo_t     *pCameras = NULL;   // A list of camera details
    VmbUint32_t         nCount = 0;         // Number of found cameras
    bool                bIsGigE = false;    // GigE transport layer present
    VmbUint32_t         nTimeout = 2000;    // Timeout for Grab


    if ( VmbErrorSuccess == err )
    {
        // Is Vimba connected to a GigE transport layer?
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );
        if ( VmbErrorSuccess == err )
        {
            if( true == bIsGigE )
            {
                // Send discovery packets to GigE cameras
                err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");
                if ( VmbErrorSuccess == err )
                {
                    // And wait for them to return
#ifdef WIN32
                    ::Sleep(200);
#else
                    ::usleep(200 * 1000);
#endif
                }
                else
                {
                    cout << "Could not ping GigE cameras over the network. Reason: " << err << endl << endl;
                }
            }
        }
        else
        {
            cout << "Could not query Vimba for the presence of a GigE transport layer. Reason: " << err << endl << endl;
        }

        // If no camera ID was provided use the first camera found
        if ( NULL == pCameraID )
        {
            // Get the amount of known cameras
            err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );
            if (    VmbErrorSuccess == err
                 && 0 < nCount )
            {
                pCameras = new VmbCameraInfo_t[ nCount ];
                if ( NULL != pCameras )
                {
                    // Query all static details of all known cameras without having to open the cameras
                    err = VmbCamerasList( pCameras, nCount, &nCount, sizeof *pCameras );
                    if ( VmbErrorSuccess != err )
                    {
                        cout << "Could not list cameras. Error code: " << err << endl;
                    }
                    else
                    {
                        // Use the first camera
                        pCameraID = pCameras[0].cameraIdString;
                    }
                }
                else
                {
                    cout << "Could not allocate camera list." << endl;
                }
            }
            else
            {
                cout << "Could not list cameras or no cameras present. Error code: " << err << endl;
            }
        }

        if ( NULL != pCameraID )
        {
            VmbAccessMode_t cameraAccessMode = VmbAccessModeFull;
            VmbHandle_t cameraHandle = NULL;

            // Open camera
            err = VmbCameraOpen( pCameraID, cameraAccessMode, &cameraHandle );
            if ( VmbErrorSuccess == err )
            {
                cout << "Camera ID: " << pCameraID << endl << endl;

                // Set the GeV packet size to the highest possible value
                // (In this example we do not test whether this cam actually is a GigE cam)
                if ( VmbErrorSuccess == VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" ))
                {
                    bool bIsCommandDone = false;
                    do
                    {
                        if ( VmbErrorSuccess != VmbFeatureCommandIsDone(    cameraHandle,
                                                                            "GVSPAdjustPacketSize",
                                                                            &bIsCommandDone ))
                        {
                            break;
                        }
                    } while ( false == bIsCommandDone );
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
                    const char* pPixelFormat;
                    VmbFeatureEnumGet( cameraHandle, "PixelFormat", &pPixelFormat );

                    if ( VmbErrorSuccess == err )
                    {
                        VmbInt64_t nPayloadSize;
                        // Evaluate frame size
                        err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );
                        if ( VmbErrorSuccess == err )
                        {
                            VmbFrame_t Frame;
                            Frame.buffer        = new char [ (VmbUint32_t)nPayloadSize ];
                            Frame.bufferSize    = (VmbUint32_t)nPayloadSize;

                            // Announce Frame
                            err = VmbFrameAnnounce( cameraHandle, &Frame, (VmbUint32_t)sizeof( VmbFrame_t ) );
                            if ( VmbErrorSuccess == err )
                            {
                                // Start Capture Engine
                                err = VmbCaptureStart( cameraHandle );
                                if ( VmbErrorSuccess == err )
                                {
                                    // Queue Frame
                                    err = VmbCaptureFrameQueue( cameraHandle, &Frame, NULL );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        // Start Acquisition
                                        err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            // Capture one frame synchronously
                                            err = VmbCaptureFrameWait( cameraHandle, &Frame, nTimeout );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                // Convert the captured frame to a bitmap and save to disk
                                                if ( VmbFrameStatusComplete == Frame.receiveStatus )
                                                {
                                                    AVTBitmap b;
                                                    b.bufferSize = Frame.imageSize;
                                                    b.width = Frame.width;
                                                    b.height = Frame.height;
                                                    // We only support Mono and RGB in this example
                                                    if ( 0 == strcmp( "RGB8Packed", pPixelFormat ))
                                                    {
                                                        b.colorCode = ColorCodeRGB24;
                                                    }
                                                    else
                                                    {
                                                        b.colorCode = ColorCodeMono8;
                                                    }

                                                    // Create the bitmap
                                                    if ( false == CreateBitmap( b, Frame.buffer ))
                                                    {
                                                        cout << "Could not create bitmap." << endl;
                                                    }
                                                    else
                                                    {
                                                        // Save the bitmap
                                                        WriteBitmapToFile( b, pFileName );
                                                        cout << "Bitmap successfully written to file \"" << pFileName << "\"." << endl;
                                                        // Release the bitmap's buffer
                                                        ReleaseBitmap( b );
                                                    }
                                                }
                                                else
                                                {
                                                    cout << "Frame not successfully received. Error code: " << Frame.receiveStatus << endl;
                                                }
                                            }
                                            else
                                            {
                                                cout << "Could not capture frame. Error code: " << err << endl;
                                            }

                                            // Stop Acquisition
                                            err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStop" );
                                            if ( VmbErrorSuccess != err )
                                            {
                                                cout << "Could not stop acquisition. Error code: " << err << endl;
                                            }
                                        }
                                        else
                                        {
                                            cout << "Could not start acquisition. Error code: " << err << endl;
                                        }
                                    }
                                    else
                                    {
                                        cout << "Could not queue frame. Error code: " << err << endl;
                                    }

                                    // Stop Capture Engine
                                    err = VmbCaptureEnd( cameraHandle );
                                    if ( VmbErrorSuccess != err )
                                    {
                                        cout << "Could not end capture . Error code: " << err << endl;
                                    }
                                }
                                else
                                {
                                    cout << "Could not start capture. Error code: " << err << endl;
                                }

                                // Revoke frame
                                err = VmbFrameRevoke( cameraHandle, &Frame );
                                if ( VmbErrorSuccess != err )
                                {
                                    cout << "Could not revoke frame. Error code: " << err << endl;
                                }
                            }
                            else
                            {
                                cout << "Could not announce frame. Error code: " << err << endl;
                            }

                            delete (const char*)Frame.buffer;
                        }
                    }
                    else
                    {
                        cout << "Could not set pixel format to either RGB or Mono. Error code: " << err << endl;
                    }
                }
                else
                {
                    cout << "Could not adjust packet size. Error code: " << err << endl;
                }

                err = VmbCameraClose ( cameraHandle );
                if ( VmbErrorSuccess != err )
                {
                    cout << "Could not close camera. Error code: " << err << endl;
                }
            }
            else
            {
                cout << "Could not open camera. Error code: " << err << endl;
            }
        }
        VmbShutdown();
    }
    else
    {
        cout << "Could not start system. Error code: " << err << endl;
    }

    return err;
}
