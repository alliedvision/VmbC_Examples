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

typedef struct ActionCommandsOptions
{
    VmbBool_t   useAllInterfaces;
    char const* cameraId;
    VmbUint32_t deviceKey;
    VmbUint32_t groupKey;
    VmbUint32_t groupMask;
} ActionCommandsOptions;

/**
 * \brief send an Action Command to acquire and grab an image
 *
 * \param[in] options                 struct with command line options and details for the Action Command
 */
VmbError_t SendActionCommand(ActionCommandsOptions* pOptions, VmbCameraInfo_t* pCamera);

VmbError_t PrepareCameraForActionCommands(VmbHandle_t camera);

VmbError_t PrepareActionCommand(VmbHandle_t handle, ActionCommandsOptions* pOptions);

#endif
