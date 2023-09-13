/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Helper.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>
#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>

#define AVT_GIGETL_VENDOR       "Allied Vision Technologies"
#define GENTL_SFNC_GIGE_TYPE    "GigEVision"
#define ALTERNATIVE_GIGE_TYPE   "GEV"

// Helper functions

VmbBool_t BelongsToAvtGigETL(const VmbCameraInfo_t* const pCameraToCheck);
VmbBool_t HasActionCommandFeatures(const VmbHandle_t handle);
VmbBool_t IsGigeCamera(const VmbCameraInfo_t* const pCameraToCheck);
VmbError_t CheckCamera(const VmbCameraInfo_t* const pCameraInfo, const VmbBool_t needsAvtGigETL);
VmbError_t CheckProvidedCamera(const char* const pCameraId, const VmbBool_t needsAvtGigETL, VmbCameraInfo_t* const pCameraInfo);
VmbError_t FindMatchingCamera(const VmbBool_t needsAvtGigETL, VmbCameraInfo_t* const pCameraInfo);

// Implementation

VmbError_t FindCamera(const VmbBool_t needsAvtGigETL, const char* const pCameraId, VmbCameraInfo_t* const pCameraInfo)
{
    VmbError_t error = VmbErrorUnknown;

    memset(pCameraInfo, 0, sizeof(VmbCameraInfo_t));

    if (pCameraId != NULL)
    {
        // Find the first camera which can be used by this example
        error = CheckProvidedCamera(pCameraId, needsAvtGigETL, pCameraInfo);
    }
    else
    {
        // Check if the given camera id belongs to a camera which can be used by this example
        error = FindMatchingCamera(needsAvtGigETL, pCameraInfo);
    }

    return error;
}

VmbError_t StartApi(void)
{
    const VmbError_t error = VmbStartup(NULL);
    if (error != VmbErrorSuccess)
    {
        return error;
    }

    PrintVmbVersion();

    return error;
}

VmbError_t FindMatchingCamera(const VmbBool_t needsAvtGigETL, VmbCameraInfo_t* const pCameraInfo)
{
    VmbCameraInfo_t*    pCameras = 0;
    VmbUint32_t         cameraCount = 0;

    VmbUint32_t         i = 0;

    /*
    Search the camera list for a camera which can be used for the example.
    The used camera must be a GigE Vision camera which is not currently used by another application.
    Action Commands must be supported by the related Transport Layer.
    */

    const VmbError_t error = ListCameras(&pCameras, &cameraCount);
    if (error != VmbErrorSuccess)
    {
        return error;
    }

    VmbBool_t cameraFound = VmbBoolFalse;
    for (i = 0; (i < cameraCount) && !cameraFound; i++)
    {
        cameraFound = (CheckCamera(pCameras + i, needsAvtGigETL) == VmbErrorSuccess);

        if (cameraFound)
        {
            *pCameraInfo = *(pCameras + i);
        }
    }

    free(pCameras);

    return (cameraFound) ? VmbErrorSuccess : VmbErrorNotFound;
}

VmbError_t CheckProvidedCamera(const char* const pCameraId, const VmbBool_t needsAvtGigETL, VmbCameraInfo_t* const pCameraInfo)
{
    VmbCameraInfo_t cameraInfo;
    VmbError_t error = VmbCameraInfoQuery(pCameraId, &cameraInfo, sizeof(cameraInfo));
    if (error != VmbErrorSuccess)
    {
        printf("Camera \"%s\" not found\n", pCameraId);
        return error;
    }

    /*
    Check if the given camera can be used by this example.
    The camera must be a GigE Vision camera which is not currently used by another application.
    Action Commands must be supported by the related Transport Layer.
    */
    error = CheckCamera(&cameraInfo, needsAvtGigETL);
    if (error != VmbErrorSuccess)
    {
        return error;
    }

    *pCameraInfo = cameraInfo;

    return error;
}

