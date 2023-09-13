/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "ListFeatures.h"

#include <VmbC/VmbC.h>

#include <VmbCExamplesCommon/ArrayAlloc.h>
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>
#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/ListInterfaces.h>
#include <VmbCExamplesCommon/ListTransportLayers.h>
#include <VmbCExamplesCommon/TransportLayerTypeToString.h>

/**
 * \return \p string or an empty string, if \p string is null
 */
char const* PrintableString(char const* string)
{
    return string == NULL ? "" : string;
}

/**
 * Prints out all features and their values and details of a given handle.
 *
 * \param[in] moduleHandle The handle to print the features for
 */
VmbError_t ListFeatures(VmbHandle_t const moduleHandle, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    VmbUint32_t featureCount = 0;

    // Get the number of features
    VmbError_t err = VmbFeaturesList(moduleHandle,
                                     NULL,
                                     0,
                                     &featureCount,
                                     sizeof(VmbFeatureInfo_t));
    if (VmbErrorSuccess == err
        && 0 < featureCount)
    {
        VmbFeatureInfo_t* features = VMB_MALLOC_ARRAY(VmbFeatureInfo_t, featureCount);

        if (NULL != features)
        {
            // Get the feature info
            err = VmbFeaturesList(moduleHandle,
                                  features,
                                  featureCount,
                                  &featureCount,
                                  sizeof(VmbFeatureInfo_t));

            if (VmbErrorSuccess == err)
            {
                // buffer to reuse for string feature retrieval
                char* stringBuffer = NULL;
                size_t stringBufferSize = 0;


                VmbFeatureInfo_t* const end = features + featureCount;

                for (VmbFeatureInfo_t* feature = features; feature != end; ++feature)
                {
                    if (feature->visibility > printedFeatureMaximumVisibility)
                    {
                        // ignore feature with visibility that shouldn't be printed
                        continue;
                    }
                    printf("/// Feature Name: %s\n", PrintableString(feature->name));
                    printf("/// Display Name: %s\n", PrintableString(feature->displayName));
                    printf("/// Tooltip: %s\n", PrintableString(feature->tooltip));
                    printf("/// Description: %s\n", PrintableString(feature->description));
                    printf("/// SNFC Namespace: %s\n", PrintableString(feature->sfncNamespace));
                    printf("/// Value: ");

                    VmbBool_t readable;
                    if (VmbFeatureAccessQuery(moduleHandle, feature->name, &readable, NULL) != VmbErrorSuccess)
                    {
                        printf("Unable to determine, if the feature is readable\n");
                    }
                    else if (!readable)
                    {
                        printf("The feature is not readable\n");
                    }
                    else
                    {
                        switch (feature->featureDataType)
                        {
                        case VmbFeatureDataBool:
                        {
                            VmbBool_t value;
                            err = VmbFeatureBoolGet(moduleHandle, feature->name, &value);
                            if (VmbErrorSuccess == err)
                            {
                                printf("%d\n", value);
                            }
                            break;
                        }
                        case VmbFeatureDataEnum:
                        {
                            char const* value = NULL;
                            err = VmbFeatureEnumGet(moduleHandle, feature->name, &value);
                            if (VmbErrorSuccess == err)
                            {
                                printf("%s\n", value);
                            }
                            break;
                        }
                        case VmbFeatureDataFloat:
                        {
                            double value;
                            err = VmbFeatureFloatGet(moduleHandle, feature->name, &value);
                            if (err == VmbErrorSuccess)
                            {
                                printf("%f\n", value);
                            }
                            break;
                        }
                        case VmbFeatureDataInt:
                        {
                            VmbInt64_t value;
                            err = VmbFeatureIntGet(moduleHandle, feature->name, &value);
                            if (err == VmbErrorSuccess)
                            {
                                printf("%lld\n", value);
                            }
                            break;
                        }
                        case VmbFeatureDataString:
                        {
                            VmbUint32_t size = 0;
                            err = VmbFeatureStringGet(moduleHandle, feature->name, NULL, 0, &size);
                            if (VmbErrorSuccess == err
                                && size > 0)
                            {
                                if (stringBufferSize < size)
                                {
                                    char* newBuffer = realloc(stringBuffer, size);
                                    if (newBuffer != NULL)
                                    {
                                        stringBuffer = newBuffer;
                                        stringBufferSize = size;
                                    }
                                }

                                if (stringBufferSize >= size)
                                {
                                    err = VmbFeatureStringGet(moduleHandle, feature->name, stringBuffer, size, &size);
                                    if (VmbErrorSuccess == err)
                                    {
                                        printf("%s\n", stringBuffer);
                                    }
                                }
                                else
                                {
                                    printf("Could not allocate sufficient memory for string.\n");
                                }
                            }
                            break;
                        }
                        case VmbFeatureDataCommand:
                        default:
                            printf("[None]\n");
                            break;
                        }
                    }

                    if (VmbErrorSuccess != err)
                    {
                        printf("Could not get feature value. Error code: %s\n", ErrorCodeToMessage(err));
                    }

                    printf("\n");
                }

                free(stringBuffer);
                err = VmbErrorSuccess; // reset error to state prior to error on individual feature
            }
            else
            {
                printf("Could not get features. Error code: %d\n", err);
            }

            free(features);
        }
        else
        {
            printf("Could not allocate feature list.\n");
            err = VmbErrorResources;
        }
    }
    else
    {
        printf("Could not get features or the module does not provide any. Error code: %d\n", err);
    }
    return VmbErrorSuccess;
}

