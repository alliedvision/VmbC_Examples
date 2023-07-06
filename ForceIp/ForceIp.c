/**
 * \date 2022-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of the function which implements the force ip configuration
 */

#define _CRT_SECURE_NO_WARNINGS // disable sscanf warning for Windows

#include "ForceIpProg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#endif

#include <VmbC/VmbC.h>

#include <VmbCExamplesCommon/AccessModeToString.h>
#include <VmbCExamplesCommon/ArrayAlloc.h>
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>
#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/ListInterfaces.h>
#include <VmbCExamplesCommon/ListTransportLayers.h>
#include <VmbCExamplesCommon/IpAddressToHostByteOrderedInt.h>



/**
 * \brief Converts a hexadecimal MAC address into its decimal representation.
 * \param[in] strMAC    The hexadecimal (with preceding 0x) or decimal representation of the MAC
 *
 * \return 0 in case of error, otherwise the decimal representation of the MAC address as integer
*/
unsigned long long MacAddr(const char* const strMAC)
{
    unsigned long long nMAC;

    if (sscanf(strMAC, "0x%llx", &nMAC)
        || sscanf(strMAC, "%lld", &nMAC))
    {
        return nMAC;
    }
    else
    {
        return 0;
    }
}


/*
 * Macros used to control the print out of the progress and any errors.
 * Define _VMB_FORCE_IP_NO_PRINT to disable the print out
 */
#ifdef _VMB_FORCE_IP_NO_PRINT
    #define VMB_PRINT(text, ...)
#else
    #define VMB_PRINT(...) printf(__VA_ARGS__)
#endif



 /*
  * Helper macros used for cleaner error handling
  */
#define _ON_ERROR(op, onError)  {\
                                    VmbError_t error = op; \
                                    if(error != VmbErrorSuccess) \
                                    { \
                                        onError; \
                                    } \
                                }

#define _ON_ERROR_WITH_PRINT(op, onError) _ON_ERROR(op, VMB_PRINT("%s failed. %s Error code: %d. \n", #op, ErrorCodeToMessage(error), error); onError)

#define RETURN_ON_ERROR(op)                             _ON_ERROR(op, return error;)
#define CONTINUE_ON_ERROR(op)                           _ON_ERROR(op, continue;)

#define RETURN_SPECIFIC_AND_PRINT_ON_ERROR(op, retVal)  _ON_ERROR_WITH_PRINT(op, return retVal;)
#define RETURN_AND_PRINT_ON_ERROR(op)                   _ON_ERROR_WITH_PRINT(op, return error;)

#define CONTINUE_AND_PRINT_ON_ERROR(op)                 _ON_ERROR_WITH_PRINT(op, continue;)
#define BREAK_AND_PRINT_ON_ERROR(op)                    _ON_ERROR_WITH_PRINT(op, break;)

/*
 * Helper structs
 */

/**
 * \brief Helper struct used to store information about a found interface
 */
typedef struct InterfaceSearchResult
{
    VmbInterfaceInfo_t  info;           //!< Information about the found interface
    char*               cameraId;       //!< Camera ID as reported by the interface
    VmbInt64_t          deviceSelector; //!< The configured DeviceSelector the camera was found with
} InterfaceSearchResult;

/**
 * \brief Helper struct used to store information about a found transport layer
 */
typedef struct TlSearchResult
{
    VmbTransportLayerInfo_t info;           //!< Information about the found transport layer
    VmbInt64_t              interfaceCount; //!< Number of interfaces of the transport layer
} TlSearchResult;

/*
 * Helper functions
 */

/**
 * \brief Send a force ip command to a camera with the given mac address via an interface that the camera is connected to.
 * Each found interface will be used until success, otherwise an error is returned.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] macValue             The mac address of the camera
 * \param[in] ipValue              The desired ip address
 * \param[in] subnetMaskValue      The desired subnet mask
 * \param[in] gatewayValue         The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t ForceIpViaInterface(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue);

 /**
  * \brief Send a force ip command to a camera with the given mac address via a transport layer.
  * Each found Vimba X transport layer will be used until success, otherwise an error is returned.
  * The new configuration will be lost after a power-cycle of the camera.
  *
  * \param[in] macValue             The mac address of the camera
  * \param[in] ipValue              The desired ip address
  * \param[in] subnetMaskValue      The desired subnet mask
  * \param[in] gatewayValue         The desired gateway
  *
  * \return Result of the operation
 */
