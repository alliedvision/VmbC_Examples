/*=============================================================================
  Copyright (C) 2013 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:         Bitmap.c

  Description:  The bitmap class represents a MS(R) Windows(TM) bitmap version 3

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
#include <string.h>
#include <stdlib.h>

#include <Bitmap.h>

#define THREE_CHANNEL   0xC
#define BMP_HEADER_SIZE 54
#define ALIGNMENT_SIZE  4

//
// Creates a MS Windows bitmap with header and color palette.
// Fills it with the content of the given byte buffer
// 
// Parameters:
//  [out]   pBitmap         A pointer to an AVTBitmap that will get filled
//  [in]    pBuffer         The buffer that will be used to fill the created bitmap
//
// Returns:
//  0 in case of error
//  1 in case of success
//
unsigned char AVTCreateBitmap( AVTBitmap * const pBitmap, const void* pBuffer )
{
    unsigned char   nNumColors      = 0;        // Number of colors of our image
    unsigned char   nPadLength      = 0;        // The padding we need to align the bitmap ALIGNMENT_SIZE
    unsigned long   nPaletteSize    = 0;        // The size of the bitmap's palette
    unsigned long   nHeaderSize     = 0;        // The size of the bitmap's header
    unsigned long   nFileSize       = 0;        // The size of the bitmap file
    unsigned char*  pBitmapBuffer   = 0;        // A buffer we use for creating the bitmap
    unsigned char*  pCurBitmapBuf   = 0;        // A cursor to move over "pBitmapBuffer"
    unsigned char*  pCurSrc         = 0;        // A cursor to move over the given buffer "pBuffer"
    unsigned long   px              = 0;        // A single pixel for storing transformed color information
    unsigned long   x               = 0;        // The horizontal position within our image
    unsigned long   y               = 0;        // The vertical position within our image
    unsigned long   i               = 0;        // Counter for some iteration

    // The bitmap header
    char fileHeader[14] = { 'B','M',            // Default
                            0,0,0,0,            // File size
                            0,0,0,0,            // Reserved
                            0,0,0,0 };          // Offset to image content
    char infoHeader[40] = { 40,0,0,0,           // Size of info header
                            0,0,0,0,            // Width
                            0,0,0,0,            // Height
                            1,0,                // Default
                            0, 0 };             // bpp

    if (    NULL == pBitmap
         || 0 == pBitmap->bufferSize
         || 0 == pBitmap->width
         || 0 == pBitmap->height )
    {
        return 0;
    }

    if ( pBitmap->colorCode == ( pBitmap->colorCode & THREE_CHANNEL ))
    {
        nNumColors = 3;
    }
    else
    {
        nNumColors = 1;
    }

    // Bitmap padding always is a multiple of four Bytes. If data is not we need to pad with zeros.
    nPadLength = ( pBitmap->width * nNumColors ) % ALIGNMENT_SIZE;
    if ( 0 != nPadLength )
    {
        nPadLength = ALIGNMENT_SIZE - nPadLength;
    }

    if ( ColorCodeRGB24 != pBitmap->colorCode )
    {
        nPaletteSize = 256;
    }
    
    nHeaderSize     = BMP_HEADER_SIZE + nPaletteSize * 4;
    pBitmapBuffer   = (unsigned char*)malloc( nHeaderSize + pBitmap->bufferSize + (nPadLength * pBitmap->height ));
    nFileSize       = nHeaderSize + pBitmap->bufferSize + ( nPadLength * pBitmap->height );

    // File size
    fileHeader[ 2]  = (char)( nFileSize );
    fileHeader[ 3]  = (char)( nFileSize >> 8 );
    fileHeader[ 4]  = (char)( nFileSize >> 16 );
    fileHeader[ 5]  = (char)( nFileSize >> 24 );
    // Offset to image content
    fileHeader[10]  = (char)( nHeaderSize );
    fileHeader[11]  = (char)( nHeaderSize >> 8 );
    fileHeader[12]  = (char)( nHeaderSize >> 16 );
    fileHeader[13]  = (char)( nHeaderSize >> 24 );

    // Width
    infoHeader[ 4]  = (char)( pBitmap->width );
    infoHeader[ 5]  = (char)( pBitmap->width >> 8 );
    infoHeader[ 6]  = (char)( pBitmap->width >> 16 );
    infoHeader[ 7]  = (char)( pBitmap->width >> 24 );
    // Height (has to be negative for a top down image)
    infoHeader[ 8]  = (char)( -(long)pBitmap->height );
    infoHeader[ 9]  = (char)( -(long)pBitmap->height >> 8 );
    infoHeader[10]  = (char)( -(long)pBitmap->height >> 16 );
    infoHeader[11]  = (char)( -(long)pBitmap->height >> 24 );
    // bpp
    infoHeader[14]  = 8 * nNumColors;
    // Image size
    infoHeader[20]  = (char)( pBitmap->bufferSize );
    infoHeader[21]  = (char)( pBitmap->bufferSize >> 8 );
    infoHeader[22]  = (char)( pBitmap->bufferSize >> 16 );
    infoHeader[23]  = (char)( pBitmap->bufferSize >> 24 );
    // Palette size
    infoHeader[32]  = (char)( nPaletteSize );
    infoHeader[33]  = (char)( nPaletteSize >> 8 );
    infoHeader[34]  = (char)( nPaletteSize >> 16 );
    infoHeader[35]  = (char)( nPaletteSize >> 24 );
    // Used colors
    infoHeader[36] = (char)( nPaletteSize );
    infoHeader[37] = (char)( nPaletteSize >> 8 );
    infoHeader[38] = (char)( nPaletteSize >> 16 );
    infoHeader[39] = (char)( nPaletteSize >> 24 );

    // Write header
    pCurBitmapBuf = pBitmapBuffer;
    memcpy( pCurBitmapBuf, fileHeader, 14 );
    pCurBitmapBuf += 14;
    memcpy( pCurBitmapBuf, infoHeader, 40 );
    pCurBitmapBuf += 40;
    for( i = 0; i < nPaletteSize; ++i )
    {
        pCurBitmapBuf[0] = (char)(i);
        pCurBitmapBuf[1] = (char)(i);
        pCurBitmapBuf[2] = (char)(i);
        pCurBitmapBuf[3] = 0;
        pCurBitmapBuf += 4;
    }

    // RGB -> BGR (a Windows bitmap is BGR)
    if ( ColorCodeRGB24 == pBitmap->colorCode )
    {
        pCurSrc = (unsigned char*)pBuffer;
        for ( y=0; y<pBitmap->height; ++y, pCurBitmapBuf+=nPadLength )
        {
            for (   x = 0;
                    x < pBitmap->width;
                    ++x,
                    pCurSrc += 3,
                    pCurBitmapBuf += 3 )
            {
                px = 0;
                // Create a 4 Byte structure to store ARGB (we don't use A)
                px = px | ( pCurSrc[0] << 16 ) | ( pCurSrc[1] << 8 ) | pCurSrc[2];
                // Due to endianess ARGB is stored as BGRA 
                // and we only have to write the first three Bytes
                memcpy( pCurBitmapBuf, &px, 3 );
            }
            // Add padding at the end of each row
            memset( pCurBitmapBuf, 0, nPadLength );
        }
        pBitmap->colorCode = ColorCodeBGR24;
    }
    // Mono8
    else
    {
        if ( 0 == nPadLength )
        {
            memcpy( pCurBitmapBuf, pBuffer, pBitmap->bufferSize );
        }
        else
        {
            pCurSrc = (unsigned char*)pBuffer;
            for (   y=0;
                    y<pBitmap->height;
                    ++y,
                    pCurSrc += pBitmap->width * nNumColors )
            {
                // Write a single row of colored pixels
                memcpy( pCurBitmapBuf, pCurSrc, pBitmap->width * nNumColors );
                pCurBitmapBuf += pBitmap->width * nNumColors;
                // Write padding pixels
                memset( pCurBitmapBuf, 0, nPadLength );
                pCurBitmapBuf += nPadLength;
            }
        }
    }

    pBitmap->buffer     = pBitmapBuffer;
    pBitmap->bufferSize = nFileSize;
    return 1;
}

//
// Releases (frees) a given bitmap
//
// Parameters:
//  [in, out]   pBitmap     The bitmap whose memory will be freed
//
// Returns:
//  0 in case of error
//  1 in case of success
//
unsigned char AVTReleaseBitmap( AVTBitmap * const pBitmap )
{
    if (    NULL != pBitmap
         && NULL != pBitmap->buffer
         && 0 < pBitmap->bufferSize )
    {
        free( pBitmap->buffer );
        pBitmap->buffer = NULL;
        return 1;
    }

    return 0;
}

//
// Writes a given bitmap to file
//
// Parameters:
//  [in] pBitmap            The AVTBitmap to write to file
//  [in] pFileName          The destination (complete path) where to write the bitmap to
//
// Returns:
//  0 in case of error
//  1 in case of success
//
unsigned char AVTWriteBitmapToFile( AVTBitmap const * const pBitmap, char const * const pFileName )
{
    FILE *file;
    if (    NULL != pBitmap
         && NULL != pBitmap->buffer
         && NULL != pFileName )
    {
        file = fopen( pFileName, "wb" );
        fwrite( pBitmap->buffer, 1, pBitmap->bufferSize, file );
        fclose( file );

        return 1;
    }

    return 0;
}
