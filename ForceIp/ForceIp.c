/**
 * \date 2022
 * \copyright Allied Vision Technologies.  All Rights Reserved.
 *
 * \copyright Redistribution of this file, in original or modified form, without
 *            prior written consent of Allied Vision Technologies is prohibited.
 *
 * \warning THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 * NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#define _ON_ERROR_WITH_PRINT(op, onError) _ON_ERROR(op, VMB_PRINT("%s failed. %s Error code: %d.\n", #op, ErrorCodeToMessage(error), error); onError)

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
    VmbError_t          error;          //!< Error code representing the success of the operation
    VmbInterfaceInfo_t  interfaceInfo;  //!< Information about the found interface
    char*               tlCameraId;     //!< Camera id as reported by the interface
} InterfaceSearchResult;

/**
 * \brief Helper struct used to store information about a found transport layer
 */
typedef struct TlSearchResult
{
    VmbError_t               error;       //!< Error code representing the success of the operation
    VmbTransportLayerInfo_t  tlInfo;      //!< Information about the found transport layer
} TlSearchResult;


/**
 * \brief The names of the transport layers that will be used
 */
const char vimbaXTlName[] = "AVT GigE Transport Layer";
const char vimbaTlName[] = "Vimba GigE Transport Layer";

/*
 * Helper functions
 */

/**
 * \brief Send a force ip command to a camera with the given mac address via an interface that the camera is connected to.
 * The first found interface of a Vimba X transport layer will be used if available,
 * otherwise the first found interface of a Vimba transport layer if available,
 * otherwise an error is returned.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] interfaceHandle Handle of the interface that should be used to send the command
 * \param[in] mac             The mac address of the camera
 * \param[in] ip              The desired ip address
 * \param[in] subnetMask      The desired subnet mask
 * \param[in] gateway         The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t ForceIpViaInterface(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue);

 /**
  * \brief Send a force ip command to a camera with the given mac address via a transport layer.
  * The first found Vimba X transport layer will be used if available,
  * otherwise the first found Vimba transport layer if available,
  * otherwise an error is returned.
  * The new configuration will be lost after a power-cycle of the camera.
  *
  * \param[in] interfaceHandle Handle of the interface that should be used to send the command
  * \param[in] mac             The mac address of the camera
  * \param[in] ip              The desired ip address
  * \param[in] subnetMask      The desired subnet mask
  * \param[in] gateway         The desired gateway
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
VmbError_t GetRelatedInterfaces(const VmbInt64_t mac, InterfaceSearchResult** interfaceSearchResults, VmbUint32_t* interfaceSearchResultsCount);

/**
 * \brief Search for interfaces to which a camera with the given mac address is connected
 *
 * \param[in] mac                   The camera's mac address
 * \param[out] tlSearchResults      The transport layers to which the camera is connected.
 *                                  The first one will be a Vimba X transport layer, if available,
 *                                  and the second one a Vimba one, if available
 * \param[out] tlSearchResultsCount The number of transport layers to which the camera is connected
 *
 * \return Result of the operation
*/
VmbError_t GetRelatedTls(const VmbInt64_t mac, TlSearchResult** tlSearchResult, VmbUint32_t* tlSearchResultsCount);

