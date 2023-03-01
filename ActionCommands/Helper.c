/*=============================================================================
  Copyright (C) 2014 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        Helper.c

  Description: Helper functions used by the example.

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

#include <stdlib.h>
#include <string.h>

#include "Helper.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/ListInterfaces.h>
#include <VmbCExamplesCommon/ListTransportLayers.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>


VmbError_t GetInterfaceByHandle(VmbInterfaceInfo_t* pInterfaces, const VmbUint32_t interfaceCount, VmbInterfaceInfo_t* infoToFill, VmbHandle_t interfaceHandle);
VmbError_t GetTransportLayerByHandle(VmbTransportLayerInfo_t* pTransportLayers, const VmbUint32_t transportLayerCount, VmbTransportLayerInfo_t* infoToFill, VmbHandle_t transportLayerHandle);
VmbBool_t HasGivenTlVendor(VmbCameraInfo_t* pCameraToCheck, const char* pTlVendor);
VmbError_t FindMatchingCameraByIdAndType(const char* pCameraId, VmbTransportLayerType_t interfaceType, VmbCameraInfo_t* pCameraInfo);

VmbError_t FindMatchingCamera(VmbTransportLayerType_t interfaceType, const char* const pTlVendor, VmbCameraInfo_t* pCameraInfo)
{
    VmbCameraInfo_t*            pCameras = 0;
    VmbUint32_t                 cameraCount = 0;

    VmbTransportLayerInfo_t*    pTransportLayers = 0;
    VmbUint32_t                 transportLayerCount = 0;

    VmbInterfaceInfo_t*         pInterfaces = 0;
    VmbUint32_t                 interfaceCount = 0;

    VmbUint32_t                 i = 0;

    VmbError_t err = ListCameras(&pCameras, &cameraCount);
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    err = ListInterfaces(&pInterfaces, &interfaceCount);
    if (err != VmbErrorSuccess)
    {
        free(pCameras);
        return err;
    }

    if (pTlVendor != 0)
    {
        err = ListTransportLayers(&pTransportLayers, &transportLayerCount);
        if (err != VmbErrorSuccess)
        {
            free(pCameras);
            free(pInterfaces);
            return err;
        }
    }

    VmbBool_t cameraFound = VmbBoolFalse;
    for (i = 0; (i < cameraCount) && !cameraFound; i++)
    {
        VmbCameraInfo_t currentCamera = pCameras[i];

        VmbInterfaceInfo_t camerasInterface;
        err = GetInterfaceByHandle(pInterfaces, interfaceCount, &camerasInterface, currentCamera.interfaceHandle);
        if (err != VmbErrorSuccess)
        {
            continue;
        }

        VmbBool_t matchingInterfaceType = (camerasInterface.interfaceType == interfaceType);
        if (!matchingInterfaceType)
        {
            continue;
        }
        else if (pTlVendor == 0)
        {
            *pCameraInfo = currentCamera;
            cameraFound = VmbBoolTrue;
            continue;
        }

        VmbTransportLayerInfo_t camerasTransportLayer;
        err = GetTransportLayerByHandle(pTransportLayers, transportLayerCount, &camerasTransportLayer, currentCamera.interfaceHandle);
        if (err != VmbErrorSuccess)
        {
            continue;
        }

        if (strcmp(pTlVendor, camerasTransportLayer.transportLayerVendor) == 0)
        {
            *pCameraInfo = currentCamera;
            cameraFound = VmbBoolTrue;
        }
    }

    free(pCameras);
    free(pInterfaces);
    if (pTlVendor != 0)
    {
        free(pTransportLayers);
    }

    return (cameraFound) ? VmbErrorSuccess : VmbErrorNotFound;
}

VmbError_t FindMatchingCameraById(const char* pCameraId, VmbTransportLayerType_t interfaceType, const char* const pTlVendor, VmbCameraInfo_t* pCameraInfo)
{
    VmbUint32_t cameraCount = 0;
    VmbError_t err = VmbCamerasList(NULL, 0, &cameraCount, 0);
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    err = FindMatchingCameraByIdAndType(pCameraId, interfaceType, pCameraInfo);
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    if (pTlVendor == NULL)
    {
        return err;
    }

    VmbBool_t matchingVendor = HasGivenTlVendor(pCameraInfo, pTlVendor);

    return (matchingVendor) ? VmbErrorSuccess : VmbErrorNotFound;
}

VmbError_t StartApi()
{
    VmbError_t err = VmbStartup(NULL);
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    PrintVmbVersion();

    return err;
}

VmbError_t GetInterfaceByHandle(VmbInterfaceInfo_t* pInterfaces, const VmbUint32_t interfaceCount, VmbInterfaceInfo_t* infoToFill, VmbHandle_t interfaceHandle)
{
    VmbUint32_t         i = 0;
    VmbInterfaceInfo_t* info = 0;
    for (i = 0; (i < interfaceCount) && (info == 0); i++)
    {
        VmbInterfaceInfo_t current = pInterfaces[i];
        if (current.interfaceHandle == interfaceHandle)
        {
            info = pInterfaces + i;
        }
    }

    if (info != 0)
    {
        *infoToFill = *info;
    }

    return (info != 0) ? VmbErrorSuccess : VmbErrorNotFound;
}

VmbError_t GetTransportLayerByHandle(VmbTransportLayerInfo_t* pTransportLayers, const VmbUint32_t transportLayerCount, VmbTransportLayerInfo_t* infoToFill, VmbHandle_t transportLayerHandle)
{
    VmbUint32_t                 i = 0;
    VmbTransportLayerInfo_t* info = 0;

    for (i = 0; (i < transportLayerCount) && (info == 0); i++)
    {
        VmbTransportLayerInfo_t current = pTransportLayers[i];
        if (current.transportLayerHandle == transportLayerHandle)
        {
            info = pTransportLayers + i;
        }
    }

    if (info != 0)
    {
        *infoToFill = *info;
    }

    return (info != 0) ? VmbErrorSuccess : VmbErrorNotFound;
}


VmbError_t FindMatchingCameraByIdAndType(const char* pCameraId, VmbTransportLayerType_t interfaceType, VmbCameraInfo_t* pCameraInfo)
{
    VmbInterfaceInfo_t* pInterfaces = 0;
    VmbUint32_t                 interfaceCount = 0;

    VmbBool_t                   cameraFound = VmbBoolFalse;

    VmbError_t err = ListInterfaces(&pInterfaces, &interfaceCount);
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    VmbCameraInfo_t cameraInfo;
    err = VmbCameraInfoQuery(pCameraId, &cameraInfo, sizeof(cameraInfo));
    if (err != VmbErrorSuccess)
    {
        free(pInterfaces);
        return err;
    }

    VmbInterfaceInfo_t camerasInterface;
    err = GetInterfaceByHandle(pInterfaces, interfaceCount, &camerasInterface, cameraInfo.interfaceHandle);
    if (err != VmbErrorSuccess)
    {
        free(pInterfaces);
        return err;
    }

    VmbBool_t matchingInterfaceType = (camerasInterface.interfaceType == interfaceType);

    if (matchingInterfaceType)
    {
        *pCameraInfo = cameraInfo;
        cameraFound = VmbBoolTrue;
    }

    free(pInterfaces);

    return (cameraFound) ? VmbErrorSuccess : VmbErrorNotFound;
}

VmbBool_t HasGivenTlVendor(VmbCameraInfo_t* pCameraToCheck, const char* pTlVendor)
{
    VmbTransportLayerInfo_t* pTransportLayers = 0;
    VmbUint32_t                 transportLayerCount = 0;

    VmbError_t err = ListTransportLayers(&pTransportLayers, &transportLayerCount);
    if (err != VmbErrorSuccess)
    {
        return VmbBoolFalse;
    }

    VmbTransportLayerInfo_t camerasTransportLayer;
    err = GetTransportLayerByHandle(pTransportLayers, transportLayerCount, &camerasTransportLayer, pCameraToCheck->transportLayerHandle);
    if (err != VmbErrorSuccess)
    {
        free(pTransportLayers);
        return VmbBoolFalse;
    }

    VmbBool_t matchingVendor = (strcmp(camerasTransportLayer.transportLayerVendor, pTlVendor) == 0);

    free(pTransportLayers);

    return matchingVendor;
}
