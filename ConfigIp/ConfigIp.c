/**
 * \date 2022-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of the function which implements the IP configuration
 */

#define _CRT_SECURE_NO_WARNINGS // disable sscanf warning for Windows

#include "ConfigIpProg.h"

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
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>
#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/IpAddressToHostByteOrderedInt.h>



/*
 * Macros used to control the print out of the progress and upcoming errors.
 * Define _VMB_CONFIG_IP_NO_PRINT to disable the print out
 */
#ifdef _VMB_CONFIG_IP_NO_PRINT
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
 * \brief Helper struct used to store information about an opened camera
 */
typedef struct CameraOpenResult
{
    VmbError_t      error;          //!< Error code representing the success of the operation
    VmbHandle_t     cameraHandle;   //!< Handle of the opened camera
} CameraOpenResult;

/*
 * Helper functions
 */

 /**
  * \brief Writes the desired persistent IP configuration. The new configuration will be retained and applied after a power-cycle of the camera.
 *         Assumes that the VmbC API is already started.
  *
  * \param[in] cameraHandle The handle of the desired camera
  * \param[in] ip           The desired IP address
  * \param[in] subnet       The desired subnet mask
  * \param[in] gateway      The desired gateway
  *
  * \return Result of the operation
 */
VmbError_t SetPersistentIp(const VmbHandle_t cameraHandle, const char* const ip, const char* const subnet, const char* const gateway);

/**
 * \brief Writes the DHCP IP configuration. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraHandle The handle of the desired camera
  *
  * \return Result of the operation
 */
VmbError_t SetDhcp(const VmbHandle_t cameraHandle);

/**
 * \brief Writes the LLA IP configuration. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraHandle The handle of the desired camera
  *
  * \return Result of the operation
 */
VmbError_t SetLla(const VmbHandle_t cameraHandle);

/**
 * \brief Open a camera using the given camera id
 *
 * \param[in] cameraId The desired camera id
 *
 * \return Information collected during opening
*/
CameraOpenResult OpenCamera(const char* const cameraId);

/**
 * \brief Writes the desired IP configuration. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraHandle The handle of the desired camera
 * \param[in] ip           The desired IP address in host byte order
 * \param[in] subnetMask   The desired subnet mask in host byte order
 * \param[in] gateway      The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t WritePersistentIp(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Writes the desired IP configuration using standard features from the SFNC. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraHandle The handle of the desired camera
 * \param[in] ip           The desired IP address in host byte order
 * \param[in] subnetMask   The desired subnet mask in host byte order
 * \param[in] gateway      The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t WritePersistentIpFeatures(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Writes the desired IP configuration using standard camera registers. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraHandle The handle of the desired camera
 * \param[in] ip           The desired IP address in host byte order
 * \param[in] subnetMask   The desired subnet mask in host byte order
 * \param[in] gateway      The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t WritePersistentIpRegisters(const VmbHandle_t cameraHandle, const VmbUint32_t ip, const VmbUint32_t subnetMask, const VmbUint32_t gateway);

/**
 * \brief Writes data to a standard camera register
 *
 * \param[in] cameraHandle The handle of the camera whose register shall be written to
 * \param[in] address      The address of the register to write to
 * \param[in] data         The data to write to the register
 *
 * \return Result of the operation
*/
VmbError_t WriteRegister(const VmbHandle_t cameraHandle, const VmbUint64_t address, VmbUint32_t data);

/**
 * \brief Write an interface configuration to the interface configuration register
 *
 * \param[in] cameraHandle                    The handle of the camera whose interface configuration register shall be written to
 * \param[in] SetInterfaceConfigRegisterValue A pointer to a function which is given write access to a host-byte-ordered copy of the current configuration data as a parameter,
 *                                            i.e. this function's implementation defines the new configuration to be written
 *
 * \return Result of the operation
*/
VmbError_t WriteInterfaceConfigRegister(const VmbHandle_t cameraHandle, void (*SetInterfaceConfigRegisterValue) (VmbUint32_t* const));

/**
 * \brief Modifies an interface configuration to enable persistent IP, for passing to WriteInterfaceConfigRegister()
 *
 * \param[in,out] interfaceConfiguration A pointer to the configuration data in host-byte-order
 */
