/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        main.c

  Description: Main entry point of ListFeatures example of VmbC.

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


#include "../Common/PrintVmbVersion.h"

#include "ListFeatures.h"

#define VMB_PARAM_TL            "/t"
#define VMB_PARAM_INTERFACE     "/i"
#define VMB_PARAM_REMOTE_DEVICE "/c"
#define VMB_PARAM_LOCAL_DEVICE  "/l"
#define VMB_PARAM_STREAM        "/s"
#define VMB_PARAM_USAGE         "/?"

void PrintUsage()
{
    printf("Usage:\n\n"
           "  ListFeatures %s                                          Print this usage information\n"
           "  ListFeatures                                             Print the remote device features of the first camera\n"
           "  ListFeatures %s TransportLayerIndex                      Show Transport Layer features\n"
           "  ListFeatures %s InterfaceIndex                           Show interface features\n"
           "  ListFeatures %s (CameraIndex | CameraId)                 Show the remote device features of the specified camera\n"
           "  ListFeatures %s (CameraIndex | CameraId)                 Show the local device features of the specified camera\n"
           "  ListFeatures %s (CameraIndex | CameraId) [StreamIndex]   Show the features of a stream for the specified camera\n",
           VMB_PARAM_USAGE, VMB_PARAM_TL, VMB_PARAM_INTERFACE, VMB_PARAM_REMOTE_DEVICE, VMB_PARAM_LOCAL_DEVICE, VMB_PARAM_STREAM);
}

int main(int argc, char* argv[])
{
    printf( "\n" );
    printf( "///////////////////////////////////////\n" );
    printf( "/// Vmb API List Features Example   ///\n" );
    printf( "///////////////////////////////////////\n" );
    printf( "\n" );

    PrintVmbVersion();
    printf("\n");

    if(argc < 2)
    {
        return ListCameraFeaturesAtIndex(0, true);
    }
    else
    {
        char const* const moduleCommand = argv[1];
        if (strcmp(moduleCommand, VMB_PARAM_TL) == 0)
        {
            if (argc < 3)
            {
                printf("transport layer index missing but required for option %s\n", VMB_PARAM_TL);
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    printf("transport layer index required but found %s\n", argv[2]);
                }
                else
                {
                    return ListTransportLayerFeatures(index);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_INTERFACE) == 0)
        {
            if (argc < 3)
            {
                printf("interface index missing but required for option %s\n", VMB_PARAM_INTERFACE);
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    printf("interface index required but found %s\n", argv[2]);
                }
                else
                {
                    return ListInterfaceFeatures(index);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_REMOTE_DEVICE) == 0
                 || strcmp(moduleCommand, VMB_PARAM_LOCAL_DEVICE) == 0)
        {
            bool remoteDevice = (strcmp(moduleCommand, VMB_PARAM_REMOTE_DEVICE) == 0);
            if (argc < 3)
            {
                printf("listing device features requires the index or the id of the camera to be provided\n");
            }
            else
            {
                char* end = argv[2];
                unsigned long index = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    // no an index -> try using the parameter as id
                    return ListCameraFeaturesAtId(argv[2], remoteDevice);
                }
                else
                {
                    return ListCameraFeaturesAtIndex(index, remoteDevice);
                }
            }
        }
        else if (strcmp(moduleCommand, VMB_PARAM_STREAM) == 0)
        {
            if (argc < 3)
            {
                printf("listing stream feature requires the index or the id of the camera to be provided\n");
            }
            else
            {
                unsigned long streamIndex = 0;
                if (argc >= 4)
                {
                    char* end = argv[2];
                    streamIndex = strtoul(argv[3], &end, 10);
                    if (*end != '\0')
                    {
                        printf("the index of the stream needs to be valid, but found %s\n", argv[3]);
                        return 1;
                    }
                }
                char* end = argv[2];
                unsigned long cameraIndex = strtoul(argv[2], &end, 10);
                if (*end != '\0')
                {
                    // no an index -> try using the parameter as camers id
                    return ListStreamFeaturesAtId(argv[2], streamIndex);
                }
                else
                {
                    return ListStreamFeaturesAtIndex(cameraIndex, streamIndex);
                }
            }
        }
        else if(strcmp(moduleCommand, VMB_PARAM_USAGE) == 0)
        {
            PrintUsage();
            return 0;
        }
        else
        {
            printf("invalid parameter: %s\n\n", argv[1]);
            PrintUsage();
        }
        return 1;
    }

}
