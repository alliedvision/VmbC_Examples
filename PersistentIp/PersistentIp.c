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
 * \brief Definition of the function which implements the persistent ip configuration
 */

#include "PersistentIpProg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
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



/**
 * \brief Converts a hexadecimal MAC address into its decimal representation.
 * \param[in] strMAC    The hexadecimal (with preceding 0x) or decimal MAC
 *
 * \return 0 in case of error otherwise the decimal representation of the MAC address on success as integer
*/
unsigned long long MacAddr(const char* strMAC)
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



/**
 * \brief Converts a hexadecimal MAC address into its decimal representation.
 * \param[in] strIp    The string representation of the IP
 *
 * \return The decimal representation of the IP address as integer in host byte order
*/
unsigned int IpAddr(const char* strIp)
{
    unsigned long ip = inet_addr(strIp);

    if (ip == INADDR_NONE)
    {
        return INADDR_NONE;
    }

#ifdef _LITTLE_ENDIAN
    ip = ntohl(ip);
#endif

    return ip;
}


/*
 * Macros used to controll the print out of the progress and upcoming errors.
 * Define _VMB_PERSISTENT_IP_NO_PRINT to disable the print out
 */
#ifdef _VMB_PERSISTENT_IP_NO_PRINT
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
 * \brief Helper struct used to store information about a found camera
 */
typedef struct CameraSearchResult
{
    VmbError_t      error;      //!< Error code representing the success of the operation
    VmbCameraInfo_t cameraInfo; //!< Information about the found camera
} CameraSearchResult;

/**
 * \brief Helper struct used to store information about a opened camera
 */
typedef struct CameraOpenResult
{
    VmbError_t      error;          //!< Error code representing the success of the operation
    VmbCameraInfo_t cameraInfo;     //!< Information about the opened camera
    VmbHandle_t     cameraHandle;   //!< Handle of the opened camera
} CameraOpenResult;

/*
 * Helper functions
 */

/**
 * \brief Search an interface a camera with the given mac is connected to
 *
 * \param mac   The cameras mac address
 *
 * \return Information about the found interface
*/
InterfaceSearchResult   GetRelatedInterface(const VmbInt64_t mac);

/**
 * \brief Send a force ip command to the camera with the given mac address
 *
 * \param interfaceHandle   Handle of the interface that should be used to send the command
 * \param mac               The mac address of the camera
 * \param ip                The desired ip address
 * \param subnetMask        The desired subnet mask
 * \param gateway           The desired gateway
 *
 * \return Result of the operation
*/
VmbError_t SendForceIp(VmbHandle_t interfaceHandle, VmbInt64_t mac, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway);

/**
 * \brief Get api specific information about the given camera which which is known to the given transport layer
 *
 * \param transportLayerHandl Handle of the transport layer the camera belongs to
 * \param tlCameraId          The camera id as reported by the given transport layer
 *
 * \return The collected information
*/
CameraSearchResult GetRelatedCamera(VmbHandle_t transportLayerHandl, const char* const tlCameraId);
/**
 * \brief Open a camera using the given camera id
 *
 * \param cameraId The unique camera id reported by the api
 *
 * \return Information collected during the opening
*/
CameraOpenResult OpenCamera(const char* const cameraId);

/**
 * \brief Writes the given ip configuration which will not be lost after a power-cycle
 *
 * \param cameraHandle  The handle of the related camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIp(VmbHandle_t cameraHandle, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway);

/**
 * \brief Writes the given ip configuration using standard features from the SFNC. The configuration will not be lost after a power-cycle
 *
 * \param cameraHandle  The handle of the related camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIpFeatures(VmbHandle_t cameraHandle, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway);

/**
 * \brief Writes the given ip configuration using standard camera registers. The configuration will not be lost after a power-cycle
 *
 * \param cameraHandle  The handle of the related camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIpRegisters(VmbHandle_t cameraHandle, VmbUint32_t ip, VmbUint32_t subnetMask, VmbUint32_t gateway);



/*
 * PersistenIp implementation
 */

