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
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>

#define AVT_GIGETL_VENDOR "Allied Vision Technologies"

VmbError_t GetInterfaceByHandle(VmbInterfaceInfo_t* pInterfaces, const VmbUint32_t interfaceCount, VmbInterfaceInfo_t* infoToFill, VmbHandle_t interfaceHandle);
VmbError_t GetTransportLayerByHandle(VmbTransportLayerInfo_t* pTransportLayers, const VmbUint32_t transportLayerCount, VmbTransportLayerInfo_t* infoToFill, VmbHandle_t transportLayerHandle);
VmbBool_t HasGivenTlVendor(VmbCameraInfo_t* pCameraToCheck, const char* pTlVendor);
VmbError_t FindMatchingCameraByIdAndType(const char* pCameraId, VmbTransportLayerType_t interfaceType, VmbCameraInfo_t* pCameraInfo);
VmbBool_t HasActionCommandFeatures(VmbHandle_t handle);
VmbError_t FindMatchingCamera(VmbTransportLayerType_t interfaceType, const char* const pTlVendor, VmbCameraInfo_t* pCameraInfo);
VmbError_t FindMatchingCameraById(const char* pCameraId, VmbTransportLayerType_t interfaceType, const char* const pTlVendor, VmbCameraInfo_t* pCameraInfo);

VmbError_t FindCamera(VmbBool_t findAvtCamera, char* const pCameraId, VmbHandle_t* cameraHandle, VmbCameraInfo_t* cameraInfo)
{
    VmbError_t error = VmbErrorUnknown;

    const char* tlVendor = (findAvtCamera) ? AVT_GIGETL_VENDOR : NULL;
    memset(cameraInfo, 0, sizeof(VmbCameraInfo_t));

    //Diesen Block noch auslagern zwecks lesbarkeit
    if (pCameraId != NULL)
    {
        //Hier drin noch loggen wieso die Kamera nicht genommen werden kann
        error = FindMatchingCameraById(pCameraId, VmbTransportLayerTypeGEV, tlVendor, cameraInfo);
    }
    else
    {
        //Hier noch loggen warum Kameras nicht genommen werden können?
        // Und welche Kamera ID von welchem TL genommen wurde?
        error = FindMatchingCamera(VmbTransportLayerTypeGEV, tlVendor, cameraInfo);
    }

    return error;
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

VmbError_t FindMatchingCamera(VmbTransportLayerType_t interfaceType, const char* const pTlVendor, VmbCameraInfo_t* pCameraInfo)
{
    VmbCameraInfo_t* pCameras = 0;
    VmbUint32_t                 cameraCount = 0;

    VmbTransportLayerInfo_t* pTransportLayers = 0;
    VmbUint32_t                 transportLayerCount = 0;

    VmbInterfaceInfo_t* pInterfaces = 0;
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

        if ((currentCamera.permittedAccess & VmbAccessModeFull) != VmbAccessModeFull)
        {
            printf("Ignoring camera \"%s\" (already used by other application)\n", currentCamera.cameraIdString);
            continue;
        }

        VmbBool_t matchingInterfaceType = (camerasInterface.interfaceType == interfaceType);
        if (!matchingInterfaceType)
        {
            printf("Ignoring camera \"%s\" (wrong interface type)\n", currentCamera.cameraIdString);
            continue;
        }

        VmbBool_t interfaceHasActionCommand = HasActionCommandFeatures(currentCamera.interfaceHandle);
        if (!interfaceHasActionCommand)
        {
            printf("Ignoring camera \"%s\" (transport layer without Action Command features on interface level)\n", currentCamera.cameraIdString);
            continue;
        }
        else if (pTlVendor == NULL)
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
        else
        {
            printf("Ignoring camera \"%s\" (camera does not belong to the Allied Vision GigETL)\n", currentCamera.cameraIdString);
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
        printf("Camera \"%s\" not found\n", pCameraId);
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
    if (!matchingInterfaceType)
    {
        printf("Camera \"%s\" has the wrong interface type.\n", pCameraId);
        free(pInterfaces);
        return VmbErrorNotFound;
    }

    if ((cameraInfo.permittedAccess & VmbAccessModeFull) != VmbAccessModeFull)
    {
        printf("Ignoring camera \"%s\" (already used by other application)\n", cameraInfo.cameraIdString);
        free(pInterfaces);
        return VmbErrorNotFound;
    }

    VmbBool_t interfaceHasActionCommand = HasActionCommandFeatures(cameraInfo.interfaceHandle);
    if (!interfaceHasActionCommand)
    {
        printf("Camera \"%s\" belongs to a transport layer without Action Command features on interface level.\n", cameraInfo.cameraIdString);
        free(pInterfaces);
        return VmbErrorNotFound;
    }

    *pCameraInfo = cameraInfo;
    cameraFound = VmbBoolTrue;

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

    if (!matchingVendor)
    {
        printf("Ignoring camera \"%s\" (camera does not belong to the Allied Vision GigETL)\n", pCameraToCheck->cameraIdString);
    }

    free(pTransportLayers);

    return matchingVendor;
}

VmbBool_t HasActionCommandFeatures(VmbHandle_t handle)
{
    VmbFeatureInfo_t info;

    VmbError_t error = VmbFeatureInfoQuery(handle, "ActionCommand", &info, sizeof(info));

    return (error == VmbErrorSuccess) ? VmbBoolTrue : VmbBoolFalse;
}