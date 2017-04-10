/*=============================================================================
  Copyright (C) 2012 - 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        AsynchronousGrab.h

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

#ifndef ASYNCHRONOUS_GRAB_H_
#define ASYNCHRONOUS_GRAB_H_

#include <VimbaC/Include/VmbCommonTypes.h>

typedef enum FrameInfos
{
    FrameInfos_Off,
    FrameInfos_Show,
    FrameInfos_Automatic
} FrameInfos;

VmbError_t StartContinuousImageAcquisition( const char* pCameraID, FrameInfos eFrameInfos, VmbBool_t bEnableColorProcessing, VmbBool_t bRGBValue );
void StopContinuousImageAcquisition();

VmbBool_t   CreateApiLock();
void        DestroyApiLock();
VmbBool_t   AquireApiLock();
void        ReleaseApiLock();


#endif
