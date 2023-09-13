/*=============================================================================
  Copyright (C) 2013-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <string.h>

#include <VmbC/VmbC.h>

#include "ActionCommands.h"
#include "Helper.h"
#include "ImageAcquisition.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

// Globally stored handle of the used camera (needed for the signal handlers)
VmbHandle_t g_CameraHandle = NULL;

// Used command line parameters
#define VMB_PARAM_PRINT_HELP        "/h"
#define VMB_PARAM_ON_ALL_INTERFACES "/a"
#define VMB_PARAM_AS_UNICAST        "/u"

// Keys used during the example
#define VMB_ACTION_KEY              'a'
#define VMB_QUIT_KEY                'q'

// Used Action Command value
#define VMB_ACTION_DEVICE_KEY       1
#define VMB_ACTION_GROUP_KEY        1
#define VMB_ACTION_GROUP_MASK       1

/**
 * \brief Handling of signals related to forced closing of the example application.
 *
 * Will clean up the API.
 */
void HandleForcedClose(void)
{
    // It's not recommended to call printf during the signal handling.
    // In this example it is called to reduce the complexity.
    printf("Press %c + 'ENTER' to stop the example.\n", VMB_QUIT_KEY);

    StopStream(g_CameraHandle);
    VmbCameraClose(g_CameraHandle);
    VmbShutdown();
}

// OS specific signal handling
#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI ConsoleHandler(DWORD signal)
{
    switch (signal)
    {
        case CTRL_CLOSE_EVENT:
        case CTRL_C_EVENT:
        {
            HandleForcedClose();
        }
    }
    return TRUE;
}
#else
#include <signal.h>

void ConsoleHandler(int signal)
{
    HandleForcedClose();
}

#endif

#define CLEANUP_AND_RETURN(camera, error)   if(camera != NULL)          \
                                            {                           \
                                                VmbCameraClose(camera); \
                                            }                           \
                                            VmbShutdown();              \
                                            return error;

void PrintUsage(void)
{
    printf( "Usage: ActionCommands [CameraID] [%s] [%s] [%s]\n"
            "Parameters:    CameraID    ID of the camera to use (using first camera if not specified)\n"
            "               %s          Send the Action Command on all interfaces (requires the AVT GigETL)\n"
            "               %s          Send the Action Command as unicast directly to the camera (otherwise as broadcast)\n"
            "               %s          Print out help\n",
            VMB_PARAM_ON_ALL_INTERFACES,
            VMB_PARAM_AS_UNICAST,
            VMB_PARAM_PRINT_HELP,
            VMB_PARAM_ON_ALL_INTERFACES,
            VMB_PARAM_AS_UNICAST,
            VMB_PARAM_PRINT_HELP);
}

VmbError_t ParseCommandLineParameters(ActionCommandsOptions* cmdOptions, VmbBool_t* printHelp, int argc, char* argv[])
{
    VmbError_t result   = VmbErrorSuccess;

    *printHelp          = VmbBoolFalse;

    // Parsing of the provided command line parameters

    char** const paramsEnd = argv + argc;
    for ( char** param = argv + 1; result == VmbErrorSuccess && param != paramsEnd; ++param)
    {
        const size_t len = strlen(*param);
        if (len != 0)
        {
            if (0 == strcmp(*param, VMB_PARAM_PRINT_HELP))
            {
                if (argc != 2)
                {
                    printf("%s is required to be the only command line parameter\n\n", VMB_PARAM_PRINT_HELP);
                    result = VmbErrorBadParameter;
                }
                else
                {
                    *printHelp = VmbBoolTrue;
                    break;
                }
            }

            if (0 == strcmp(*param, VMB_PARAM_ON_ALL_INTERFACES))
            {
                cmdOptions->useAllInterfaces = VmbBoolTrue;
                continue;
            }

            if (0 == strcmp(*param, VMB_PARAM_AS_UNICAST))
            {
                cmdOptions->sendAsUnicast = VmbBoolTrue;
                continue;
            }

            if (**param == '/')
            {
                printf("unknown command line option: %s\n", *param);
                result = VmbErrorBadParameter;
            }
            else
            {
                if (cmdOptions->pCameraId == NULL)
                {
                    cmdOptions->pCameraId = *param;
                }
                else
                {
                    printf("Multiple camera ids specified: \"%s\" and \"%s\"\n", cmdOptions->pCameraId, *param);
                    result = VmbErrorBadParameter;
                }
            }
        }
        else
        {
            result = VmbErrorBadParameter;
        }
    }
    return result;
}

