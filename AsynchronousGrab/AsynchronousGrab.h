/*=============================================================================
  Copyright (C) 2012 - 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        AsynchronousGrab.h

  Description: The AsynchronousGrab example will grab images asynchronously
               using VmbC.

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

#include <VmbC/VmbCommonTypes.h>

typedef enum FrameInfos
{
    FrameInfos_Undefined,
    FrameInfos_Off,
    FrameInfos_Show,
    FrameInfos_Automatic
} FrameInfos;

typedef struct AsynchronousGrabOptions
{
    FrameInfos  frameInfos;
    VmbBool_t   showRgbValue;
    VmbBool_t   enableColorProcessing;
    char const* cameraId;
} AsynchronousGrabOptions;

/**
 * \brief starts image acquisition on a given camera
 * 
 * Note: Vmb has to be uninitialized and the camera has to allow access mode full
 * 
 * \param[in] pCameraId               zero terminated C string with the camera id for the camera to be used
 * \param[in] eFrameInfos             enumeration value for the frame infos to show for received frames
 * \param[in] bEnableColorProcessing  toggle for enabling image processing, in this case just swapping red with blue
 */
VmbError_t StartContinuousImageAcquisition(AsynchronousGrabOptions* options);

/**
 * \brief stops image acquisition that was started with StartContinuousImageAcquisition
 */
void StopContinuousImageAcquisition();


#endif
