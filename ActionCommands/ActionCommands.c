/*=============================================================================
  Copyright (C) 2014 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ActionCommands.c

  Description: The ActionCommands example will grab images asynchronously.
               The acquisition of each image is triggered by an Action Command.

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
#include <stdlib.h>
#include <string.h>

#include "ActionCommands.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>

/**
 * \brief number of frames used for the acquisition
 */
#define NUM_FRAMES ((size_t)5)

/**
 * \brief feature name of custom command for choosing the packet size provided by the AVT GigETL
 */
#define ADJUST_PACKAGE_SIZE_COMMAND "GVSPAdjustPacketSize"

//VmbBool_t               g_vmbStarted               = VmbBoolFalse;      // Remember if Vmb is started
//VmbBool_t               g_streaming                = VmbBoolFalse;      // Remember if Vmb is streaming
//VmbBool_t               g_acquiring                = VmbBoolFalse;      // Remember if Vmb is acquiring
//VmbHandle_t             g_cameraHandle             = NULL;              // A handle to our camera
//VmbFrame_t              g_frames[NUM_FRAMES];                           // The frames we capture into

VmbError_t SendActionCommandOnSingleInterface(ActionCommandsOptions* options)
{
    return VmbErrorAlready;
}

/**
 * \brief send an Action Command on all interfaces to acquire and grab an image
 *
 * Note: Vmb has to be uninitialized and the camera has to allow access mode full.
 *       The Allied Vision GigETL is needed since transport layer features are used
 *       which are not defined by the GenTL SFNC. The sent out Action Commands are
 *       still compliant to the GigE Vision specification.
 *
 * \param[in] options                 struct with command line options and details for the Action Command
 */
VmbError_t SendActionCommandOnAllInterfaces(ActionCommandsOptions* options)
{
    return VmbErrorAlready;
}

/**
 *\brief called from Vmb if a frame is ready for user processing
 * 
 * \param[in] cameraHandle handle to camera that supplied the frame
 * \param[in] streamHandle handle to stream that supplied the frame
 * \param[in] frame pointer to frame structure that can hold valid data
 */
void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, const VmbHandle_t streamHandle, VmbFrame_t* frame)
{
    //
    // from here on the frame is under user control until returned to Vmb by requeuing it
    // if you want to have smooth streaming keep the time you hold the frame short
    //

    //
    // Note:    If VmbCaptureEnd is called asynchronously, while this callback is running, VmbCaptureEnd blocks,
    //          until the callback returns.
    //

    //Irgendeine Ausgabe das der Frame empfangen wurde

    // requeue the frame so it can be filled again
    VmbCaptureFrameQueue(cameraHandle, frame, &FrameCallback);
}