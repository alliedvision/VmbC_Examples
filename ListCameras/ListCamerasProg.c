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

#include "ListCamerasProg.h"

#include <VmbC/VmbC.h>

#include "../Common/ArrayAlloc.h"
#include "../Common/ListCameras.h"
#include "../Common/ListInterfaces.h"
#include "../Common/ListTransportLayers.h"
#include "../Common/PrintVmbVersion.h"

int ListCamerasProg()
{
    VmbUint32_t         i               = 0;
    VmbUint32_t         nCount          = 0;
    VmbUint32_t         nFoundCount     = 0;
    
    VmbError_t err = VmbStartup(NULL);                                                      // Initialize the Vmb API
    PrintVmbVersion();

    if (VmbErrorSuccess == err)
    {
        VmbCameraInfo_t* cameras = NULL;
        VmbUint32_t cameraCount;
        VmbInterfaceInfo_t* interfaces = NULL;
        VmbUint32_t interfaceCount;
        VmbTransportLayerInfo_t* transportLayers = NULL;
        VmbUint32_t transportLayerCount;

        if ((err = ListCameras(&cameras, &cameraCount)) == VmbErrorSuccess
            && (err = ListInterfaces(&interfaces, &interfaceCount)) == VmbErrorSuccess
            && (err = ListTransportLayers(&transportLayers, &transportLayerCount)) == VmbErrorSuccess)
        {
            printf("Cameras found: %u\n\n", cameraCount);

            VmbCameraInfo_t* const camerasEnd = cameras + cameraCount;
            VmbInterfaceInfo_t* const interfacesEnd = interfaces + interfaceCount;
            VmbTransportLayerInfo_t* const tlsEnd = transportLayers + transportLayerCount;

            for (VmbCameraInfo_t* cam = cameras; cam != camerasEnd; ++cam)                  // And print them out
            {
                printf("/// Camera Name         : %s\n"
                       "/// Model Name          : %s\n"
                       "/// Camera ID           : %s\n"
                       "/// Serial Number       : %s\n",
                       cam->cameraName,
                       cam->modelName,
                       cam->cameraIdString,
                       cam->serialString
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
                    printf("/// @ Interface ID      : %s\n", foundIFace->interfaceIdString);
                }

                // find corresponding tl
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
                    printf("/// @ Transport Layer ID: %s\n\n\n", foundTl->transportLayerIdString);
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
        
        VmbShutdown();                                                                      // Close Vmb
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
    return err == VmbErrorSuccess ? 0 : 1;
}