int ListTransportLayerFeatures(size_t tlIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        VmbUint32_t count = 0;
        VmbTransportLayerInfo_t* tls = NULL;

        err = ListTransportLayers(&tls, &count);
        if (err == VmbErrorSuccess)
        {
            size_t index = tlIndex;
            if (index >= count)
            {
                printf("Transport layer index out of bounds; using index %zu instead\n", (index = count - 1));
            }
            VmbTransportLayerInfo_t* tl = tls + index;

            printf("Transport layer id     : %s\n"
                    "Transport layer model  : %s\n"
                    "Transport layer name   : %s\n"
                    "Transport layer path   : %s\n"
                    "Transport layer type   : %s\n"
                    "Transport layer vendor : %s\n"
                    "Transport layer version: %s\n\n",
                    PrintableString(tl->transportLayerIdString),
                    PrintableString(tl->transportLayerModelName),
                    PrintableString(tl->transportLayerName),
                    PrintableString(tl->transportLayerPath),
                    TransportLayerTypeToString(tl->transportLayerType),
                    PrintableString(tl->transportLayerVendor),
                    PrintableString(tl->transportLayerVersion));

            err = ListFeatures(tl->transportLayerHandle, printedFeatureMaximumVisibility);
            free(tls);
        }
        else
        {
            printf("Error listing the transport layers: %d\n", err);
        }

        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}

int ListInterfaceFeatures(size_t interfaceIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        VmbUint32_t count = 0;
        VmbInterfaceInfo_t* interfaces = NULL;
        err = ListInterfaces(&interfaces, &count);
        if (err == VmbErrorSuccess)
        {
            size_t index = interfaceIndex;
            if (index >= count)
            {
                printf("Interface index out of bounds; using index %zu instead\n", (index = count - 1));
            }
            VmbInterfaceInfo_t* iFace = interfaces + index;
            printf("Interface id  : %s\n"
                    "Interface name: %s\n"
                    "Interface type: %s\n\n",
                    PrintableString(iFace->interfaceIdString),
                    PrintableString(iFace->interfaceName),
                    TransportLayerTypeToString(iFace->interfaceType));
            err = ListFeatures(iFace->interfaceHandle, printedFeatureMaximumVisibility);
            free(interfaces);
        }
        else if (count == 0)
        {
            printf("No interfaces found\n");
        }
        else
        {
            printf("Error listing the interfaces: %d\n", err);
        }

        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}

VmbCameraInfo_t* GetCameraByIndex(VmbCameraInfo_t* cameraInfos, size_t cameraCount, size_t index)
{
    size_t usedIndex = index;
    if (index >= cameraCount)
    {
        usedIndex = cameraCount - 1;
        printf("Camera index out of range: %zu; using camera at index %zu instead\n", index, usedIndex);
    }
    return cameraInfos + usedIndex;
}


typedef VmbHandle_t (*FeatureModuleExtractor)(VmbHandle_t remoteDevice, VmbCameraInfo_t* cameraInfo, size_t param);

VmbHandle_t CameraModuleExtractor(VmbHandle_t remoteDevice, VmbCameraInfo_t* cameraInfo, size_t param)
{
    return (param == 0) ? cameraInfo->localDeviceHandle : remoteDevice;
}

/**
 * \brief list the features of a module determined based on remote device handle, VmbCameraInfo_t struct and \p featureExtractorParam.
 */
