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

#define _CRT_SECURE_NO_WARNINGS // disable sscanf warning for Windows

#include "PersistentIpProg.h"

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



/**
 * \brief Converts a hexadecimal IP address into its decimal representation.
 * \param[in] strIp    The string representation of the IP
 *
 * \return The decimal representation of the IP address as integer in host byte order
*/
unsigned int IpAddr(const char* const strIp)
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
 * Macros used to control the print out of the progress and upcoming errors.
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
 * \brief Helper struct used to store information about a found camera
 */
typedef struct CameraSearchResult
{
    VmbError_t      error;      //!< Error code representing the success of the operation
    VmbCameraInfo_t cameraInfo; //!< Information about the found camera
} CameraSearchResult;

/**
 * \brief Helper struct used to store information about an opened camera
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
 * \brief Get api specific information about the desired camera
 *
 * \param cameraId  The id of the desired camera
 *
 * \return The collected information
*/
CameraSearchResult  GetCamera(const char* const cameraId);

/**
 * \brief Open a camera using the given camera id
 *
 * \param cameraId  The desired camera id
 *
 * \return Information collected during the opening
*/
CameraOpenResult    OpenCamera(const char* const cameraId);

/**
 * \brief Writes the desired ip configuration. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param cameraHandle  The handle of the desired camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIp(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Writes the desired ip configuration using standard features from the SFNC. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param cameraHandle  The handle of the desired camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIpFeatures(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway);

/**
 * \brief Writes the desired ip configuration using standard camera registers. The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param cameraHandle  The handle of the desired camera
 * \param ip            The desired ip address in host byte order
 * \param subnetMask    The desired subnet mask in host byte order
 * \param gateway       The desired gateway in host byte order
 *
 * \return Result of the operation
*/
VmbError_t              WritePersistentIpRegisters(const VmbHandle_t cameraHandle, const VmbUint32_t ip, const VmbUint32_t subnetMask, const VmbUint32_t gateway);



/*
 * PersistentIp implementation
 */

VmbError_t SetPersistentIp(const char* const cameraId, const char* const ip, const char* const subnet, const char* const gateway)
{
    /*
     * Convert the string parameters into the needed integer values in host byte order
     */
    VmbInt64_t ipValue          = (VmbInt64_t)IpAddr(ip);
    VmbInt64_t subnetMaskValue  = (VmbInt64_t)IpAddr(subnet);
    VmbInt64_t gatewayValue     = (gateway) ? (VmbInt64_t)IpAddr(gateway) : 0;

    VmbBool_t invalidConfiguration = ((ipValue == INADDR_NONE) || (subnetMaskValue == INADDR_NONE) || (gatewayValue == INADDR_NONE));
    if (invalidConfiguration)
    {
        VMB_PRINT("One or more invalid parameters: %s %s %s\n.", ip, subnet, gateway);
        return 1;
    }

    /*
     * Search the camera list to get the api wide unique extended camera id.
     */
    CameraSearchResult foundCamera = GetCamera(cameraId);
    RETURN_ON_ERROR(foundCamera.error);

    VMB_PRINT("Device is known to the api with unique extended id \"%s\"\n", foundCamera.cameraInfo.cameraIdExtended);

    /*
     * Open the camera using the previous found unique extended camera id.
     */
    CameraOpenResult openedCamera = OpenCamera(foundCamera.cameraInfo.cameraIdExtended);
    RETURN_ON_ERROR(openedCamera.error);

    /*
     * Write the desired persistent ip configuration using either features from the SFNC or standard camera registers
     */
    VmbError_t configError = WritePersistentIp(openedCamera.cameraHandle, ipValue, subnetMaskValue, gatewayValue);

    (configError == VmbErrorSuccess) ? VMB_PRINT("Persistent IP configuration written to camera\n") : VMB_PRINT("Persistent IP configuration not written completely to camera\n");

    if (openedCamera.cameraHandle)
    {
        VmbCameraClose(openedCamera.cameraHandle);
    }

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

CameraSearchResult GetCamera(const char* const cameraId)
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
     * Search the camera list for a camera with an ID matching the ID of the desired camera.
     * Although this camera may be present for each GigE transport layer, we will just choose the first in the list.
     */
    VmbBool_t cameraFound = VmbBoolFalse;
    for (VmbUint32_t currentCameraIndex = 0; (currentCameraIndex < cameraCount) && (!cameraFound); currentCameraIndex++)
    {
        VmbCameraInfo_t* currentCamera = cameras + currentCameraIndex;
        VmbBool_t matchingCameraId = (strcmp(currentCamera->cameraIdString, cameraId) == 0);

        if (matchingCameraId)
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
     * In case the ip configuration was recently changed (e.g. via the force ip command),
     * the transport layer might need some time to detect this
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

VmbError_t WritePersistentIp(const VmbHandle_t cameraHandle, const VmbInt64_t ip, const VmbInt64_t subnetMask, const VmbInt64_t gateway)
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
    const VmbUint64_t GIGE_INTERFACE_CONFIG_REGISTER        = 0x0014;
    const VmbUint64_t GIGE_PERSISTENT_IP_REGISTER           = 0x064C;
    const VmbUint64_t GIGE_PERSISTENT_SUBNET_MASK_REGISTER  = 0x065C;
    const VmbUint64_t GIGE_PERSISTENT_GATEWAY_REGISTER      = 0x066C;

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
     * Set the persistent ip configuration bit
     */
    interfaceConfiguration = interfaceConfiguration | 0x1;


    /*
     * Write the configuration to the camera registers
     */
    VmbUint64_t addresses[] = { GIGE_PERSISTENT_IP_REGISTER, GIGE_PERSISTENT_SUBNET_MASK_REGISTER, GIGE_PERSISTENT_GATEWAY_REGISTER, GIGE_INTERFACE_CONFIG_REGISTER };
    VmbUint32_t datas[]     = { ip , subnetMask , gateway, interfaceConfiguration };
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