void SetInterfaceConfigRegisterValue_PersistentIpConfig(VmbUint32_t* const interfaceConfiguration);

/**
 * \brief Modifies an interface configuration to enable DHCP, for passing to WriteInterfaceConfigRegister()
 *
 * \param[in,out] interfaceConfiguration A pointer to the configuration data in host-byte-order
*/
void SetInterfaceConfigRegisterValue_DhcpIpConfig(VmbUint32_t* const interfaceConfiguration);

/**
 * \brief Modifies an interface configuration to enable LLA, for passing to WriteInterfaceConfigRegister()
 *
 * \param[in,out] interfaceConfiguration A pointer to the configuration data in host-byte-order
*/
void SetInterfaceConfigRegisterValue_LlaIpConfig(VmbUint32_t* const interfaceConfiguration);


/*
 * ConfigIp implementation
 */

VmbError_t ConfigIp(const char* const cameraId, const char* const ip, const char* const subnet, const char* const gateway)
{
    /*
     * Open the camera using the desired camera id.
     */
    const CameraOpenResult openedCamera = OpenCamera(cameraId);
    RETURN_ON_ERROR(openedCamera.error);

    VmbError_t configIpError = VmbErrorUnknown;

    if (openedCamera.cameraHandle)
    {
        if (ip != NULL && subnet != NULL)
        {
            configIpError = SetPersistentIp(openedCamera.cameraHandle, ip, subnet, gateway);
        }
        else if (ip != NULL)
        {
            configIpError = SetDhcp(openedCamera.cameraHandle);
        }
        else
        {
            configIpError = SetLla(openedCamera.cameraHandle);
        }

        VmbCameraClose(openedCamera.cameraHandle);
    }

    return configIpError;
}

VmbError_t SetPersistentIp(const VmbHandle_t cameraHandle, const char* const ip, const char* const subnet, const char* const gateway)
{
    /*
     * Convert the string parameters into the needed integer values in host byte order
     */
    const VmbInt64_t ipValue          = (VmbInt64_t)IpAddressToHostByteOrderedInt(ip);
    const VmbInt64_t subnetMaskValue  = (VmbInt64_t)IpAddressToHostByteOrderedInt(subnet);
    const VmbInt64_t gatewayValue     = (gateway) ? (VmbInt64_t)IpAddressToHostByteOrderedInt(gateway) : 0;

    const VmbBool_t invalidConfiguration = ((ipValue == INADDR_NONE) || (subnetMaskValue == INADDR_NONE) || (gatewayValue == INADDR_NONE));
    if (invalidConfiguration)
    {
        VMB_PRINT("One or more invalid parameters: %s %s %s\n.", ip, subnet, gateway);
        return 1;
    }

    /*
     * Write the desired persistent IP configuration using either features from the SFNC or standard camera registers
     */
    const VmbError_t configError = WritePersistentIp(cameraHandle, ipValue, subnetMaskValue, gatewayValue);

    (configError == VmbErrorSuccess) ? VMB_PRINT("IP configuration written to camera\n") : VMB_PRINT("IP configuration not written completely to camera\n");

    return VmbErrorSuccess;
}

VmbError_t SetDhcp(const VmbHandle_t cameraHandle)
{
    /*
     * Check if the optional sfnc feature GevCurrentIPConfigurationDHCP is available and writeable.
     */
    VmbBool_t writeable = VmbBoolFalse;
    const VmbError_t error = VmbFeatureAccessQuery(cameraHandle, "GevCurrentIPConfigurationDHCP", NULL, &writeable);
    const VmbBool_t cameraHasPersistentFeatures = (error == VmbErrorSuccess) && (writeable);

    /*
     * Use the available sfnc features or camera registers to set the DHCP IP configuration
     */
    if (cameraHasPersistentFeatures)
    {
        VMB_PRINT("Setting DHCP using sfnc features\n");
        RETURN_AND_PRINT_ON_ERROR(VmbFeatureBoolSet(cameraHandle, "GevCurrentIPConfigurationDHCP", VmbBoolTrue));
    }
    else
    {
        VMB_PRINT("Setting DHCP using device registers\n");
        RETURN_ON_ERROR(WriteInterfaceConfigRegister(cameraHandle, SetInterfaceConfigRegisterValue_DhcpIpConfig));
    }

    return error;
}