VmbError_t SetPersistentIp(const char* const mac, const char* const ip, const char* const subnet, const char* const gateway)
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
     * Convert the give string parameters into the needed integer values in host byte order
     */
    VmbInt64_t macValue         = MacAddr(mac);
    VmbInt64_t ipValue          = (VmbInt64_t)IpAddr(ip);
    VmbInt64_t subnetMaskValue  = (VmbInt64_t)IpAddr(subnet);
    VmbInt64_t gatewayValue     = (gateway) ? (VmbInt64_t)IpAddr(gateway) : 0;

    VmbBool_t invalidConfiguration = ((macValue == 0) || (ipValue == INADDR_NONE) || (subnetMaskValue == INADDR_NONE) || (gatewayValue == INADDR_NONE));
    if (invalidConfiguration)
    {
        VMB_PRINT("One or more invalid parameters: %s %s %s %s\n.", mac, ip, subnet, gateway);
        return 1;
    }

    /*
     * Get an interface the camera is connected to.
     */
    InterfaceSearchResult foundInterface = GetRelatedInterface(macValue);
    RETURN_ON_ERROR(foundInterface.error);

    VMB_PRINT("Found related device \"%s\" connected to interface \"%s\"\n", foundInterface.tlCameraId, foundInterface.interfaceInfo.interfaceIdString);

    /*
     * Send a force ip command using the previously found interface.
     */
    RETURN_ON_ERROR(SendForceIp(foundInterface.interfaceInfo.interfaceHandle, macValue, ipValue, subnetMaskValue, gatewayValue));

    /*
     * Search the camera list to get the api wide unique camera id.
     */
    CameraSearchResult foundCamera = GetRelatedCamera(foundInterface.interfaceInfo.transportLayerHandle, foundInterface.tlCameraId);
    RETURN_ON_ERROR(foundCamera.error);

    VMB_PRINT("Related device \"%s\" is known to the api with unique id \"%s\"\n", foundInterface.tlCameraId, foundCamera.cameraInfo.cameraIdExtended);

    /*
     * Open the camera using the previous found unique camera id.
     */
    CameraOpenResult openedCamera = OpenCamera(foundCamera.cameraInfo.cameraIdExtended);
    RETURN_ON_ERROR(openedCamera.error);

    /*
     * Write the desired persistent ip configuration using either features from the SFNC or standard camera registers
     */
    VmbError_t configError = WritePersistentIp(openedCamera.cameraHandle, ipValue, subnetMaskValue, gatewayValue);

    (configError == VmbErrorSuccess) ? VMB_PRINT("Persistent IP configuration written to camera\n") : VMB_PRINT("Persistent IP configuration not written completly to camera\n");

    if (openedCamera.cameraHandle)
    {
        VmbCameraClose(openedCamera.cameraHandle);
    }

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
VmbError_t QueryDeviceIdFeature(VmbHandle_t interfaceHandle, char** deviceId)
{
    /*
     * Get the camera id reported by the transport layer
     */
    VmbUint32_t cameraIdLength = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureStringGet(interfaceHandle, "DeviceID", 0, 0, &cameraIdLength));

    *deviceId = VMB_MALLOC_ARRAY(char, cameraIdLength);

    VmbError_t error = VmbFeatureStringGet(interfaceHandle, "DeviceID", *deviceId, cameraIdLength, &cameraIdLength);
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
        VmbInterfaceInfo_t* currentInterface = (interfaces + interfaceIndex);

        /*
         * Ignore interfaces whoose interface technology is not based on the GigE Vision standard
         */
        VmbBool_t ignoreInterface = (currentInterface->interfaceType != VmbTransportLayerTypeGEV);
        if (ignoreInterface)
        {
            continue;
        }

        VmbHandle_t currentInterfaceHandle = currentInterface->interfaceHandle;

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

