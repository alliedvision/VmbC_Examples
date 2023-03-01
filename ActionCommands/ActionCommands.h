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

#include <VmbC/VmbCommonTypes.h>

typedef struct ActionCommandsOptions
{
    VmbBool_t   useAllInterfaces;
    char const* cameraId;
    VmbUint32_t deviceKey;
    VmbUint32_t groupKey;
    VmbUint32_t groupMask;
} ActionCommandsOptions;


/**
 * \brief send an Action Command on a single interface to acquire and grab an image
 *
 * Note: Vmb has to be uninitialized and the camera has to allow access mode full.
 *       The choosen interface is the one the camera is connected to. The function
 *       uses the standard compliant Action Command features defined by the GenTL SFNC.
 *
 * \param[in] options                 struct with command line options and details for the Action Command
 */
VmbError_t SendActionCommandOnSingleInterface(ActionCommandsOptions* options);

/**
 * \brief send an Action Command on all interfaces to acquire and grab an image
 *
 * Note: Vmb has to be uninitialized and the camera has to allow access mode full.
 *       The AVT GigETL is needed since transport layer features are used
 *       which are not defined by the GenTL SFNC. The sent out Action Commands are
 *       still compliant to the GigE Vision specification.
 *
 * \param[in] options                 struct with command line options and details for the Action Command
 */
VmbError_t SendActionCommandOnAllInterfaces(ActionCommandsOptions* options);

#endif