VmbError_t SetLla(const VmbHandle_t cameraHandle)
{
    /*
     * Check if the optional sfnc feature GevCurrentIPConfigurationLLA is available and writeable.
     */
    VmbBool_t writeable = VmbBoolFalse;
    const VmbError_t error = VmbFeatureAccessQuery(cameraHandle, "GevCurrentIPConfigurationLLA", NULL, &writeable);
    const VmbBool_t cameraHasPersistentFeatures = (error == VmbErrorSuccess) && (writeable);

    /*
     * Use the available sfnc features or camera registers to set the LLA IP configuration
     */
    if (cameraHasPersistentFeatures)
    {
        VMB_PRINT("Setting LLA using sfnc features\n");
        RETURN_AND_PRINT_ON_ERROR(VmbFeatureBoolSet(cameraHandle, "GevCurrentIPConfigurationLLA", VmbBoolTrue));
    }
    else
    {
        VMB_PRINT("Setting LLA using device registers\n");
        RETURN_ON_ERROR(WriteInterfaceConfigRegister(cameraHandle, SetInterfaceConfigRegisterValue_LlaIpConfig));
    }

    return VmbErrorSuccess;
}

CameraOpenResult OpenCamera(const char* const cameraId)
{
    CameraOpenResult result;
    memset(&result, 0, sizeof(result));
    result.error = VmbErrorUnknown;

    /*
     * Check that the camera is not opened by another application or located in a different subnet
     */
    VmbCameraInfo_t info;
    RETURN_SPECIFIC_AND_PRINT_ON_ERROR(result.error = VmbCameraInfoQuery(cameraId, &info, sizeof(info)), result);

    VmbBool_t noFullAccess = ((info.permittedAccess & VmbAccessModeFull) != VmbAccessModeFull);
    if (noFullAccess)
    {
        result.error = VmbErrorInvalidAccess;
        VMB_PRINT("Not able to open the camera. The camera is used by a different application or located in a different subnet.");
        return result;
    }

    /*
     * Open the camera for later access
     */
    RETURN_SPECIFIC_AND_PRINT_ON_ERROR( (result.error = VmbCameraOpen(cameraId, VmbAccessModeFull, &result.cameraHandle)), result);

    VMB_PRINT("Opened camera\n");

    return result;
}

VmbError_t WritePersistentIp(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    /*
     * Check if the optional sfnc feature GevCurrentIPConfigurationPersistentIP is available and writeable.
     * It is assumed that the camera will either implement all the needed features for the persistent IP or none of them
     */
    VmbBool_t writeable = VmbBoolFalse;
    VmbError_t error = VmbFeatureAccessQuery(cameraHandle, "GevCurrentIPConfigurationPersistentIP", NULL, &writeable);
    const VmbBool_t cameraHasPersistentFeatures = (error == VmbErrorSuccess) && (writeable);

    /*
     * Use the available sfnc features or camera registers to set the persistent IP configuration
     */
    if (cameraHasPersistentFeatures)
    {
        VMB_PRINT("Setting persistent IP using sfnc features\n");
        error = WritePersistentIpFeatures(cameraHandle, ip, subnetMask, gateway);
    }
    else
    {
        VMB_PRINT("Setting persistent IP using device registers\n");
        error = WritePersistentIpRegisters(cameraHandle, (VmbUint32_t)ip, (VmbUint32_t)subnetMask, (VmbUint32_t)gateway);
    }
    return error;
}

VmbError_t WritePersistentIpFeatures(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
{
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentIPAddress", ip));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentSubnetMask", subnetMask));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureIntSet(cameraHandle, "GevPersistentDefaultGateway", gateway));
    RETURN_AND_PRINT_ON_ERROR(VmbFeatureBoolSet(cameraHandle, "GevCurrentIPConfigurationPersistentIP", VmbBoolTrue));

    return VmbErrorSuccess;
}

