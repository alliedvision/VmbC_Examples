/*=============================================================================
  Copyright (C) 2013-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <string.h>

#include <VmbC/VmbC.h>

#include "AsynchronousGrab.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI ConsoleHandler(DWORD signal)
{
    switch (signal)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
        StopContinuousImageAcquisition();
    }
    return TRUE;
}

#endif

#define VMB_PARAM_RGB "/r"
#define VMB_PARAM_COLOR_PROCESSING "/c"
#define VMB_PARAM_FRAME_INFOS "/i"
#define VMB_PARAM_SHOW_CORRUPT_FRAMES "/a"
#define VMB_PARAM_ALLOC_AND_ANNOUNCE "/x"
#define VMB_PARAM_PRINT_HELP "/h"

void PrintUsage(void)
{
    printf("Usage: AsynchronousGrab [CameraID] [/i] [/h]\n"
           "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n"
           "              %s          Convert to RGB and show RGB values\n"
           "              %s          Enable color processing (includes %s)\n"
           "              %s          Show frame infos\n"
           "              %s          Automatically only show frame infos of corrupt frames\n"
           "              %s          AllocAndAnnounce mode: Buffers are allocated by the GenTL producer\n"
           "              %s          Print out help\n",
           VMB_PARAM_RGB,
           VMB_PARAM_COLOR_PROCESSING,
           VMB_PARAM_RGB,
           VMB_PARAM_FRAME_INFOS,
           VMB_PARAM_SHOW_CORRUPT_FRAMES,
           VMB_PARAM_ALLOC_AND_ANNOUNCE,
           VMB_PARAM_PRINT_HELP);
}

VmbError_t ParseCommandLineParameters(AsynchronousGrabOptions* cmdOptions, VmbBool_t* printHelp, int argc, char* argv[])
{
    VmbError_t result = VmbErrorSuccess;

    *printHelp                          = VmbBoolFalse;
    cmdOptions->frameInfos              = FrameInfos_Undefined;
    cmdOptions->showRgbValue            = VmbBoolFalse;
    cmdOptions->enableColorProcessing   = VmbBoolFalse;
    cmdOptions->allocAndAnnounce        = VmbBoolFalse;
    cmdOptions->cameraId                = NULL;

    char** const paramsEnd = argv + argc;
    for (char** param = argv + 1; result == VmbErrorSuccess && param != paramsEnd; ++param)
    {
        size_t const len = strlen(*param);
        if (len != 0)
        {
            if (0 == strcmp(*param, VMB_PARAM_FRAME_INFOS))
            {
                if (cmdOptions->frameInfos == FrameInfos_Undefined)
                {
                    cmdOptions->frameInfos = FrameInfos_Show;
                }
                else if (cmdOptions->frameInfos != FrameInfos_Show)
                {
                    printf("conflicting frame info printing already specified\n");
                    result = VmbErrorBadParameter;
                }
            }
            else if (0 == strcmp(*param, VMB_PARAM_RGB))
            {
                cmdOptions->showRgbValue = VmbBoolTrue;
            }
            else if (0 == strcmp(*param, VMB_PARAM_SHOW_CORRUPT_FRAMES))
            {
                if (cmdOptions->frameInfos == FrameInfos_Undefined)
                {
                    cmdOptions->frameInfos = FrameInfos_Automatic;
                }
                else if (cmdOptions->frameInfos != FrameInfos_Automatic)
                {
                    printf("conflicting frame info printing already specified\n");
                    result = VmbErrorBadParameter;
                }
            }
            else if (0 == strcmp(*param, VMB_PARAM_COLOR_PROCESSING))
            {
                cmdOptions->enableColorProcessing = VmbBoolTrue;
                cmdOptions->showRgbValue = VmbBoolTrue;
            }
            else if (0 == strcmp(*param, VMB_PARAM_ALLOC_AND_ANNOUNCE))
            {
                cmdOptions->allocAndAnnounce = VmbBoolTrue;
            }
            else if (0 == strcmp(*param, VMB_PARAM_PRINT_HELP))
            {
                if (argc != 2)
                {
                    printf("%s is required to be the only command line parameter\n\n", VMB_PARAM_PRINT_HELP);
                    result = VmbErrorBadParameter;
                }
                else
                {
                    *printHelp = VmbBoolTrue;
                }
            }
            else if (**param == '/')
            {
                printf("unknown command line option: %s\n", *param);
                result = VmbErrorBadParameter;
            }
            else
            {
                if (cmdOptions->cameraId == NULL)
                {
                    cmdOptions->cameraId = *param;
                }
                else
                {
                    printf("Multiple camera ids specified: \"%s\" and \"%s\"\n", cmdOptions->cameraId, *param);
                    result = VmbErrorBadParameter;
                }
            }
        }
        else
        {
            result = VmbErrorBadParameter;
        }
    }
    if (cmdOptions->frameInfos == FrameInfos_Undefined)
    {
        cmdOptions->frameInfos = FrameInfos_Off;
    }
    return result;
}

int main(int argc, char* argv[])
{
    printf("/////////////////////////////////////////\n"
           "/// Vmb API Asynchronous Grab Example ///\n"
           "/////////////////////////////////////////\n\n");

    AsynchronousGrabOptions cmdOptions;
    VmbBool_t printHelp;
    VmbError_t err = ParseCommandLineParameters(&cmdOptions, &printHelp, argc, argv);

    StreamStatistics streamStatistics = { 0, 0, 0, 0, 0 };

    if (err == VmbErrorSuccess && !printHelp)
    {
#ifdef _WIN32
        SetConsoleCtrlHandler(ConsoleHandler, TRUE);
#endif
        err = StartContinuousImageAcquisition(&cmdOptions, &streamStatistics);
        if (VmbErrorSuccess == err)
        {
            printf("Press <enter> to stop acquisition...\n");
            ((void)getchar());

            StopContinuousImageAcquisition();
            printf("\nAcquisition stopped.\n\n");

            if (cmdOptions.frameInfos != FrameInfos_Off)
            {
                printf("Frames complete   = %llu\n", streamStatistics.framesComplete);
                printf("Frames incomplete = %llu\n", streamStatistics.framesIncomplete);
                printf("Frames too small  = %llu\n", streamStatistics.framesTooSmall);
                printf("Frames invalid    = %llu\n\n", streamStatistics.framesInvalid);
                VmbUint64_t framesTotal = streamStatistics.framesComplete +
                                  streamStatistics.framesIncomplete +
                                  streamStatistics.framesTooSmall +
                                  streamStatistics.framesInvalid;
                printf("Frames total      = %llu\n", framesTotal);
                printf("Frames missing    = %llu\n", streamStatistics.framesMissing);
            }
        }
        else
        {
            printf("\nAn error occurred: %s\n", ErrorCodeToMessage(err));
        }
    }
    else
    {
        PrintUsage();
    }

    return err == VmbErrorSuccess ? 0 : 1;
}
