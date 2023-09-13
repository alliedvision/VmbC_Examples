/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
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
void StopContinuousImageAcquisition(void);


#endif
