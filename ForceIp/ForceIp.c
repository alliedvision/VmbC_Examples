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
 * \brief Send a force ip command to a camera with the given mac address. The new configuration will be lost after a power-cycle of the camera.
 *
 * \param interfaceHandle   Handle of the interface that should be used to send the command
 * \param mac               The mac address of the camera
 * \param ip                The desired ip address
 * \param subnetMask        The desired subnet mask
 * \param gateway           The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIp(const VmbHandle_t interfaceHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/*
 * Helper functions
 */

/**
 * \brief Search for the interface to which a camera with the given mac address is connected
 *
 * \param mac   The camera's mac address
 *
 * \return Information about the found interface
*/
InterfaceSearchResult   GetRelatedInterface(const VmbInt64_t mac);



/*
 * ForceIp implementation
 */

VmbError_t ForceIp(const char* const mac, const char* const ip, const char* const subnet, const char* const gateway)
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

    /*
     * Get an interface the camera is connected to.
     */
    const InterfaceSearchResult foundInterface = GetRelatedInterface(macValue);
    RETURN_ON_ERROR(foundInterface.error);

    VMB_PRINT("Found related device \"%s\" connected to interface \"%s\"\n", foundInterface.tlCameraId, foundInterface.interfaceInfo.interfaceIdString);

    /*
     * Send a force ip command using the previously found interface.
     */
    RETURN_ON_ERROR(SendForceIp(foundInterface.interfaceInfo.interfaceHandle, macValue, ipValue, subnetMaskValue, gatewayValue));

    return VmbErrorSuccess;
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

InterfaceSearchResult GetRelatedInterface(const VmbInt64_t mac)
{
    InterfaceSearchResult result;
    memset(&result, 0, sizeof(result));
    result.error = VmbErrorNotFound;

    /*
     * Get a list of all available interfaces.
     */
    VmbInterfaceInfo_t* interfaces = 0;
    VmbUint32_t         interfaceCount = 0;
    RETURN_SPECIFIC_AND_PRINT_ON_ERROR(ListInterfaces(&interfaces, &interfaceCount), result);

    /*
     * Find the first GigE Vision interface the camera is connected to.
     * Multiple transport layers can enumerate the same physical interface.
     */
    VmbBool_t    cameraFound = VmbBoolFalse;
    VmbHandle_t* camerasTransportLayerHandle = 0;
    char*        cameraId = 0;

    for (VmbUint32_t interfaceIndex = 0; (interfaceIndex < interfaceCount) && (!cameraFound); interfaceIndex++)
    {
        const VmbInterfaceInfo_t* const currentInterface = (interfaces + interfaceIndex);

        /*
         * Ignore interfaces whoose interface technology is not based on the GigE Vision standard
         */
        const VmbBool_t ignoreInterface = (currentInterface->interfaceType != VmbTransportLayerTypeGEV);
        if (ignoreInterface)
        {
            continue;
        }

        const VmbHandle_t currentInterfaceHandle = currentInterface->interfaceHandle;

        /*
         * Get the number of cameras connected to the interface
         */
        VmbInt64_t deviceCount = 0;
        CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterface->interfaceHandle, "DeviceCount", &deviceCount));

        /*
         * Check based on the mac if the desired camera is connected to the current interface
         */
        for (VmbInt64_t deviceIndex = 0; (deviceIndex < deviceCount) && (!cameraFound); deviceIndex++)
        {
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntSet(currentInterfaceHandle, "DeviceSelector", deviceIndex));

            VmbInt64_t currentMAC = 0;
            CONTINUE_AND_PRINT_ON_ERROR(VmbFeatureIntGet(currentInterfaceHandle, "GevDeviceMACAddress", &currentMAC));

            if (currentMAC == mac)
            {
                CONTINUE_ON_ERROR(QueryDeviceIdFeature(currentInterfaceHandle, &cameraId));

                result.tlCameraId = cameraId;
                result.error = VmbErrorSuccess;
                result.interfaceInfo = *currentInterface;
                cameraFound = VmbBoolTrue;
            }
        }
    }

    free(interfaces);

    if (!cameraFound)
    {
        VMB_PRINT("No camera with matching mac address found.\n");
    }

    return result;
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

VmbError_t SendForceIp(const VmbHandle_t interfaceHandle, const VmbInt64_t mac, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    /*
     * The force ip features are related to the DeviceSelector.
     * Ensure that the current selector value selects the desired camera
     */
    VmbInt64_t selectedMAC = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntGet(interfaceHandle, "GevDeviceMACAddress", &selectedMAC));

    if (selectedMAC != mac)
    {
        return VmbErrorNotFound;
    }

    /*
     * Set the force ip features and send the force ip command
     */
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(interfaceHandle, "GevDeviceForceIPAddress", ip));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(interfaceHandle, "GevDeviceForceSubnetMask", subnetMask));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(interfaceHandle, "GevDeviceForceGateway", gateway));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureCommandRun(interfaceHandle, "GevDeviceForceIP"));

    VMB_PRINT("Force ip command sent. Waiting for completion...\n");

    /*
     * Wait for the force ip command to be completed
     */
    VmbBool_t  completedForceIp = VmbBoolFalse;
    VmbInt16_t retriesLeft      = 25;
    do
    {
        BREAK_AND_PRINT_ON_ERROR(VmbFeatureCommandIsDone(interfaceHandle, "GevDeviceForceIP", &completedForceIp));
        if (!completedForceIp)
        {
            Sleep100Ms();
            retriesLeft -= 1;
        }
    } while ((!completedForceIp) && (retriesLeft > 0));

    const char* const commandStatus = (completedForceIp) ? " completed." : " not completed.";
    VMB_PRINT("Force ip command %s\n", commandStatus);

    return VmbErrorSuccess;
}
