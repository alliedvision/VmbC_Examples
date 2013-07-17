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

#ifndef AVT_BITMAP_H
#define AVT_BITMAP_H

typedef enum
{
    ColorCodeMono8  = 1,
    ColorCodeMono16 = 2,
    ColorCodeBGR24  = 4,
    ColorCodeRGB24  = 8
} ColorCode;

typedef struct
{
    void*           buffer;
    unsigned long   bufferSize;
    unsigned long   width; 
    unsigned long   height;
    ColorCode       colorCode;
} AVTBitmap;

unsigned char AVTCreateBitmap( AVTBitmap * const pBitmap, const void* pBuffer );

unsigned char AVTReleaseBitmap( AVTBitmap * const pBitmap );

unsigned char AVTWriteBitmapToFile( AVTBitmap const * const pBitmap, char const * const pFileName );

#endif
