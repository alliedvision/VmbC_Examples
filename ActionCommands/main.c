/*=============================================================================
  Copyright (C) 2013 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        main.c

  Description: Implementation of main entry point of ActionCommands example
               of VmbC.

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
#include <string.h>

#include <VmbC/VmbC.h>

#include "ActionCommands.h"
#include "Helper.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

#define VMB_PARAM_PRINT_HELP        "/h"
#define VMB_PARAM_ON_ALL_INTERFACES "/a"

#define AVT_GIGETL_VENDOR "Allied Vision Technologies"

void PrintUsage()
{
    printf( "Usage: ActionCommands [CameraID] [/a] [/h]\n"
            "Parameters:    CameraID    ID of the camera to use (using first camera if not specified)\n"
            "               %s          Send the Action Command on all interfaces (requires the AVT GigETL)\n"
            "               %s          Print out help\n",
            VMB_PARAM_ON_ALL_INTERFACES,
            VMB_PARAM_PRINT_HELP);
}

VmbError_t ParseCommandLineParameters(ActionCommandsOptions* cmdOptions, VmbBool_t* printHelp, int argc, char* argv[])
{
    VmbError_t result = VmbErrorSuccess;

    *printHelp                          = VmbBoolFalse;
    cmdOptions->useAllInterfaces        = VmbBoolFalse;
    cmdOptions->cameraId                = NULL;

    // Action Command information to be set in the camera
    cmdOptions->deviceKey               = 1;
    cmdOptions->groupKey                = 1;
    cmdOptions->groupMask               = 1;

    char** const paramsEnd = argv + argc;
    for (char** param = argv + 1; result == VmbErrorSuccess && param != paramsEnd; ++param)
    {
        size_t const len = strlen(*param);
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
                }
            }
            else if (0 == strcmp(*param, VMB_PARAM_ON_ALL_INTERFACES))
            {
                cmdOptions->useAllInterfaces = VmbBoolTrue;
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
    return result;
}

int main(int argc, char* argv[])
{
    printf("/////////////////////////////////////////\n"
           "/// Vmb API Action Commands Example ///\n"
           "/////////////////////////////////////////\n\n");

    ActionCommandsOptions cmdOptions;
    VmbBool_t printHelp;
    VmbCameraInfo_t cameraToUse;

    VmbError_t err = ParseCommandLineParameters(&cmdOptions, &printHelp, argc, argv);
    
    if (err != VmbErrorSuccess || printHelp)
    {
        PrintUsage();
        return err;
    }

    err = StartApi();
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    const char* tlVendor = (cmdOptions.useAllInterfaces) ? AVT_GIGETL_VENDOR : NULL;
    memset(&cameraToUse, 0, sizeof(cameraToUse));

    if(cmdOptions.cameraId != NULL)
    {
        err = FindMatchingCameraById(cmdOptions.cameraId, VmbTransportLayerTypeGEV, tlVendor, &cameraToUse);
    }
    else
    {
        err = FindMatchingCamera(VmbTransportLayerTypeGEV, tlVendor, &cameraToUse);
    }

    if (err != VmbErrorSuccess)
    {
        printf("No camera found which could be used for the example. Onyl: %s\n", cameraToUse.cameraIdString);
        VmbShutdown();
        return err;
    }

    printf("Using camera %s\n", cameraToUse.cameraIdString);


    VmbShutdown();
    return err;
}
