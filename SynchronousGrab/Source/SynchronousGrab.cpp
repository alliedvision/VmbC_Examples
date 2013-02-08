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
#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include <SynchronousGrab.h>

#include <VimbaC/Include/VimbaC.h>

using namespace std;

VmbError_t SynchronousGrab( const char* pCameraID, char* pFileName )
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
        if (    VmbErrorSuccess == err
             && true == bIsGigE )
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
        else
        {
            cout << "Could not query Vimba for the presence of a GigE transport layer. Reason: " << err << endl << endl;
        }        

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

        VmbAccessMode_t cameraAccessMode = VmbAccessModeFull;
        VmbHandle_t cameraHandle = NULL;
        if ( NULL == pCameraID )
        {
            pCameraID = pCameras[0].cameraIdString;
        }

        // Open camera
        err = VmbCameraOpen( pCameraID, cameraAccessMode, &cameraHandle );
        if ( VmbErrorSuccess == err )
        {
            cout << "Camera ID: " << pCameraID << endl << endl;

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

                    VmbUint32_t nSizeOfFrame = (VmbUint32_t)nPayloadSize;

                    Frame.buffer        = new char [ nSizeOfFrame ];
                    Frame.bufferSize    = nSizeOfFrame;

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
                                        err = SaveBitmapToFile( cameraHandle, &Frame, pPixelFormat, pFileName );
                                        if ( VmbErrorSuccess != err )
                                        {
                                            cout << "Could not save bitmap to file. Error code: " << err << endl;
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

                    delete ( Frame.buffer );
                }
            }
            else
            {
                cout << "Could not set pixel format to either RGB or Mono. Error code: " << err << endl;
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

        VmbShutdown();
    }
    else
    {
        cout << "Could not start system. Error code: " << err << endl;
    }

    return err;
}

VmbError_t SaveBitmapToFile( VmbHandle_t hCamera, VmbFrame_t* pFrame, const char *pPixelFormat, const char* pFileName )
{
    if (    NULL == pFrame
         || NULL == pFileName )
    {
        return VmbErrorBadParameter;
    }

    VmbUint32_t nWidthFactor = 1;
    if ( 0 == strcmp("RGB8Packed", pPixelFormat) )
    {
        // An RGB image is three times wider than mono
        nWidthFactor = 3;

        // RGB -> BGR (needed for bitmap)
        unsigned char* pCur = (unsigned char*)pFrame->buffer;
        unsigned long px;
        for (VmbUint32_t i=0; i<pFrame->width*pFrame->height; ++i, pCur+=3)
        {
            px = 0;
            // Create a 4 Byte structure to hold ABGR (we don't use A)
            px = (pCur[0] << 16) | (pCur[1] << 8) | (pCur[2]);
            // Erase A and swap B & R to obtain RGB
            px = (px & 0x00000000) | ((px & 0xFF0000) >> 16) | (px & 0x00FF00) | ((px & 0x0000FF) << 16);
            // Write back to buffer
            pCur[0] = (unsigned char)((px & 0xFF0000) >> 16);
            pCur[1] = (unsigned char)((px & 0x00FF00) >> 8);
            pCur[2] = (unsigned char)(px & 0x0000FF);
        }
    }

    FILE *file;
    int filesize = 54 + pFrame->imageSize;

    // Create the bitmap header
    char fileHeader[14] = { 'B','M',                // Default
                            0,0,0,0,                // Size
                            0,0,0,0,                // Reserved
                            54,0,0,0 };             // Offset to image content
    char infoHeader[40] = { 40,0,0,0,               // Size of info header
                            0,0,0,0,                // Width
                            0,0,0,0,                // Height
                            1,0,                    // Default
                            8 * nWidthFactor, 0 };  // bpp

    // Bitmap size
    fileHeader[2] = (char)(filesize);
    fileHeader[3] = (char)(filesize >> 8);
    fileHeader[4] = (char)(filesize >> 16);
    fileHeader[5] = (char)(filesize >> 24);

    // Width and height (height has to be negative for a top down image)
    infoHeader[4] = (char)( pFrame->width);
    infoHeader[5] = (char)( pFrame->width >> 8 );
    infoHeader[6] = (char)( pFrame->width >> 16 );
    infoHeader[7] = (char)( pFrame->width >> 24 );
    infoHeader[8] = (char)( -(long)pFrame->height );
    infoHeader[9] = (char)( -(long)pFrame->height >> 8 );
    infoHeader[10] = (char)( -(long)pFrame->height >> 16 );
    infoHeader[11] = (char)( -(long)pFrame->height >> 24 );

    // Write header to file
    file = fopen(pFileName,"wb");
    fwrite( fileHeader, 1, 14, file );
    fwrite( infoHeader, 1, 40, file );

    // Bitmap padding always is a multiple of four Bytes. If data is not we need to pad with zeros.
    int nPadSize = (pFrame->width * nWidthFactor) % 4;
    if ( 0 == nPadSize )
    {
        fwrite( pFrame->buffer, 1, pFrame->imageSize, file );
    }
    else
    {
        unsigned char* pCur = (unsigned char*)pFrame->buffer;
        unsigned char* pPad = new unsigned char[nPadSize*nWidthFactor];
        for ( int i=0; i<nPadSize; ++i )
        {
            pPad[i] = 0;
        }

        for ( VmbUint32_t y=0; y<pFrame->height; ++y )
        {
            fwrite( pCur, 1, pFrame->width * nWidthFactor, file );
            fwrite( pPad, 1, nPadSize, file );
            pCur += pFrame->width * nWidthFactor;
        }
    }

    fclose( file );

    return VmbErrorSuccess;
}
