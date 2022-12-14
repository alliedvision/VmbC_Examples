/*=============================================================================
  Copyright (C) 2012 - 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListCamerasProg.c

  Description: The ListCameras example will list all available cameras that
               are found by VmbC.

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

#include "ListCamerasProg.h"

#include <VmbC/VmbC.h>

#include <VmbCExamplesCommon/ArrayAlloc.h>
#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/ListInterfaces.h>
#include <VmbCExamplesCommon/ListTransportLayers.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

void concatStrings(char *dest, size_t destSize, const char *src, const char *separator)
{
    if (strlen(dest) > 0)
    {
        strcat_s(dest, destSize, separator);
    }
    strcat_s(dest, destSize, src);
}

void AccessModesToString(VmbAccessMode_t accessMode, char *const buffer, size_t bufferSize)
{
    if (accessMode == VmbAccessModeNone)
    {
        strcpy_s(buffer, bufferSize, "No access");
        return;
    }
    if (accessMode & VmbAccessModeFull)
    {
        concatStrings(buffer, bufferSize, "Full access", ", ");
    }
    if (accessMode & VmbAccessModeRead)
    {
        concatStrings(buffer, bufferSize, "Read access", ", ");
    }
    if (accessMode & VmbAccessModeUnknown)
    {
        concatStrings(buffer, bufferSize, "Unknown access", ", ");
    }
    if (accessMode & VmbAccessModeExclusive)
    {
        concatStrings(buffer, bufferSize, "Exclusive access", ", ");
    }
}

int ListCamerasProg()
{
    PrintVmbVersion();
    VmbError_t err = VmbStartup(NULL);

    if (VmbErrorSuccess == err)
    {
        VmbCameraInfo_t* cameras = NULL;
        VmbUint32_t cameraCount;
        VmbInterfaceInfo_t* interfaces = NULL;
        VmbUint32_t interfaceCount;
        VmbTransportLayerInfo_t* transportLayers = NULL;
        VmbUint32_t transportLayerCount;

        err  = ListTransportLayers(&transportLayers, &transportLayerCount);
        if (err == VmbErrorSuccess) printf("TransportLayers found: %u\n", transportLayerCount);
        err |= ListInterfaces(&interfaces, &interfaceCount);
        if (err == VmbErrorSuccess) printf("Interfaces found: %u\n", interfaceCount);
        err |= ListCameras(&cameras, &cameraCount);

        if (err == VmbErrorSuccess)
        {
            printf("Cameras found: %u\n\n", cameraCount);

            VmbCameraInfo_t* const camerasEnd = cameras + cameraCount;
            VmbInterfaceInfo_t* const interfacesEnd = interfaces + interfaceCount;
            VmbTransportLayerInfo_t* const tlsEnd = transportLayers + transportLayerCount;

            for (VmbCameraInfo_t* cam = cameras; cam != camerasEnd; ++cam)
            {
                char accessModes[200] = "";
                AccessModesToString(cam->permittedAccess, accessModes, sizeof(accessModes) / sizeof(accessModes[0]));
                printf("/// Camera Name            : %s\n"
                       "/// Model Name             : %s\n"
                       "/// Camera ID              : %s\n"
                       "/// Serial Number          : %s\n"
                       "/// Permitted Access Modes : %s\n",
                       cam->cameraName,
                       cam->modelName,
                       cam->cameraIdString,
                       cam->serialString,
                       accessModes
                );

                // find corresponding interface
                VmbInterfaceInfo_t* foundIFace = NULL;
                for (VmbInterfaceInfo_t* iFace = interfaces; foundIFace == NULL && iFace != interfacesEnd; ++iFace)
                {
                    if (cam->interfaceHandle == iFace->interfaceHandle)
                    {
                        foundIFace = iFace;
                    }
                }

                if (foundIFace == NULL)
                {
                    printf("corresponding interface not found\n");
                }
                else
                {
                    printf("/// @ Interface ID         : %s\n", foundIFace->interfaceIdString);
                }

                // find corresponding transport layer
                VmbTransportLayerInfo_t* foundTl = NULL;
                for (VmbTransportLayerInfo_t* tl = transportLayers; foundTl == NULL && tl != tlsEnd; ++tl)
                {
                    if (cam->transportLayerHandle == tl->transportLayerHandle)
                    {
                        foundTl = tl;
                    }
                }

                if (foundTl == NULL)
                {
                    printf("corresponding transport layer not found\n");
                }
                else
                {
                    printf("/// @ Transport Layer ID   : %s\n", foundTl->transportLayerIdString);
                    printf("/// @ Transport Layer Path : %s\n\n\n", foundTl->transportLayerPath);
                }
            }
        }
        else
        {
            printf("listing the modules failed\n");
        }

        free(cameras);
        free(interfaces);
        free(transportLayers);
        
        VmbShutdown();
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
    return err == VmbErrorSuccess ? 0 : 1;
}