int main(int argc, char* argv[])
{
    printf("////////////////////////////////////////\n"
           "/// Vmb API Action Commands Example ////\n"
           "////////////////////////////////////////\n\n");

    ActionCommandsOptions cmdOptions = { VmbBoolFalse, VmbBoolFalse, NULL, VMB_ACTION_DEVICE_KEY, VMB_ACTION_GROUP_KEY, VMB_ACTION_GROUP_MASK };

    VmbBool_t printHelp = VmbBoolFalse;
    VmbCameraInfo_t cameraToUse;
    memset(&cameraToUse, 0, sizeof(cameraToUse));

    VmbError_t error = ParseCommandLineParameters(&cmdOptions, &printHelp, argc, argv);

    if (error != VmbErrorSuccess || printHelp)
    {
        PrintUsage();
        return error;
    }

    #ifdef _WIN32
        SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    #else
        signal(SIGINT, ConsoleHandler);
    #endif

    error = StartApi();
    if (error != VmbErrorSuccess)
    {
        return error;
    }

    // Find a camera which can be used by this example or check the compatibility of the given camera
    error = FindCamera(cmdOptions.useAllInterfaces, cmdOptions.pCameraId, &cameraToUse);
    if (error != VmbErrorSuccess)
    {
        printf("\nNo camera found which could be used by the example");
        CLEANUP_AND_RETURN(g_CameraHandle, error);
    }

    printf("\nUsing camera %s\n", cameraToUse.cameraIdString);

    // Open the found camera
    error = VmbCameraOpen(cameraToUse.cameraIdString, VmbAccessModeFull, &g_CameraHandle);
    if (error != VmbErrorSuccess)
    {
        printf("Could not open %s. Reason: %s\n", cameraToUse.cameraIdString, ErrorCodeToMessage(error));
        CLEANUP_AND_RETURN(g_CameraHandle, error);
    }

    // Update the camera information (some values are only available after the camera is opened)
    error = VmbCameraInfoQuery(cameraToUse.cameraIdString, &cameraToUse, sizeof(cameraToUse));
    if (error != VmbErrorSuccess)
    {
        printf("Could not query camera info for %s. Reason: %s\n", cameraToUse.cameraIdString, ErrorCodeToMessage(error));
        CLEANUP_AND_RETURN(g_CameraHandle, error);
    }

    // Prepare the camera to be triggered by received Action Commands
    error = PrepareCameraForActionCommands(g_CameraHandle);
    if (error != VmbErrorSuccess)
    {
        CLEANUP_AND_RETURN(g_CameraHandle, error);
    }

    //Set up the Action Command values on the camera
    error = PrepareActionCommand(g_CameraHandle, &cmdOptions);
    if (error != VmbErrorSuccess)
    {
        CLEANUP_AND_RETURN(g_CameraHandle, error);
    }

    //Prepare and start the stream
    error = StartStream(g_CameraHandle);

    if (error == VmbErrorSuccess)
    {
        printf("\nExample ready to send Action Commands\n");
        printf("Press %c + ENTER to send an Action Command\n", VMB_ACTION_KEY);
        printf("Press %c + ENTER to quit\n", VMB_QUIT_KEY);

        int key = 0;
        do
        {
            key = getchar();
            if (key == VMB_ACTION_KEY)
            {
                error = SendActionCommand(&cmdOptions, &cameraToUse);
            }
        } while ( (key != VMB_QUIT_KEY) && (g_CameraHandle != NULL));
        printf("Terminating example...\n");
    }

    //Clean up the API before the example is closed

    StopStream(g_CameraHandle);

    VmbCameraClose(g_CameraHandle);

    VmbShutdown();
    return error;
}