/**
 * \brief Send a force ip command to a camera with the given mac address using an interface.
 * A check that the camera is connected to the interface using the camera's mac address is performed first.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] interfaceHandle Handle of the interface that should be used to send the command
 * \param[in] mac             The mac address of the camera
 * \param[in] ip              The desired ip address
 * \param[in] subnetMask      The desired subnet mask
 * \param[in] gateway         The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIpViaInterface(const VmbHandle_t interfaceHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Send a force ip command to a camera with the given mac address using a transport layers.
 * The camera is first connected to the transport layer using the camera's mac address.
 * The new configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] tlHande    Handle of the interface that should be used to send the command
 * \param[in] mac        The mac address of the camera
 * \param[in] ip         The desired ip address
 * \param[in] subnetMask The desired subnet mask
 * \param[in] gateway    The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIpViaTl(const VmbHandle_t tlHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Send a force ip command to a camera once the camera is known to be connected to the entity.
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

VmbError_t ForceIp(const VmbBool_t useTl, const char* const mac, const char* const ip, const char* const subnet, const char* const gateway)
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
        VMB_PRINT("One or more invalid parameters: %s %s %s %s\n.", mac, ip, subnet, gateway);
        return 1;
    }

    if (useTl)
    {
        return ForceIpViaTl(macValue, ipValue, subnetMaskValue, gatewayValue);
    }
    else
    {
        return ForceIpViaInterface(macValue, ipValue, subnetMaskValue, gatewayValue);
    }
}

VmbError_t ForceIpViaInterface(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue)
{
    /*
     * Get a list of currently connected cameras.
     * Forces the used transport layers to update their internal camera lists.
     */
    {
        VmbCameraInfo_t* cameras = 0;
        VmbUint32_t         cameraCount = 0;
        RETURN_ON_ERROR(ListCameras(&cameras, &cameraCount));
        free(cameras);
    }

    /*
     * Get the interfaces the camera is connected to.
     */
    InterfaceSearchResult* interfaceSearchResults = 0;
    VmbUint32_t interfaceSearchResultsCount = 0;
    RETURN_AND_PRINT_ON_ERROR(GetRelatedInterfaces(macValue, &interfaceSearchResults, &interfaceSearchResultsCount));

    for(VmbUint32_t interfaceSearchResultsIndex = 0; interfaceSearchResultsIndex < interfaceSearchResultsCount; interfaceSearchResultsIndex++)
    {
        if (interfaceSearchResults[interfaceSearchResultsIndex].error == VmbErrorSuccess)
        {
            VMB_PRINT("Found related device \"%s\" connected to interface \"%s\"\n", interfaceSearchResults[interfaceSearchResultsIndex].tlCameraId, interfaceSearchResults[interfaceSearchResultsIndex].interfaceInfo.interfaceName);
        }
    }

    /*
     * Send a force ip command using the previously found interfaces.
     */
    VmbError_t error = VmbErrorSuccess;
    for (VmbUint32_t interfaceSearchResultsIndex = 0; interfaceSearchResultsIndex < interfaceSearchResultsCount; interfaceSearchResultsIndex++)
    {
        if (interfaceSearchResults[interfaceSearchResultsIndex].error == VmbErrorSuccess)
        {
            error = SendForceIpViaInterface(interfaceSearchResults[interfaceSearchResultsIndex].interfaceInfo.interfaceHandle, macValue, ipValue, subnetMaskValue, gatewayValue);
            if (error == VmbErrorSuccess)
            {
                VMB_PRINT("Operation succeeded using interface \"%s\"\n", interfaceSearchResults[interfaceSearchResultsIndex].interfaceInfo.interfaceName);
                break;
            }
        }
    }

    return error;
}

VmbError_t ForceIpViaTl(const VmbInt64_t macValue, const VmbInt64_t ipValue, const VmbInt64_t subnetMaskValue, const VmbInt64_t gatewayValue)
{
    /*
     * Get the interfaces the camera is connected to.
     */
    TlSearchResult* tlSearchResults = 0;
    VmbUint32_t tlSearchResultsCount = 0;
    RETURN_AND_PRINT_ON_ERROR(GetRelatedTls(macValue, &tlSearchResults, &tlSearchResultsCount));

    for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlSearchResultsCount; tlSearchResultsIndex++)
    {
        VMB_PRINT("Found transport layer \"%s\"\n", tlSearchResults[tlSearchResultsIndex].tlInfo.transportLayerName);
    }

    /*
     * Send a force ip command using the previously found interfaces.
     */
    VmbError_t error;
    for (VmbUint32_t tlSearchResultsIndex = 0; tlSearchResultsIndex < tlSearchResultsCount; tlSearchResultsIndex++)
    {
        error = SendForceIpViaTl(tlSearchResults[tlSearchResultsIndex].tlInfo.transportLayerHandle, macValue, ipValue, subnetMaskValue, gatewayValue);
        if (error == VmbErrorSuccess)
        {
            VMB_PRINT("Operation succeeded using transport layer \"%s\"\n", tlSearchResults[tlSearchResultsIndex].tlInfo.transportLayerName);
            break;
        }
    }

    return error;
}