VmbError_t SendForceIp(VmbHandle_t interfaceHandle, VmbInt64_t mac, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway)
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

    const char* commandStatus = (completedForceIp) ? " completed." : " not completed. Next steps might fail.";
    VMB_PRINT("Force ip command %s\n", commandStatus);

    return VmbErrorSuccess;
}

CameraSearchResult GetRelatedCamera(VmbHandle_t transportLayerHandl, const char* const tlCameraId)
{
    CameraSearchResult result;
    memset(&result, 0, sizeof(result));
    result.error = VmbErrorUnknown;

    /*
     * Get a list of currently connected cameras.
     */
    VmbCameraInfo_t*    cameras = 0;
    VmbUint32_t         cameraCount = 0;
    RETURN_SPECIFIC_AND_PRINT_ON_ERROR(ListCameras(&cameras, &cameraCount), result);

    /*
     * Search the camera list for a camera with an id matching to the id reported by the transport layers interface
     */
    VmbBool_t cameraFound = VmbBoolFalse;
    for (VmbUint32_t currentCameraIndex = 0; (currentCameraIndex < cameraCount) && (!cameraFound); currentCameraIndex++)
    {
        VmbCameraInfo_t* currentCamera = cameras + currentCameraIndex;
        VmbBool_t connectedToSameTL = (transportLayerHandl == currentCamera->transportLayerHandle);
        VmbBool_t matchingCameraId = (strcmp(currentCamera->cameraIdString, tlCameraId) == 0);

        if (connectedToSameTL && matchingCameraId)
        {
            cameraFound = VmbBoolTrue;
            result.error = VmbErrorSuccess;
            result.cameraInfo = *(currentCamera);
        }
    }

    free(cameras);

    return result;
}

CameraOpenResult OpenCamera(const char* const cameraId)
{
    CameraOpenResult result;
    memset(&result, 0, sizeof(result));
    result.error = VmbErrorUnknown;

    /*
     * After the force ip command the transport layer might need some time to detect the changed ip configuration
     * and report the correct permitted access status
     */
    VmbBool_t cameraPermitsFullAccess = VmbBoolFalse;
    VmbInt16_t retriesLeft = 100;
    do
    {
        BREAK_AND_PRINT_ON_ERROR(VmbCameraInfoQuery(cameraId, &result.cameraInfo, sizeof(result.cameraInfo)));
        cameraPermitsFullAccess = (result.cameraInfo.permittedAccess & VmbAccessModeFull);
        if (!cameraPermitsFullAccess)
        {
            retriesLeft -= 1;
            Sleep100Ms();
            if ((retriesLeft % 5) == 0)
            {
                VMB_PRINT("Waiting for permitted write access. Now the camera \"%s\" only suports the access modes: %s\n", result.cameraInfo.cameraIdString, AccessModesToString(result.cameraInfo.permittedAccess));
            }
        }
    } while ((!cameraPermitsFullAccess) && (retriesLeft > 0));

    if (!cameraPermitsFullAccess)
    {
        result.error = VmbErrorInvalidAccess;
    }
    else
    {
        VMB_PRINT("Camera \"%s\" now supports the needed access modes: %s\n", result.cameraInfo.cameraIdString, AccessModesToString(result.cameraInfo.permittedAccess));
    }

    /*
     * Open the camera for later access
     */
    RETURN_SPECIFIC_AND_PRINT_ON_ERROR( (result.error = VmbCameraOpen(cameraId, VmbAccessModeFull, &result.cameraHandle)), result);

    VMB_PRINT("Opened camera \"%s\" with VmbAccessModeFull\n", result.cameraInfo.cameraIdString);

    result.error = VmbErrorSuccess;

    return result;
}

