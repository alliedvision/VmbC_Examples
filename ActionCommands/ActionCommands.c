/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ActionCommands.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

#include <VmbC/VmbC.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/**
 * \brief Suspend the execution of the current thread for 100 milliseconds
 */
void Sleep100Ms(void)
{
    #ifdef WIN32
        Sleep(100);
    #else
        struct timespec duration;
        duration.tv_sec = 0;
        duration.tv_nsec = 100000000;
        nanosleep(&duration, NULL);
    #endif
}

VmbError_t SendActionCommand(const ActionCommandsOptions* const pOptions, const VmbCameraInfo_t* const pCamera)
{
    /*
    Decide which GenTL module is used to send the Action Commands - Interface or Transport Layer.
    Use of the Interface module follows the GenTL SFNC standard. The AVT GigE TL implements
    the features also in the Transport Layer module. This is useful to send the Action Command to
    multiple cameras connected to different interfaces.
    */
    const VmbHandle_t handleToUse = (pOptions->useAllInterfaces) ? pCamera->transportLayerHandle : pCamera->interfaceHandle;

    VmbError_t error = VmbErrorUnknown;

    // Prepare the Action Command features for unicast or broadcast usage.
    if (pOptions->sendAsUnicast)
    {
        error = PrepareActionCommandAsUnicast(handleToUse, pOptions, pCamera);
    }
    else
    {
        error = PrepareActionCommand(handleToUse, pOptions);
    }

    if (error != VmbErrorSuccess)
    {
        return error;
    }

    // Executing the feature ActionCommand sends the Action Command.
    error = VmbFeatureCommandRun(handleToUse, "ActionCommand");
    if (error != VmbErrorSuccess)
    {
        printf("Failed to run feature command \"ActionCommand\". Reason: %s", ErrorCodeToMessage(error));
        return error;
    }

    /*
    Based on the used Transport Layer, an acknowledgement of the Action Command may be sent by the camera to the host
    in order to complete the command. It's recommended to query the completion of the Action Command to detect a
    wrong configuration of the related trigger and Action Command features.
    */
    VmbBool_t actionCmdDone = VmbBoolFalse;
    size_t    retries = 10;
    do
    {
        error = VmbFeatureCommandIsDone(handleToUse, "ActionCommand", &actionCmdDone);
        if (error != VmbErrorSuccess)
        {
            printf("Failed to query completion of feature command \"ActionCommand\". Reason: %s", ErrorCodeToMessage(error));
        }
        else if (actionCmdDone != VmbBoolTrue)
        {
            retries -= 1;
            Sleep100Ms();
        }
    } while ((actionCmdDone != VmbBoolTrue) && (error == VmbErrorSuccess) && (retries > 0));

    if (actionCmdDone == VmbBoolTrue)
    {
        printf("Sending Action Command succeeded.\n");
    }
    else
    {
        printf("Sending Action Command timed out.\n");
    }

    return error;
}

VmbError_t PrepareCameraForActionCommands(const VmbHandle_t camera)
{
    /*
    The camera must be configured to process Action Commands.
    The following features are configured accordingly: TriggerSelector, TriggerSource and TriggerMode
    */

    VmbError_t error = VmbFeatureEnumSet(camera, "TriggerSelector", "FrameStart");
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"TriggerSelector\" to \"FrameStart\". Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    error = VmbFeatureEnumSet(camera, "TriggerSource", "Action0");
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"TriggerSource\" to \"Action0\". Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    error = VmbFeatureEnumSet(camera, "TriggerMode", "On");
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"TriggerMode\" to \"On\". Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    return VmbErrorSuccess;
}

VmbError_t PrepareActionCommand(const VmbHandle_t handle, const ActionCommandsOptions* const pOptions)
{
    /*
    The provided Action Command information is stored in the Interface or Transport Layer module.
    Features to set: ActionDeviceKey, ActionGroupKey, ActionGroupMask and GevActionDestinationIPAddress for the unicast.
    */

    VmbError_t error = VmbFeatureIntSet(handle, "ActionDeviceKey", pOptions->deviceKey);
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"ActionDeviceKey\" to %u. Reason: %s\n", pOptions->deviceKey, ErrorCodeToMessage(error));
    }

    error = VmbFeatureIntSet(handle, "ActionGroupKey", pOptions->groupKey);
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"ActionGroupKey\" to %u. Reason: %s\n", pOptions->groupKey, ErrorCodeToMessage(error));
    }

    error = VmbFeatureIntSet(handle, "ActionGroupMask", pOptions->groupMask);
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"ActionGroupMask\" to %u. Reason: %s\n", pOptions->groupMask, ErrorCodeToMessage(error));
    }

    return VmbErrorSuccess;
}

VmbError_t PrepareActionCommandAsUnicast(const VmbHandle_t handle, const ActionCommandsOptions* const pOptions, const VmbCameraInfo_t* const pCameraInfo)
{
    /*
    The provided Action Command information is stored in the Interface or Transport Layer module.
    Features to set: ActionDeviceKey, ActionGroupKey and ActionGroupMask.
    */

    VmbError_t error = PrepareActionCommand(handle, pOptions);
    if (error != VmbErrorSuccess)
    {
        return error;
    }


    /*
    Action Commands are sent out as broadcast if GevActionDestinationIPAddress is set to 0.
    The camera's IP address is read from the Local Device module and written to the feature to enable the unicast.
    */

    VmbInt64_t cameraIp = 0;
    error = VmbFeatureIntGet(pCameraInfo->localDeviceHandle, "GevDeviceIPAddress", &cameraIp); //Using GenTL SFNC features to read the camera's IP address.
    if (error != VmbErrorSuccess)
    {
        printf("Could not get feature \"GevDeviceIPAddress\". Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    error = VmbFeatureIntSet(handle, "GevActionDestinationIPAddress", cameraIp);
    if (error != VmbErrorSuccess)
    {
        printf("Could not set feature \"GevActionDestinationIPAddress\" to %X. Reason: %s\n", (VmbUint32_t)cameraIp, ErrorCodeToMessage(error));
        return error;
    }

    return error;
}