/**
 * \brief Get the value of the string feature DeviceID
 *
 * \param[in]   interfaceHandle Handle of the interface the feature should be queried from
 * \param[out]  deviceId        Target pointer which points to the allocated memory after the call. Must be freed later
 *
 * \return Error code reported during feature access
*/
VmbError_t QueryDeviceIdFeature(const VmbHandle_t interfaceHandle, char** deviceId)
{
    /*
     * Get the camera id reported by the transport layer
     */
    VmbUint32_t cameraIdLength = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureStringGet(interfaceHandle, "DeviceID", 0, 0, &cameraIdLength));

    *deviceId = VMB_MALLOC_ARRAY(char, cameraIdLength);

    const VmbError_t error = VmbFeatureStringGet(interfaceHandle, "DeviceID", *deviceId, cameraIdLength, &cameraIdLength);
    if (error != VmbErrorSuccess)
    {
        free(*deviceId);
        *deviceId = NULL;
    }

    return error;
}

VmbError_t GetRelatedInterfaces(const VmbInt64_t mac, InterfaceSearchResult** interfaceSearchResults, VmbUint32_t* interfaceSearchResultsCount)
{
    /*
     * Get a list of all available interfaces.
     */
    VmbInterfaceInfo_t* interfaces = 0;
    VmbUint32_t         interfaceCount = 0;
    RETURN_AND_PRINT_ON_ERROR(ListInterfaces(&interfaces, &interfaceCount));

    /*
     * Get a list of all available transport layers.
     * Used to get the names of the transport layers to which each interface belongs
     */
    VmbTransportLayerInfo_t* tls = 0;
    VmbUint32_t              tlCount = 0;
    RETURN_AND_PRINT_ON_ERROR(ListTransportLayers(&tls, &tlCount));

    /*
     * Only 2 results are obtained: the first for the interface of a Vimba X transport layer as default,
     * the second for that of a Vimba one as fallback.
     */
    *interfaceSearchResultsCount = 2;
    *interfaceSearchResults = VMB_MALLOC_ARRAY(InterfaceSearchResult, *interfaceSearchResultsCount);

    /*
     * Initialize the results
     */
    for (VmbUint32_t interfaceSearchResultIndex = 0; interfaceSearchResultIndex < *interfaceSearchResultsCount; interfaceSearchResultIndex++)
    {
        (*interfaceSearchResults)[interfaceSearchResultIndex].error = VmbErrorNotFound;
    }

    VmbError_t error = VmbErrorNotFound;

    /*
     * Find the first Vimba X and Vimba GigE Vision interfaces that the camera is connected to.
     * Multiple transport layers can enumerate the same physical interface.
     */
    for (VmbUint32_t interfaceIndex = 0; interfaceIndex < interfaceCount; interfaceIndex++)
    {
        const VmbInterfaceInfo_t* const currentInterface = (interfaces + interfaceIndex);

        /*
         * Get the name of the transport layer to which this interface belongs
         */
        const char* tlName;
        for (VmbUint32_t tlIndex = 0; tlIndex < tlCount; tlIndex++)
        {
            if (tls[tlIndex].transportLayerHandle == currentInterface->transportLayerHandle)
            {
                tlName = tls[tlIndex].transportLayerName;
            }
        }

        /*
         * Ignore interfaces whose transport layer is not Vimba (X)
         */
        if (strcmp(tlName, vimbaXTlName) != 0 && strcmp(tlName, vimbaTlName) != 0)
        {
            continue;
        }

        const VmbHandle_t currentInterfaceHandle = currentInterface->interfaceHandle;

        /*
         * Get the number of cameras connected to the interface
         */
        VmbInt64_t deviceCount = 0;
        CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterfaceHandle, "DeviceCount", &deviceCount));

        /*
         * Check based on the mac if the desired camera is connected to the current interface
         */
        for (VmbInt64_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
        {
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntSet(currentInterfaceHandle, "DeviceSelector", deviceIndex));

            VmbInt64_t currentMAC = 0;
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterfaceHandle, "GevDeviceMACAddress", &currentMAC));

            if (currentMAC == mac)
            {
                char* cameraId = NULL;
                CONTINUE_ON_ERROR(QueryDeviceIdFeature(currentInterfaceHandle, &cameraId));

                /*
                 * Camera is connected, add the interface.
                 * In case another transport layer with the same name was already found, only the first one found will be used.
                 */
                VmbUint32_t interfaceSearchResultIndex = (strcmp(tlName, vimbaXTlName) == 0) ? 0 : 1;
                if ((*interfaceSearchResults)[interfaceSearchResultIndex].error == VmbErrorNotFound)
                {
                    (*interfaceSearchResults)[interfaceSearchResultIndex].tlCameraId = cameraId;
                    (*interfaceSearchResults)[interfaceSearchResultIndex].error = VmbErrorSuccess;
                    (*interfaceSearchResults)[interfaceSearchResultIndex].interfaceInfo = *currentInterface;
                    error = VmbErrorSuccess;
                }
            }
        }
    }

    free(interfaces);

    return error;
}