VmbError_t WritePersistentIp(VmbHandle_t cameraHandle, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway)
{
    /*
     * Check if the optional sfnc feature GevPersistentIPAddress is available and writeable.
     * It is assumed that the camera will either implement all the needed features for the persistent ip or none of them
     */
    VmbBool_t writeable = VmbBoolFalse;
    VmbError_t error = VmbFeatureAccessQuery(cameraHandle, "GevPersistentIPAddress", NULL, &writeable);
    VmbBool_t cameraHasPersistentFeatures = (error == VmbErrorSuccess) && (writeable);

    /*
     * Use the available sfnc features or camera registers to set the persistent ip configuration
     */
    if (cameraHasPersistentFeatures)
    {
        VMB_PRINT("Setting persistent ip using sfnc features\n");
        error = WritePersistentIpFeatures(cameraHandle, ip, subnetMask, gateway);
    }
    else
    {
        VMB_PRINT("Setting persistent ip using device registers\n");
        error = WritePersistentIpRegisters(cameraHandle, (VmbUint32_t)ip, (VmbUint32_t)subnetMask, (VmbUint32_t)gateway);
    }
    return error;
}

VmbError_t WritePersistentIpFeatures(VmbHandle_t cameraHandle, VmbInt64_t ip, VmbInt64_t subnetMask, VmbInt64_t gateway)
{
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentIPAddress", ip));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentSubnetMask", subnetMask));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentDefaultGateway", gateway));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureBoolSet(cameraHandle, "GevCurrentIPConfigurationPersistentIP", VmbBoolTrue));

    return VmbErrorSuccess;
}

VmbError_t WritePersistentIpRegisters(VmbHandle_t cameraHandle, VmbUint32_t ip, VmbUint32_t subnetMask, VmbUint32_t gateway)
{
    const VmbUint64_t GIGE_INTERFACE_CONFIG_REGISTER        = 0x0014;
    const VmbUint64_t GIGE_PERSISTENT_IP_REGISTER           = 0x064C;
    const VmbUint64_t GIGE_PERSISTENT_SUBNET_MASK_REGISTER  = 0x065C;
    const VmbUint64_t GIGE_PERSISTENT_GATEWAY_REGISTER      = 0x066C;

    /*
     * Read the Network Interface Configuration register
     */
    VmbUint32_t infterfaceConfiguration = 0;
    VmbUint32_t bytesRead = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbMemoryRead(cameraHandle, GIGE_INTERFACE_CONFIG_REGISTER, sizeof(infterfaceConfiguration), (char*)(&infterfaceConfiguration), &bytesRead));

    #ifdef _LITTLE_ENDIAN
        infterfaceConfiguration = ntohl(infterfaceConfiguration);
    #endif // _LITTLE_ENDIAN

    /*
     * Set the persisten ip configuration bit
     */
    infterfaceConfiguration = infterfaceConfiguration | 0x1;


    /*
     * Write the configuration to the camera registers
     */
    VmbUint64_t addresses[] = { GIGE_PERSISTENT_IP_REGISTER, GIGE_PERSISTENT_SUBNET_MASK_REGISTER, GIGE_PERSISTENT_GATEWAY_REGISTER, GIGE_INTERFACE_CONFIG_REGISTER };
    VmbUint32_t datas[]     = { ip , subnetMask , gateway, infterfaceConfiguration };
    const VmbUint32_t registersToWrite = sizeof(datas) / sizeof(VmbUint32_t);
    for (VmbUint32_t currentIndex = 0; currentIndex < registersToWrite; currentIndex++)
    {
        #ifdef _LITTLE_ENDIAN
            datas[currentIndex] = htonl(datas[currentIndex]);
        #endif

        const char* currentData = (const char*)(&(datas[currentIndex]));
        const VmbUint64_t currentAddress = addresses[currentIndex];

        VmbUint32_t written = 0;
        VmbUint32_t bufferSize = sizeof(VmbUint32_t);
        RETURN_AND_PRINT_ON_ERROR(VmbMemoryWrite(cameraHandle, currentAddress, bufferSize, currentData, &written));
    }

    return VmbErrorSuccess;
}