VmbError_t ForceIpViaTl(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue);

/**
 * \brief Search for interfaces to which a camera with the given mac address is connected
 *
 * \param[in] mac                          The camera's mac address
 * \param[out] interfaceSearchResults      The interfaces to which the camera is connected.
 *                                         The first one will be an interface of a Vimba X transport layer, if available,
 *                                         and the second one that of a Vimba one, if available
 * \param[out] interfaceSearchResultsCount The number of interfaces to which the camera is connected
 *
 * \return Result of the operation
*/
VmbError_t SearchCamerasInterface(const VmbInt64_t mac, InterfaceSearchResult** interfaceSearchResults, VmbUint32_t* interfaceSearchResultsCount);

/**
 * \brief Search for transport layers to which a camera with the given mac address is connected
 *
 * \param[in]  tls                        The available transport layers
 * \param[in]  tlCount                    The number of available transport layers
 * \param[out] tlVimbaXSearchResult      The Vimba X transport layers to which the camera is connected.
 * \param[out] tlVimbaXSearchResultsCount The number of Vimba X transport layers to which the camera is connected
 * \param[out] tlVimbaSearchResult       The Vimba transport layers to which the camera is connected.
 * \param[out] tlVimbaSearchResultsCount  The number of Vimba transport layers to which the camera is connected
 *
 * \return Result of the operation
*/
VmbError_t GetRelatedTls(const VmbTransportLayerInfo_t* const tls, const VmbUint32_t tlCount, TlSearchResult** tlVimbaXSearchResult, VmbUint32_t* tlVimbaXSearchResultsCount, TlSearchResult** tlVimbaSearchResult, VmbUint32_t* tlVimbaSearchResultsCount);

/**
 * \brief Send a force ip command to a camera with the given mac address using an interface.
 * A check that the camera's mac is found by the interface is performed first.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] interfaceData   Information about the interface that should be used to send the command
 * \param[in] mac             The mac address of the camera
 * \param[in] ip              The desired ip address
 * \param[in] subnetMask      The desired subnet mask
 * \param[in] gateway         The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIpViaInterface(const InterfaceSearchResult* interfaceData, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Send a force ip command to a camera with the given mac address using a transport layer.
 * The camera's mac is first communicated to the transport layer.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] tlHandle    Handle of the interface that should be used to send the command
 * \param[in] mac        The mac address of the camera
 * \param[in] ip         The desired ip address
 * \param[in] subnetMask The desired subnet mask
 * \param[in] gateway    The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIpViaTl(const VmbHandle_t tlHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Send a force ip command to a camera once the camera's mac is known to the entity.
 *  The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] hande      Handle of the entity that should be used to send the command
 * \param[in] ip         The desired ip address
 * \param[in] subnetMask The desired subnet mask
 * \param[in] gateway    The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIp(const VmbHandle_t hande, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);



/*
 * ForceIp implementation
 */

VmbError_t ForceIp(const char* const mac, const char* const ip, const char* const subnet, const char* const gateway)
{
    /*
     * Convert the string parameters into the needed integer values in host byte order
     */
    const VmbInt64_t macValue         = MacAddr(mac);
    const VmbInt64_t ipValue          = (VmbInt64_t)IpAddressToHostByteOrderedInt(ip);
    const VmbInt64_t subnetMaskValue  = (VmbInt64_t)IpAddressToHostByteOrderedInt(subnet);
    const VmbInt64_t gatewayValue     = (gateway) ? (VmbInt64_t)IpAddressToHostByteOrderedInt(gateway) : 0;

    const VmbBool_t invalidConfiguration = ((macValue == 0) || (ipValue == INADDR_NONE) || (subnetMaskValue == INADDR_NONE) || (gatewayValue == INADDR_NONE));
    if (invalidConfiguration)
    {
        VMB_PRINT("One or more invalid parameters: %s %s %s %s.\n", mac, ip, subnet, gateway);
        return 1;
    }

    VmbError_t error = ForceIpViaInterface(macValue, ipValue, subnetMaskValue, gatewayValue);

    if(error != VmbErrorSuccess)
    {
        VMB_PRINT("ForceIp using interfaces failed, trying transport layers.\n\n");
        error = ForceIpViaTl(macValue, ipValue, subnetMaskValue, gatewayValue);
    }

    return error;
}