VmbError_t GetRelatedTls(const VmbInt64_t mac, TlSearchResult** tlSearchResult, VmbUint32_t* tlSearchResultsCount)
{
    /*
     * Get a list of all available transport layers.
     */
    VmbTransportLayerInfo_t* tls = 0;
    VmbUint32_t              tlCount = 0;
    RETURN_AND_PRINT_ON_ERROR(ListTransportLayers(&tls, &tlCount));

    /*
     * Only 2 results are obtained: the first for a Vimba X transport layer as default,
     * the second for a Vimba one as fallback.
     */
    *tlSearchResultsCount = 2;
    *tlSearchResult = VMB_MALLOC_ARRAY(TlSearchResult, *tlSearchResultsCount);

    /*
     * Initialize the results
     */
    for (VmbUint32_t tlSearchResultIndex = 0; tlSearchResultIndex < *tlSearchResultsCount; tlSearchResultIndex++)
    {
        (*tlSearchResult)[tlSearchResultIndex].error = VmbErrorNotFound;
    }

    VmbError_t error = VmbErrorNotFound;

    /*
     * Find the first Vimba X and Vimba GigE Vision transport layers that the camera is connected to.
     * Multiple transport layers can enumerate the same physical interface.
     */
    for (VmbUint32_t tlIndex = 0; tlIndex < tlCount; tlIndex++)
    {
        const VmbTransportLayerInfo_t* const currentTl = (tls + tlIndex);

        /*
         * Ignore interfaces whose transport layer is not Vimba (X)
         */
        if (strcmp(currentTl->transportLayerName, vimbaXTlName) != 0 && strcmp(currentTl->transportLayerName, vimbaTlName) != 0)
        {
            continue;
        }

        /*
         * Camera is connected, add the transport Layer.
         * In case another transport layer with the same name was already found, only the first one found will be used.
         */
        VmbUint32_t tlSearchResultIndex = (strcmp(currentTl->transportLayerName, vimbaXTlName) == 0) ? 0 : 1;
        if ((*tlSearchResult)[tlSearchResultIndex].error == VmbErrorNotFound)
        {
            (*tlSearchResult)[tlSearchResultIndex].error = VmbErrorSuccess;
            (*tlSearchResult)[tlSearchResultIndex].tlInfo = *currentTl;
            error = VmbErrorSuccess;
        }
    }

    free(tls);

    return error;
}

VmbError_t SendForceIpViaInterface(const VmbHandle_t interfaceHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    /*
     * The force ip features are related to the DeviceSelector.
     * Ensure that the current selector value selects the desired camera.
     */
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
void Sleep100Ms()
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
