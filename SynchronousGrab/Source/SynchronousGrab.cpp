#include <iostream>
#include <windows.h>

#include <SynchronousGrab.h>

#include <VimbaC/Include/VimbaC.h>

VmbError_t SynchronousGrab( char* pCameraID, char* pFileName )
{
	VmbError_t			err = VmbStartup();															// Initialize the Vimba API
    VmbCameraInfo_t	   *pCameras = NULL;															// A list of camera details
    VmbUint32_t			nCount = 0;																	// Number of found cameras
    bool				bIsGigE = false;															// GigE transport layer present
	VmbUint32_t			nTimeout = 2000;															// Timeout for Grab


    if ( VmbErrorSuccess == err )
    {
        err = VmbFeatureBoolGet( gVimbaHandle, "GeVTLIsPresent", &bIsGigE );						// Is Vimba connected to a GigE transport layer?
        if (    VmbErrorSuccess == err
             && true == bIsGigE )
        {
            err = VmbFeatureCommandRun( gVimbaHandle, "GeVDiscoveryAllOnce");						// Send discovery packets to GigE cameras
            if ( VmbErrorSuccess == err )
            {
                Sleep( 200 );																		// And wait for them to return
            }
            else
            {
                std::cout << "Could not ping GigE cameras over the network. Reason: " << err << std::endl << std::endl;
            }
        }
        else
        {
            std::cout << "Could not query Vimba for the presence of a GigE transport layer. Reason: " << err << std::endl << std::endl;
        }        

        if ( NULL == pCameraID )																	
		{
			err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );								// Get the amount of known cameras
			if (    VmbErrorSuccess == err
				 && 0 < nCount )
			{
				pCameras = new VmbCameraInfo_t[ nCount ];
				if ( NULL != pCameras )
				{
					err = VmbCamerasList( pCameras, nCount, &nCount, sizeof *pCameras );			// Query all static details of all known cameras
																									// without having to open the cameras
					if ( VmbErrorSuccess != err )
					{
						std::cout << "Could not allocate camera list." << std::endl;
					}
				}
				else
				{
					std::cout << "Could not allocate camera list." << std::endl;
				}
			}
			else
			{
				std::cout << "Could not list cameras or no cameras present. Error code: " << err << std::endl;
			}
		}

		VmbAccessMode_t cameraAccessMode = VmbAccessModeFull;
		VmbHandle_t		cameraHandle = NULL;
		err = VmbCameraOpen(	pCameraID ? pCameraID : pCameras[0].cameraIdString,
								cameraAccessMode, &cameraHandle );									// Open camera
		if ( VmbErrorSuccess == err )
		{				
			VmbInt64_t nPayloadSize;
			err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );					// Evaluate frame size	
			if ( VmbErrorSuccess == err )
			{
				VmbFrame_t Frame;

				VmbUint32_t nSizeOfFrame = (VmbUint32_t)nPayloadSize;
					
				Frame.buffer		= new char [ nSizeOfFrame ];
				Frame.bufferSize	= nSizeOfFrame;

				err = VmbFrameAnnounce( cameraHandle, &Frame, (VmbUint32_t)sizeof( VmbFrame_t ) );	// Announce Frame
				if ( VmbErrorSuccess == err )
				{
					err = VmbCaptureStart( cameraHandle );
					if ( VmbErrorSuccess == err )
					{
						err = VmbCaptureFrameQueue( cameraHandle, &Frame, NULL );					// Queue Frame
						if ( VmbErrorSuccess == err )
						{
							err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
							if ( VmbErrorSuccess == err )
							{
								err = VmbCaptureFrameWait( cameraHandle, &Frame, nTimeout );		// Capture frame
								if ( VmbErrorSuccess == err )
								{
									CreateImageFile( &Frame, pFileName );
								}
								else
								{
									std::cout << "Could not	capture frame. Error code: " << err << std::endl;
								}

								err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStop" );
								if ( VmbErrorSuccess != err )
								{
									std::cout << "Could not	stop acquisition. Error code: " << err << std::endl;
								}
							}
							else
							{
								std::cout << "Could not	start acquisition. Error code: " << err << std::endl;
							}
						}
						else
						{
							std::cout << "Could not	queue frame. Error code: " << err << std::endl;
						}

						err = VmbCaptureEnd( cameraHandle );
						if ( VmbErrorSuccess != err )
						{
							std::cout << "Could not	end capture . Error code: " << err << std::endl;
						}
					}
					else
					{
						std::cout << "Could not	start capture. Error code: " << err << std::endl;
					}

					err = VmbFrameRevoke( cameraHandle, &Frame );									// Revoke frame
					if ( VmbErrorSuccess != err )
					{
						std::cout << "Could not revoke frame. Error code: " << err << std::endl;
					}
				}
				else
				{
					std::cout << "Could not announce frame. Error code: " << err << std::endl;
				}

				delete ( Frame.buffer );

			}

			err = VmbCameraClose ( cameraHandle );													// Close Vimba
			if ( VmbErrorSuccess != err )
			{
				std::cout << "Could not close camera. Error code: " << err << std::endl;
			}
		}
		else
		{
			std::cout << "Could not open camera. Error code: " << err << std::endl;
		}

        VmbShutdown();																					
    }
    else
    {
        std::cout << "Could not start system. Error code: " << err << std::endl;
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
		case VmbPixelFormatBgr8:
			nBitCount = 24;
			break;
		default:
			std::cout << "Pixel Format can not be displayed. Pixel Format: " << pFrame->pixelFormat << std::endl;
			break;
	}

	if ( nBitCount )
	{
		// create bitmap
		HBITMAP memBM =  CreateBitmap( pFrame->width, pFrame->height, 1, nBitCount, NULL );  

		SelectObject ( memDC, memBM );
		SetTextColor ( memDC, RGB ( 0, 0, 255 ) );

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
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 

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
    DWORD dwBytesWritten;		// bytes written

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
        std::cout << "Could not create raw file." << std::endl;
        return hf;
    }

    if ( !WriteFile ( hf, (LPSTR) pFrame->buffer, (int) pFrame->bufferSize, (LPDWORD) &dwBytesWritten, NULL ) ) 
    {
        std::cout << "Could not write raw data." << std::endl;
        return FALSE;
    }

    // Close the .RAW file. 
    if ( !CloseHandle(hf) ) 
    {
        std::cout << "Could not close raw file." << std::endl;
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
    DWORD dwBytesWritten;		// bytes written	 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc ( GMEM_FIXED, pbih->biSizeImage );

    if (!lpBits)
    {
        std::cout << "Could not allocate memory for bitmap file." << std::endl;
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
        std::cout << "Could not create bitmap file." << std::endl;
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
        std::cout << "Could not write bitmap file header." << std::endl;
        return hf;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if ( !WriteFile ( hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)+ pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwBytesWritten, ( NULL) ) )
    {
        std::cout << "Could not write bitmap info header." << std::endl;
        return FALSE;
    }

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 

    if ( !WriteFile ( hf, (LPSTR) pFrame->buffer, (int) pFrame->bufferSize, (LPDWORD) &dwBytesWritten, NULL ) ) 
    {
        std::cout << "Could not write bitmap data." << std::endl;
        return FALSE;
    }

    // Close the .BMP file. 
    if ( !CloseHandle(hf) ) 
    {
        std::cout << "Could not close bitmap file." << std::endl;
        return FALSE;
    }

    // Free memory. 
    HGLOBAL hgl = GlobalFree ( (HGLOBAL)lpBits );
	if ( NULL != hgl )
	{
        std::cout << "Could not free memory for bitmap file." << std::endl;
	}

    return hf;
}