VmbError_t ForceIpViaInterface(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue)
{
    /*
     * Get a list of currently connected cameras.
     * Forces the used transport layers to update their internal camera lists.
     */
    {
        VmbCameraInfo_t* cameras = 0;
        VmbUint32_t cameraCount = 0;
        RETURN_ON_ERROR(ListCameras(&cameras, &cameraCount));
        free(cameras);
    }

    /*
     * Get the interfaces to which the camera is connected.
     */
    InterfaceSearchResult* interfaceSearchResults = 0;
    VmbUint32_t interfaceSearchResultsCount = 0;
    RETURN_AND_PRINT_ON_ERROR(SearchCamerasInterface(macValue, &interfaceSearchResults, &interfaceSearchResultsCount));

    for(VmbUint32_t interfaceSearchResultsIndex = 0; interfaceSearchResultsIndex < interfaceSearchResultsCount; interfaceSearchResultsIndex++)
    {
        VMB_PRINT("Found related device \"%s\" connected to interface \"%s\"\n", interfaceSearchResults[interfaceSearchResultsIndex].cameraId, interfaceSearchResults[interfaceSearchResultsIndex].info.interfaceName);
    }

    /*
     * Send a force ip command using the first of the previously found interfaces for which the operation succeeds.
     */
    VmbError_t error = VmbErrorSuccess;
    for (VmbUint32_t interfaceSearchResultsIndex = 0; interfaceSearchResultsIndex < interfaceSearchResultsCount; interfaceSearchResultsIndex++)
    {
        error = SendForceIpViaInterface(&(interfaceSearchResults[interfaceSearchResultsIndex]), macValue, ipValue, subnetMaskValue, gatewayValue);
        if (error == VmbErrorSuccess)
        {
            VMB_PRINT("Operation succeeded using interface \"%s\"\n", interfaceSearchResults[interfaceSearchResultsIndex].info.interfaceName);
            break;
        }
    }

    return error;
}

VmbError_t ForceIpViaTl(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue)
{
    /*
     * Get a list of all available transport layers.
     */
    VmbTransportLayerInfo_t* tls = 0;
    VmbUint32_t              tlCount = 0;
    RETURN_AND_PRINT_ON_ERROR(ListTransportLayers(&tls, &tlCount));

    /*
     * Get the Vimba X and Vimba transport layers.
     */
    TlSearchResult* tlVimbaXSearchResults = 0;
    VmbUint32_t tlVimbaXSearchResultsCount = 0;
    TlSearchResult* tlVimbaSearchResults = 0;
    VmbUint32_t tlVimbaSearchResultsCount = 0;
    RETURN_AND_PRINT_ON_ERROR(GetRelatedTls(tls, tlCount, &tlVimbaXSearchResults, &tlVimbaXSearchResultsCount, &tlVimbaSearchResults, &tlVimbaSearchResultsCount); free(tls));

    for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlVimbaXSearchResultsCount; tlSearchResultsIndex++)
    {
        VMB_PRINT("%lld interface(s) belonging to Vimba X transport layer \"%s\"\n", tlVimbaXSearchResults[tlSearchResultsIndex].interfaceCount, tlVimbaXSearchResults[tlSearchResultsIndex].info.transportLayerName);
    }

    for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlVimbaSearchResultsCount; tlSearchResultsIndex++)
    {
        VMB_PRINT("%lld interface(s) belonging to Vimba transport layer \"%s\"\n", tlVimbaSearchResults[tlSearchResultsIndex].interfaceCount, tlVimbaSearchResults[tlSearchResultsIndex].info.transportLayerName);
    }

    /*
     * Send a force ip command using the first of the previously found Vimba X transport layers for which the operation succeeds.
     */
    VmbError_t error = VmbErrorSuccess;
    for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlVimbaXSearchResultsCount; tlSearchResultsIndex++)
    {
        error = SendForceIpViaTl(tlVimbaXSearchResults[tlSearchResultsIndex].info.transportLayerHandle, macValue, ipValue, subnetMaskValue, gatewayValue);
        if (error == VmbErrorSuccess)
        {
            VMB_PRINT("Operation succeeded using transport layer \"%s\"\n", tlVimbaXSearchResults[tlSearchResultsIndex].info.transportLayerName);
            break;
        }
    }

    if (error != VmbErrorSuccess)
    {
        VmbError_t error;
        for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlVimbaSearchResultsCount; tlSearchResultsIndex++)
        {
            error = SendForceIpViaTl(tlVimbaSearchResults[tlSearchResultsIndex].info.transportLayerHandle, macValue, ipValue, subnetMaskValue, gatewayValue);
            if (error == VmbErrorSuccess)
            {
                VMB_PRINT("Operation succeeded using transport layer \"%s\"\n", tlVimbaSearchResults[tlSearchResultsIndex].info.transportLayerName);
                break;
            }
        }
    }

    return error;
}

