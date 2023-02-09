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
    VmbBool_t   allocAndAnnounce;
    char const* cameraId;
} AsynchronousGrabOptions;

typedef struct FrameStatistics
{
    VmbUint64_t framesComplete;
    VmbUint64_t framesMissing;
    VmbUint64_t framesIncomplete;
    VmbUint64_t framesTooSmall;
    VmbUint64_t framesInvalid;
} StreamStatistics;

/**
 * \brief starts image acquisition on a given camera
 * 
 * Note: Vmb has to be uninitialized and the camera has to allow access mode full
 * 
 * \param[in] options                 struct with command line options (e.g. frameInfos, enableColorProcessing, allocAndAnnounce etc.)
 * \param[in] statistics              struct with stream statistics (framesReceived, framesMissing)
 */
VmbError_t StartContinuousImageAcquisition(AsynchronousGrabOptions* options, StreamStatistics* statistics);

/**
 * \brief stops image acquisition that was started with StartContinuousImageAcquisition
 */
void StopContinuousImageAcquisition();


#endif