VmbError_t WritePersistentIpRegisters(const VmbHandle_t cameraHandle, const VmbUint32_t ip, const VmbUint32_t subnetMask, const VmbUint32_t gateway)
{
    const VmbUint64_t GIGE_PERSISTENT_IP_REGISTER           = 0x064C;
    const VmbUint64_t GIGE_PERSISTENT_SUBNET_MASK_REGISTER  = 0x065C;
    const VmbUint64_t GIGE_PERSISTENT_GATEWAY_REGISTER      = 0x066C;

    /*
     * Write the configuration to the camera registers
     */
    const VmbUint64_t addresses[] = { GIGE_PERSISTENT_IP_REGISTER, GIGE_PERSISTENT_SUBNET_MASK_REGISTER, GIGE_PERSISTENT_GATEWAY_REGISTER };
    const VmbUint32_t datas[]     = { ip , subnetMask , gateway };
    const VmbUint32_t registersToWrite = sizeof(datas) / sizeof(VmbUint32_t);
    for (VmbUint32_t currentIndex = 0; currentIndex < registersToWrite; currentIndex++)
    {
        RETURN_ON_ERROR(WriteRegister(cameraHandle, addresses[currentIndex], datas[currentIndex]));
    }

    /*
     * Set the interface configuration register to enable persistent IP
     */
    RETURN_ON_ERROR(WriteInterfaceConfigRegister(cameraHandle, SetInterfaceConfigRegisterValue_PersistentIpConfig));

    return VmbErrorSuccess;
}

VmbError_t WriteRegister(const VmbHandle_t cameraHandle, const VmbUint64_t address, VmbUint32_t data)
{
    #ifdef _LITTLE_ENDIAN
    data = htonl(data);
    #endif

    char strData[4];
    strData[0] = data;
    strData[1] = data >> 8;
    strData[2] = data >> 16;
    strData[3] = data >> 24;

    VmbUint32_t written = 0;
    VmbUint32_t bufferSize = sizeof(VmbUint32_t);
    RETURN_AND_PRINT_ON_ERROR(VmbMemoryWrite(cameraHandle, address, bufferSize, strData, &written));

    return VmbErrorSuccess;
}

VmbError_t WriteInterfaceConfigRegister(const VmbHandle_t cameraHandle, void (*SetInterfaceConfigRegisterValue) (VmbUint32_t* const) )
{
    const VmbUint64_t GIGE_INTERFACE_CONFIG_REGISTER = 0x0014;

    /*
     * Read the Network Interface Configuration register
     */
    VmbUint32_t interfaceConfiguration = 0;
    VmbUint32_t bytesRead = 0;
    RETURN_AND_PRINT_ON_ERROR(VmbMemoryRead(cameraHandle, GIGE_INTERFACE_CONFIG_REGISTER, sizeof(interfaceConfiguration), (char*)(&interfaceConfiguration), &bytesRead));

    #ifdef _LITTLE_ENDIAN
    interfaceConfiguration = ntohl(interfaceConfiguration);
    #endif // _LITTLE_ENDIAN

    /*
     * Set the configuration bits
     */
    SetInterfaceConfigRegisterValue(&interfaceConfiguration);

    RETURN_ON_ERROR(WriteRegister(cameraHandle, GIGE_INTERFACE_CONFIG_REGISTER, interfaceConfiguration));

    return VmbErrorSuccess;
}

void SetInterfaceConfigRegisterValue_PersistentIpConfig(VmbUint32_t* const interfaceConfiguration)
{
    *interfaceConfiguration = *interfaceConfiguration | 1UL;
}

void SetInterfaceConfigRegisterValue_DhcpIpConfig(VmbUint32_t* const interfaceConfiguration)
{
    *interfaceConfiguration = *interfaceConfiguration & ~(1UL);
    *interfaceConfiguration = *interfaceConfiguration | (1UL << 1);
}

void SetInterfaceConfigRegisterValue_LlaIpConfig(VmbUint32_t* const interfaceConfiguration)
{
    *interfaceConfiguration = *interfaceConfiguration & ~(1UL);
    *interfaceConfiguration = *interfaceConfiguration & ~(1UL << 1);
}