/**
 * \brief Get the value of the string feature DeviceID
 *
 * \param[in]  interfaceHandle Handle of the interface the feature should be queried from
 * \param[out] deviceId        Target pointer which points to the allocated memory after the call. Must be freed later
 *
 * \return Error code reported during feature access
*/
VmbError_t QueryDeviceIdFeature(const VmbHandle_t interfaceHandle, char** deviceId)
{
    /*
     * Get the device ID reported by the interface
     */
    VmbUint32_t deviceIdLength = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureStringGet(interfaceHandle, "DeviceID", 0, 0, &deviceIdLength));

    *deviceId = VMB_MALLOC_ARRAY(char, deviceIdLength);

    const VmbError_t error = VmbFeatureStringGet(interfaceHandle, "DeviceID", *deviceId, deviceIdLength, &deviceIdLength);
    if (error != VmbErrorSuccess)
    {
        free(*deviceId);
        *deviceId = NULL;
    }

    return error;
}

VmbError_t SearchCamerasInterface(const VmbInt64_t mac, InterfaceSearchResult** interfaceSearchResults, VmbUint32_t* interfaceSearchResultsCount)
{
    /*
     * Get a list of all available interfaces.
     */
    VmbInterfaceInfo_t* interfaces = 0;
    VmbUint32_t         interfaceCount = 0;
    RETURN_AND_PRINT_ON_ERROR(ListInterfaces(&interfaces, &interfaceCount));

    /*
     * Count the interfaces to which the camera is connected.
     * Multiple transport layers can enumerate the same physical interface.
     */
    *interfaceSearchResultsCount = 0;
    for (VmbUint32_t interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++)
    {
        const VmbInterfaceInfo_t* const currentInterface = (interfaces + interfaceIndex);

        /*
         * Ignore interfaces whose interface technology is not based on the GigE Vision standard
         */
        const VmbBool_t ignoreInterface = (currentInterface->interfaceType != VmbTransportLayerTypeGEV);
        if (ignoreInterface)
        {
            continue;
        }

        const VmbHandle_t currentInterfaceHandle = currentInterface->interfaceHandle;

        /*
         * Get the number of cameras connected to the interface.
         * The feature DeviceSelector is not available if no camera is connected to the interface.
         * A reported range [0;0] means that only a single camera is connected to the interface.
         */
        VmbInt64_t deviceSelectorMin;
        VmbInt64_t deviceSelectorMax;
        CONTINUE_ON_ERROR(VmbFeatureIntRangeQuery(currentInterfaceHandle, "DeviceSelector", &deviceSelectorMin, &deviceSelectorMax));

        /*
         * Check based on the mac if the desired camera is connected to the current interface
         */
        for (VmbInt64_t deviceIndex = 0; deviceIndex <= deviceSelectorMax; deviceIndex++)
        {
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntSet(currentInterfaceHandle, "DeviceSelector", deviceIndex));

            VmbInt64_t currentMAC = 0;
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterfaceHandle, "GevDeviceMACAddress", &currentMAC));

            if (currentMAC == mac)
            {
                /*
                 * Camera is connected, add the interface.
                 */
                ++(*interfaceSearchResultsCount);
            }
        }
    }

    *interfaceSearchResults = VMB_MALLOC_ARRAY(InterfaceSearchResult, *interfaceSearchResultsCount);

    VmbError_t error = VmbErrorNotFound;

    /*
     * Store the interfaces to which the camera is connected.
     */
    VmbUint32_t interfaceSearchResultIndex = 0;
    for (VmbUint32_t interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++)
    {
        const VmbInterfaceInfo_t* const currentInterface = (interfaces + interfaceIndex);

        const VmbBool_t ignoreInterface = (currentInterface->interfaceType != VmbTransportLayerTypeGEV);
        if (ignoreInterface)
        {
            continue;
        }

        const VmbHandle_t currentInterfaceHandle = currentInterface->interfaceHandle;

        VMB_PRINT("Searching for camera on interface %s\n", currentInterface->interfaceName);

        VmbInt64_t deviceSelectorMin;
        VmbInt64_t deviceSelectorMax;
        CONTINUE_ON_ERROR(VmbFeatureIntRangeQuery(currentInterfaceHandle, "DeviceSelector", &deviceSelectorMin, &deviceSelectorMax));

        for (VmbInt64_t deviceIndex = 0; deviceIndex <= deviceSelectorMax; deviceIndex++)
        {
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntSet(currentInterfaceHandle, "DeviceSelector", deviceIndex));

            VmbInt64_t currentMAC = 0;
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterfaceHandle, "GevDeviceMACAddress", &currentMAC));

            if (currentMAC == mac)
            {
                char* cameraId = NULL;
                CONTINUE_ON_ERROR(QueryDeviceIdFeature(currentInterfaceHandle, &cameraId));

                (*interfaceSearchResults)[interfaceSearchResultIndex].cameraId = cameraId;
                (*interfaceSearchResults)[interfaceSearchResultIndex].info = *currentInterface;
                (*interfaceSearchResults)[interfaceSearchResultIndex].deviceSelector = deviceIndex;
                ++interfaceSearchResultIndex;
                error = VmbErrorSuccess;
                VMB_PRINT("Camera found on interface %s\n", currentInterface->interfaceName);
            }
        }
    }

    free(interfaces);

    return error;
}