VmbError_t CheckCamera(const VmbCameraInfo_t* const pCameraInfo, const VmbBool_t needsAvtGigETL)
{
    //Check that the camera can be opened with VmbAccessModeFull
    if ((pCameraInfo->permittedAccess & VmbAccessModeFull) != VmbAccessModeFull)
    {
        printf("Ignoring camera \"%s\" (already used by other application)\n", pCameraInfo->cameraIdString);
        return VmbErrorNotFound;
    }

    //Check that the camera is connected to a GigE Vision interface
    const VmbBool_t gigeCamera = IsGigeCamera(pCameraInfo);
    if (!gigeCamera)
    {
        return VmbErrorNotFound;
    }

    //Check that the camera's transport layer supports GenTL SFNC Action Commands
    const VmbBool_t interfaceHasActionCommand = HasActionCommandFeatures(pCameraInfo->interfaceHandle);
    if (!interfaceHasActionCommand)
    {
        printf("Ignoring camera \"%s\" (transport layer without Action Command features on interface level)\n", pCameraInfo->cameraIdString);
        return VmbErrorNotFound;
    }

    // Check that the camera belongs to the AVT GigE TL if Action Commands in the Transport Layer module are needed
    if (needsAvtGigETL && !BelongsToAvtGigETL(pCameraInfo))
    {
        return VmbErrorNotFound;
    }

    return VmbErrorSuccess;
}

VmbBool_t BelongsToAvtGigETL(const VmbCameraInfo_t* const pCameraToCheck)
{
    const char* const VENDOR_NAME_FEATURE = "TLVendorName";

    char buffer[128]; //For simplicity, use a fixed-size buffer.
    memset(buffer, 0, sizeof(buffer));

    /*
    Read the vendor name of the transport layer using the camera's transport layer handle.
    Compare the received vendor name with the expected vendor.
    */

    VmbUint32_t writtenBytes = 0;
    const VmbError_t error = VmbFeatureStringGet(pCameraToCheck->transportLayerHandle, VENDOR_NAME_FEATURE, buffer, sizeof(buffer), &writtenBytes);
    if (error != VmbErrorSuccess)
    {
        printf("Could not get feature \"%s\". Reason: %s\n", VENDOR_NAME_FEATURE, ErrorCodeToMessage(error));
        return VmbBoolFalse;
    }

    const VmbBool_t matchingVendor = (strcmp(buffer, AVT_GIGETL_VENDOR) == 0);

    if (!matchingVendor)
    {
        printf("Ignoring camera \"%s\" (Transport Layer vendor is %s, expected %s)\n", pCameraToCheck->cameraIdString, buffer, AVT_GIGETL_VENDOR);
    }

    return matchingVendor;
}

VmbBool_t IsGigeCamera(const VmbCameraInfo_t* const pCameraToCheck)
{
    const char* const INTERFACE_TYPE_FEATURE = "InterfaceType";

    /*
    Read the interface type of the interface the camera is connected to.
    Compare the received type with the expected value specified by the GenTL SFNC.
    */

    const char* typeValue = NULL;
    const VmbError_t error = VmbFeatureEnumGet(pCameraToCheck->interfaceHandle, INTERFACE_TYPE_FEATURE, &typeValue);
    if (error != VmbErrorSuccess)
    {
        printf("Could not get feature \"%s\". Reason: %s\n", INTERFACE_TYPE_FEATURE, ErrorCodeToMessage(error));
        return VmbBoolFalse;
    }

    const VmbBool_t gigeCamera = (strcmp(typeValue, GENTL_SFNC_GIGE_TYPE) == 0) || (strcmp(typeValue, ALTERNATIVE_GIGE_TYPE) == 0);

    if (!gigeCamera)
    {
        printf("Ignoring camera \"%s\" (wrong interface type %s)\n", pCameraToCheck->cameraIdString, typeValue);
    }

    return gigeCamera;
}

VmbBool_t HasActionCommandFeatures(const VmbHandle_t handle)
{
    VmbFeatureInfo_t info;

    const VmbError_t error = VmbFeatureInfoQuery(handle, "ActionCommand", &info, sizeof(info));

    return (error == VmbErrorSuccess) ? VmbBoolTrue : VmbBoolFalse;
}