VmbError_t ListCameraRelatedFeatures(char const* cameraId, FeatureModuleExtractor featureExtractor, size_t featureExtractorParam, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    VmbHandle_t remoteDeviceHandle;

    VmbError_t err = VmbCameraOpen(cameraId, VmbAccessModeRead, &remoteDeviceHandle);
    if (err == VmbErrorSuccess)
    {
        VmbCameraInfo_t cameraInfo;
        err = VmbCameraInfoQuery(cameraId, &cameraInfo, sizeof(VmbCameraInfo_t));
        if (err == VmbErrorSuccess)
        {
            printf("Camera id    : %s\n"
                   "Camera name  : %s\n"
                   "Model name   : %s\n"
                   "Serial string: %s\n\n",
                   PrintableString(cameraInfo.cameraIdString),
                   PrintableString(cameraInfo.cameraName),
                   PrintableString(cameraInfo.modelName),
                   PrintableString(cameraInfo.serialString));

            VmbHandle_t handle = featureExtractor(remoteDeviceHandle, &cameraInfo, featureExtractorParam);
            if (handle != NULL)
            {
                err = ListFeatures(handle, printedFeatureMaximumVisibility);
            }
            else
            {
                err = VmbErrorNotFound;
            }
        }
        else
        {
            printf("Error retrieving info for the camera\n");
        }

        VmbCameraClose(remoteDeviceHandle);
    }
    else
    {
        printf("Error opening camera: %d\n", err);
    }
    return err;
}

int ListCameraFeaturesAtIndex(size_t index, bool remoteDevice, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    printf("Printing %s features of the camera at index %zu\n\n", remoteDevice ? "remote device" : "local device", index);
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        VmbUint32_t cameraCount = 0;
        VmbCameraInfo_t* cameras = NULL;

        err = ListCameras(&cameras, &cameraCount);
        if (err == VmbErrorSuccess)
        {
            VmbCameraInfo_t* camera = GetCameraByIndex(cameras, cameraCount, index);
            err = ListCameraRelatedFeatures(camera->cameraIdString, &CameraModuleExtractor, remoteDevice ? 1 : 0, printedFeatureMaximumVisibility);
            free(cameras);
        }
        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}

int ListCameraFeaturesAtId(char const* id, bool remoteDevice, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    printf("Printing %s features of the camera with id %s\n\n", remoteDevice ? "remote device" : "local device", id);
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        err = ListCameraRelatedFeatures(id, &CameraModuleExtractor, remoteDevice ? 1 : 0, printedFeatureMaximumVisibility);
        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}

VmbHandle_t StreamModuleExtractor(VmbHandle_t remoteDevice, VmbCameraInfo_t* cameraInfo, size_t streamIndex)
{
    if (cameraInfo->streamCount == 0)
    {
        printf("Camera does not provide streams\n");
        return NULL;
    }
    size_t usedStreamIndex = streamIndex;
    if (streamIndex >= cameraInfo->streamCount)
    {
        printf("Stream index out of range; using index %zu instead\n", (usedStreamIndex = cameraInfo->streamCount - 1));
    }
    printf("Printing features of stream %zu\n\n", usedStreamIndex);
    return cameraInfo->streamHandles[usedStreamIndex];
}

int ListStreamFeaturesAtIndex(size_t cameraIndex, size_t streamIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    printf("Printing features of stream %zu of the camera at index %zu\n\n", streamIndex, cameraIndex);
    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        VmbUint32_t cameraCount = 0;
        VmbCameraInfo_t* cameras = NULL;

        err = ListCameras(&cameras, &cameraCount);
        if (err == VmbErrorSuccess)
        {
            VmbCameraInfo_t* camera = GetCameraByIndex(cameras, cameraCount, cameraIndex);
            err = ListCameraRelatedFeatures(camera->cameraIdString, &StreamModuleExtractor, streamIndex, printedFeatureMaximumVisibility);
            free(cameras);
        }
        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}

int ListStreamFeaturesAtId(char const* cameraId, size_t streamIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility)
{
    printf("Printing features of stream %zu of the camera with id %s\n\n", streamIndex, cameraId);

    VmbError_t err = VmbStartup(NULL);
    if (err == VmbErrorSuccess)
    {
        err = ListCameraRelatedFeatures(cameraId, &StreamModuleExtractor, streamIndex, printedFeatureMaximumVisibility);
        VmbShutdown();
    }
    return (err == VmbErrorSuccess ? 0 : 1);
}