VmbError_t GetRelatedTls( const VmbTransportLayerInfo_t* const tls, const VmbUint32_t tlCount, TlSearchResult** tlVimbaXSearchResult, VmbUint32_t* tlVimbaXSearchResultsCount, TlSearchResult** tlVimbaSearchResult, VmbUint32_t* tlVimbaSearchResultsCount)
{
    /*
     * The transport layer names used for Vimba X and Vimba transport layers
     */
    const char vimbaXTlName[] = "AVT GigE Transport Layer";
    const char vimbaTlName[] = "Vimba GigE Transport Layer";

    /*
     * Count the Vimba X and Vimba transport layers to which the camera is connected.
     * Multiple transport layers can enumerate the same physical interface.
     */
    *tlVimbaXSearchResultsCount = 0;
    *tlVimbaSearchResultsCount = 0;
    for (VmbUint32_t tlIndex = 0; tlIndex < tlCount; tlIndex++)
    {
        const VmbTransportLayerInfo_t* const currentTl = (tls + tlIndex);

        /*
         * Ignore interfaces whose transport layer is neither Vimba X nor Vimba
         */
        if ((strcmp(currentTl->transportLayerName, vimbaXTlName) != 0) && (strcmp(currentTl->transportLayerName, vimbaTlName) != 0))
        {
            continue;
        }

        /*
         * Add the transport layer if Vimba X or Vimba.
         */
        if (strcmp(currentTl->transportLayerName, vimbaXTlName) == 0)
        {
            ++(*tlVimbaXSearchResultsCount);
        }
        else if (strcmp(currentTl->transportLayerName, vimbaTlName) == 0)
        {
            ++(*tlVimbaSearchResultsCount);
        }
    }

    *tlVimbaXSearchResult = VMB_MALLOC_ARRAY(TlSearchResult, *tlVimbaXSearchResultsCount);

    *tlVimbaSearchResult = VMB_MALLOC_ARRAY(TlSearchResult, *tlVimbaSearchResultsCount);

    VmbError_t error = VmbErrorNotFound;

    /*
     * Store the Vimba X and Vimba GigE Vision transport layers to which the camera is connected.
     */
    VmbUint32_t tlVimbaXSearchResultsIndex = 0;
    VmbUint32_t tlVimbaSearchResultsIndex = 0;
    for (VmbUint32_t tlIndex = 0; tlIndex < tlCount; tlIndex++)
    {
        const VmbTransportLayerInfo_t* const currentTl = (tls + tlIndex);

        if (strcmp(currentTl->transportLayerName, vimbaXTlName) == 0)
        {
            VmbInt64_t interfaceCount;
            RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentTl->transportLayerHandle, "InterfaceCount", &interfaceCount));
            (*tlVimbaXSearchResult)[tlVimbaXSearchResultsIndex].interfaceCount = interfaceCount;
            (*tlVimbaXSearchResult)[tlVimbaXSearchResultsIndex].info = *currentTl;
            ++tlVimbaXSearchResultsIndex;
            error = VmbErrorSuccess;
        }
        else if (strcmp(currentTl->transportLayerName, vimbaTlName) == 0)
        {
            VmbInt64_t interfaceCount;
            RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentTl->transportLayerHandle, "InterfaceCount", &interfaceCount));
            (*tlVimbaSearchResult)[tlVimbaSearchResultsIndex].interfaceCount = interfaceCount;
            (*tlVimbaSearchResult)[tlVimbaSearchResultsIndex].info = *currentTl;
            ++tlVimbaSearchResultsIndex;
            error = VmbErrorSuccess;
        }
    }

    return error;
}

