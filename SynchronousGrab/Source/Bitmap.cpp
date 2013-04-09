/*=============================================================================
  Copyright (C) 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:         Bitmap.h

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
#include <cstring>

#include <Bitmap.h>

#define THREE_CHANNEL   0xC
#define BMP_HEADER_SIZE 54
#define ALIGNMENT_SIZE  4

bool CreateBitmap( AVTBitmap& rBitmap, const void* pBuffer )
{
    if (    0 == rBitmap.bufferSize
         || 0 == rBitmap.width
         || 0 == rBitmap.height )
    {
        return false;
    }

    unsigned char nNumColors;
    if ( rBitmap.colorCode == (rBitmap.colorCode & THREE_CHANNEL) )
    {
        nNumColors = 3;
    }
    else
    {
        nNumColors = 1;
    }

    // Bitmap padding always is a multiple of four Bytes. If data is not we need to pad with zeros.
    unsigned char nPadLength = (rBitmap.width * nNumColors) % ALIGNMENT_SIZE;
    if ( 0 != nPadLength )
    {
        nPadLength = ALIGNMENT_SIZE - nPadLength;
    }

    unsigned long nPaletteSize = 0;
    if ( ColorCodeRGB24 != rBitmap.colorCode )
    {
        nPaletteSize = 256;
    }
    
    unsigned long nHeaderSize = BMP_HEADER_SIZE + nPaletteSize * 4;
    unsigned char* pBitmapBuffer = new unsigned char[ nHeaderSize + rBitmap.bufferSize + (nPadLength * rBitmap.height) ];
    unsigned long nFileSize = nHeaderSize + rBitmap.bufferSize + (nPadLength * rBitmap.height);

    // Create the bitmap header
    char fileHeader[14] = { 'B','M',                // Default
                            0,0,0,0,                // File size
                            0,0,0,0,                // Reserved
                            0,0,0,0 };              // Offset to image content
    char infoHeader[40] = { 40,0,0,0,               // Size of info header
                            0,0,0,0,                // Width
                            0,0,0,0,                // Height
                            1,0,                    // Default
                            8 * nNumColors, 0 };    // bpp

    // File size
    fileHeader[2] = (char)(nFileSize);
    fileHeader[3] = (char)(nFileSize >> 8);
    fileHeader[4] = (char)(nFileSize >> 16);
    fileHeader[5] = (char)(nFileSize >> 24);
    // Offset to image content
    fileHeader[10] = (char)(nHeaderSize);
    fileHeader[11] = (char)(nHeaderSize >> 8);
    fileHeader[12] = (char)(nHeaderSize >> 16);
    fileHeader[13] = (char)(nHeaderSize >> 24);

    // Width
    infoHeader[ 4] = (char)(rBitmap.width);
    infoHeader[ 5] = (char)(rBitmap.width >> 8);
    infoHeader[ 6] = (char)(rBitmap.width >> 16);
    infoHeader[ 7] = (char)(rBitmap.width >> 24);
    // Height (has to be negative for a top down image)
    infoHeader[ 8] = (char)(-(long)rBitmap.height);
    infoHeader[ 9] = (char)(-(long)rBitmap.height >> 8);
    infoHeader[10] = (char)(-(long)rBitmap.height >> 16);
    infoHeader[11] = (char)(-(long)rBitmap.height >> 24);
    // Image size
    infoHeader[20] = (char)(rBitmap.bufferSize);
    infoHeader[21] = (char)(rBitmap.bufferSize >> 8);
    infoHeader[22] = (char)(rBitmap.bufferSize >> 16);
    infoHeader[23] = (char)(rBitmap.bufferSize >> 24);
    // Palette size
    infoHeader[32] = (char)(nPaletteSize);
    infoHeader[33] = (char)(nPaletteSize >> 8);
    infoHeader[34] = (char)(nPaletteSize >> 16);
    infoHeader[35] = (char)(nPaletteSize >> 24);
    // Used colors
    infoHeader[36] = (char)(nPaletteSize);
    infoHeader[37] = (char)(nPaletteSize >> 8);
    infoHeader[38] = (char)(nPaletteSize >> 16);
    infoHeader[39] = (char)(nPaletteSize >> 24);

    // Write header
    unsigned char* pCurBitmapBuf = pBitmapBuffer;
    memcpy( pCurBitmapBuf, fileHeader, 14 );
    pCurBitmapBuf += 14;
    memcpy( pCurBitmapBuf, infoHeader, 40 );
    pCurBitmapBuf += 40;
    for(unsigned long i = 0; i < nPaletteSize; i++)
    {
        pCurBitmapBuf[0] = (char)(i);
        pCurBitmapBuf[1] = (char)(i);
        pCurBitmapBuf[2] = (char)(i);
        pCurBitmapBuf[3] = 0;
        pCurBitmapBuf += 4;
    }

    // RGB -> BGR (a Windows bitmap is BGR)
    if ( ColorCodeRGB24 == rBitmap.colorCode )
    {
        unsigned char* pCurSrc = (unsigned char*)pBuffer;
        unsigned long px;
        for ( unsigned long y=0; y<rBitmap.height; ++y, pCurBitmapBuf+=nPadLength )
        {
            for (   unsigned long x = 0;
                    x < rBitmap.width;
                    ++x,
                    pCurSrc += 3,
                    pCurBitmapBuf += 3 )
            {
                px = 0;
                // Create a 4 Byte structure to store ARGB (we don't use A)
                px = px | (pCurSrc[0] << 16) | (pCurSrc[1] << 8) | pCurSrc[2];
                // Due to endianess ARGB is stored as BGRA 
                // and we only have to write the first three Bytes
                memcpy( pCurBitmapBuf, &px, 3 );
            }
            // Add padding at the end of each row
            memset( pCurBitmapBuf, 0, nPadLength );
        }
        rBitmap.colorCode = ColorCodeBGR24;
    }
    // Mono8
    else
    {
        if ( 0 == nPadLength )
        {
            memcpy( pCurBitmapBuf, pBuffer, rBitmap.bufferSize );
        }
        else
        {
            unsigned char* pCurSrc = (unsigned char*)pBuffer;
            for (   unsigned long y=0;
                    y<rBitmap.height;
                    ++y,
                    pCurSrc += rBitmap.width * nNumColors )
            {
                // Write a single row of colored pixels
                memcpy( pCurBitmapBuf, pCurSrc, rBitmap.width * nNumColors );
                pCurBitmapBuf += rBitmap.width * nNumColors;
                // Write padding pixels
                memset( pCurBitmapBuf, 0, nPadLength );
                pCurBitmapBuf += nPadLength;
            }
        }
    }

    rBitmap.buffer = pBitmapBuffer;
    rBitmap.bufferSize = nFileSize;
    return true;
}

void ReleaseBitmap( AVTBitmap& rBitmap )
{
    if (    NULL != rBitmap.buffer
         && 0 < rBitmap.bufferSize )
    {
        delete [] (unsigned char*)rBitmap.buffer;
        rBitmap.buffer = NULL;
    }
}

void WriteBitmapToFile( const AVTBitmap &rBitmap, const char* pFileName )
{
    FILE *file;
    file = fopen(pFileName, "wb");
    fwrite(rBitmap.buffer, 1, rBitmap.bufferSize, file );
    fclose(file);
}
