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
#include <windows.h>

#include <SynchronousGrab.h>

#include <VimbaC/Include/VimbaC.h>

using namespace std;

VmbError_t SynchronousGrab( char* pCameraID, char* pFileName )
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
                Sleep( 200 );
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
                        cout << "Could not allocate camera list." << endl;
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
        VmbHandle_t        cameraHandle = NULL;
        if (NULL == pCameraID)
        {
            pCameraID = (char*)pCameras[0].cameraIdString;
        }

        // Open camera
        err = VmbCameraOpen(    pCameraID, cameraAccessMode, &cameraHandle );
        if ( VmbErrorSuccess == err )
        {
            cout << "Camera ID: " << pCameraID << endl << endl;

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
                    err = VmbCaptureStart( cameraHandle );
                    if ( VmbErrorSuccess == err )
                    {
                        // Queue Frame
                        err = VmbCaptureFrameQueue( cameraHandle, &Frame, NULL );
                        if ( VmbErrorSuccess == err )
                        {
                            err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
                            if ( VmbErrorSuccess == err )
                            {
                                // Capture frame
                                err = VmbCaptureFrameWait( cameraHandle, &Frame, nTimeout );
                                if ( VmbErrorSuccess == err )
                                {
                                    CreateImageFile( &Frame, pFileName );
                                }
                                else
                                {
                                    cout << "Could not capture frame. Error code: " << err << endl;
                                }

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

            if ( VmbErrorSuccess != err )
            {
                VmbCameraClose ( cameraHandle );
            }
            else
            {
                err = VmbCameraClose ( cameraHandle );
                if ( VmbErrorSuccess != err )
                {
                    cout << "Could not close camera. Error code: " << err << endl;
                }
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

HANDLE CreateImageFile ( const VmbFrame_t* pFrame, const char* pFileName )
{
    HANDLE          hFile       = INVALID_HANDLE_VALUE;
    HDC             hDC;
    WCHAR           wFileName   [ MAX_PATH ];

    MultiByteToWideChar ( CP_ACP, 0L, pFileName, lstrlenA(pFileName) + 1, wFileName, MAX_PATH );
    
    hDC = GetDC ( NULL );

    HDC memDC = CreateCompatibleDC ( hDC );

    UINT nBitCount = 0;
    switch ( pFrame->pixelFormat)
    {
        case VmbPixelFormatMono8:
            nBitCount = 8;
            break;
        case VmbPixelFormatRgb8:
        case VmbPixelFormatBgr8:
            nBitCount = 24;
            break;
        default:
            break;
    }

    if ( nBitCount )
    {
        // create bitmap
        HBITMAP memBM =  CreateBitmap( pFrame->width, pFrame->height, 1, nBitCount, NULL );  

        PBITMAPINFO pbi = CreateBitmapInfoStruct( memBM );
        if ( NULL == pbi )
        {
            return pbi;
        }
        hFile = CreateBitmapFile ( wFileName, pbi, pFrame );
    }
    else
    {
        hFile = CreateRawFile ( wFileName, pFrame );  
    }
    return hFile;
}

PBITMAPINFO CreateBitmapInfoStruct ( HBITMAP hBmp )
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
    {        
        return NULL;
    } 

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 
    if (cClrBits != 24) 
    {
        pbmi = (PBITMAPINFO) LocalAlloc ( LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * ( (size_t)1 << cClrBits) ); 
    }
    // There is no RGBQUAD array for the 24-bit-per-pixel format. 
    else 
    {
        pbmi = (PBITMAPINFO) LocalAlloc ( LPTR, sizeof(BITMAPINFOHEADER) ); 
    }

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = -bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    pbmi->bmiHeader.biSizeImage = ((bmp.bmWidth * cClrBits +31) & ~31) /8
                                  * bmp.bmHeight; 

    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
    pbmi->bmiHeader.biClrImportant = pbmi->bmiHeader.biClrUsed;

    // mono bitmap
    if ( 8 == cClrBits )
    {
        for( int i = 1; i < (int)pbmi->bmiHeader.biClrUsed; ++i )
        {
            pbmi->bmiColors[i].rgbBlue      = (BYTE) i;
            pbmi->bmiColors[i].rgbGreen     = (BYTE) i;
            pbmi->bmiColors[i].rgbRed       = (BYTE) i;
            pbmi->bmiColors[i].rgbReserved  = (BYTE)(pbmi->bmiHeader.biClrUsed - 1);
        }
    }

    return pbmi; 
} 

HANDLE CreateRawFile ( LPTSTR pszFile, const VmbFrame_t* pFrame )
{
    HANDLE hf;                  // file handle 
    DWORD dwBytesWritten;        // bytes written

    // Create the .BMP file. 
    hf = CreateFile (   pszFile, 
                        GENERIC_READ | GENERIC_WRITE, 
                        (DWORD) 0, 
                        NULL, 
                        CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, 
                        (HANDLE) NULL ); 

    if (hf == INVALID_HANDLE_VALUE)
    {
        cout << "Could not create raw file." << endl;
        return hf;
    }

    if ( !WriteFile ( hf, (LPSTR) pFrame->buffer, (int) pFrame->bufferSize, (LPDWORD) &dwBytesWritten, NULL ) ) 
    {
        cout << "Could not write raw data." << endl;
        return FALSE;
    }

    // Close the .RAW file. 
    if ( !CloseHandle(hf) ) 
    {
        cout << "Could not close raw file." << endl;
        return FALSE;
    }

    return hf;
}

HANDLE CreateBitmapFile ( LPTSTR pszFile, PBITMAPINFO pbi, const VmbFrame_t* pFrame ) 
{ 
    HANDLE hf;                  // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwBytesWritten;        // bytes written     

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc ( GMEM_FIXED, pbih->biSizeImage );

    if (!lpBits)
    {
        cout << "Could not allocate memory for bitmap file." << endl;
        return FALSE;
    }

    // Create the .BMP file. 
    hf = CreateFile (   pszFile, 
                        GENERIC_READ | GENERIC_WRITE, 
                        (DWORD) 0, 
                        NULL, 
                        CREATE_ALWAYS, 
                        FILE_ATTRIBUTE_NORMAL, 
                        (HANDLE) NULL ); 

    if (hf == INVALID_HANDLE_VALUE)
    {
        cout << "Could not create bitmap file." << endl;
        return hf;
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if ( !WriteFile ( hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwBytesWritten,  NULL ) ) 
    {
        cout << "Could not write bitmap file header." << endl;
        return hf;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if ( !WriteFile ( hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)+ pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwBytesWritten, ( NULL) ) )
    {
        cout << "Could not write bitmap info header." << endl;
        return FALSE;
    }

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 

    if ( !WriteFile ( hf, (LPSTR) pFrame->buffer, (int) pFrame->bufferSize, (LPDWORD) &dwBytesWritten, NULL ) ) 
    {
        cout << "Could not write bitmap data." << endl;
        return FALSE;
    }

    // Close the .BMP file. 
    if ( !CloseHandle(hf) ) 
    {
        cout << "Could not close bitmap file." << endl;
        return FALSE;
    }

    // Free memory. 
    HGLOBAL hgl = GlobalFree ( (HGLOBAL)lpBits );
    if ( NULL != hgl )
    {
        cout << "Could not free memory for bitmap file." << endl;
    }

    return hf;
}