VmbError_t SendForceIpViaInterface(const InterfaceSearchResult* interfaceData, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    VmbHandle_t interfaceHandle = interfaceData->info.interfaceHandle;
    /*
     * The force ip features are related to the DeviceSelector.
     * Ensure that the stored selector value still selects the desired camera.
     */
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(interfaceHandle, "DeviceSelector", interfaceData->deviceSelector));

    VmbInt64_t selectedMAC = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntGet(interfaceHandle, "GevDeviceMACAddress", &selectedMAC));

    if (selectedMAC != mac)
    {
        return VmbErrorNotFound;
    }

    RETURN_AND_PRINT_ON_ERROR(SendForceIp(interfaceHandle, ip, subnetMask, gateway));

    return VmbErrorSuccess;
}

VmbError_t SendForceIpViaTl(const VmbHandle_t tlHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    /*
     * The force ip features are related to the DeviceSelector.
     * Set the current selector value to that of the desired camera.
     */
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(tlHandle, "GevDeviceForceMACAddress", mac));

    RETURN_AND_PRINT_ON_ERROR(SendForceIp(tlHandle, ip, subnetMask, gateway));

    return VmbErrorSuccess;
}

/**
 * \brief Suspend the execution of the current thread for 100 milliseconds
 */
void Sleep100Ms(void)
{
#ifdef WIN32
    Sleep(100);
#else
    struct timespec duration;
    duration.tv_sec  = 0;
    duration.tv_nsec = 100000000;
    nanosleep(&duration, NULL);
#endif
}

VmbError_t SendForceIp(const VmbHandle_t handle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    /*
     * Set the force ip features and send the force ip command
     */
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(handle, "GevDeviceForceIPAddress", ip));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(handle, "GevDeviceForceSubnetMask", subnetMask));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(handle, "GevDeviceForceGateway", gateway));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureCommandRun(handle, "GevDeviceForceIP"));

    VMB_PRINT("Force ip command sent. Waiting for completion...\n");

    /*
     * Wait for the force ip command to be completed
     */
    VmbBool_t  completedForceIp = VmbBoolFalse;
    VmbInt16_t retriesLeft      = 25;
    do
    {
        BREAK_AND_PRINT_ON_ERROR(VmbFeatureCommandIsDone(handle, "GevDeviceForceIP", &completedForceIp));
        if (!completedForceIp)
        {
            Sleep100Ms();
            retriesLeft -= 1;
        }
    } while ((!completedForceIp) && (retriesLeft > 0));

    const char* const commandStatus = (completedForceIp) ? "completed" : "not completed";
    VMB_PRINT("Force ip command %s.\n", commandStatus);

    return completedForceIp ? VmbErrorSuccess : VmbErrorRetriesExceeded;
}
