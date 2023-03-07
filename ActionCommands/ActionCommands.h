/*=============================================================================
  Copyright (C) 2012 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ActionCommands.h

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

#ifndef ACTION_COMMANDS_H_
#define ACTION_COMMANDS_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Helper sturct containing the provided command line options.
*/
typedef struct ActionCommandsOptions
{
    VmbBool_t   useAllInterfaces;
    VmbBool_t   sendAsUnicast;

    char const* pCameraId;

    VmbUint32_t deviceKey;
    VmbUint32_t groupKey;
    VmbUint32_t groupMask;
} ActionCommandsOptions;

/**
 * \brief send an Action Command to acquire and grab an image
 *
 * \param[in] pOptions  Provided command line options and details for the Action Command
 * \param[in] pCamera   Information about the used and already opened camera
 * 
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t SendActionCommand(ActionCommandsOptions* pOptions, VmbCameraInfo_t* pCamera);

/**
 * \brief Configures the camera to be triggered by Action Commands
 * 
 * \param[in] camera    Handle to the already opened camera
 * 
 * \return An error code indicating success or the type of error that occurred.
*/
VmbError_t PrepareCameraForActionCommands(VmbHandle_t camera);

/**
 * \brief Configure the Action Command features for later sending as broadcast
 *
 * \param[in] handle    Transport Layer or Interface handle used to configure the Action Command features
 * \param[in] pOptions  Provided command line options and details for the Action Command
 *
 * \return An error code indicating success or the type of error that occurred.
*/
VmbError_t PrepareActionCommand(VmbHandle_t handle, ActionCommandsOptions* pOptions);

/**
 * \brief Configure the Action Command features for later sending as unicast directly to the camera
 *
 * \param[in] handle        Transport Layer or Interface handle used to configure the Action Command features
 * \param[in] pOptions      Provided command line options and details for the Action Command
 * \param[in] pCameraInfo   Information about the used and already opened camera
 *
 * \return An error code indicating success or the type of error that occurred.
*/
VmbError_t PrepareActionCommandAsUnicast(VmbHandle_t handle, ActionCommandsOptions* pOptions, VmbCameraInfo_t* pCameraInfo);

#endif
