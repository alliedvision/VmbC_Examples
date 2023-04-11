/**
 * \date 2022-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Declaration of the function which implements the IP configuration
 */

#ifndef CONFIG_IP_H_
#define CONFIG_IP_H_

#include <VmbC/VmbC.h>

/**
 * \brief set an IP configuration for a camera identified by its ID
 *
 * Starts the VmbC API. Writes the IP configuration into the camera's IP configuration registers.
 * The new configuration will be retained and applied after a power-cycle of the camera.
 * Assumes that the VmbC API is already started.
 *
 * By default the progress and error messages are printed out. Define _VMB_CONFIG_IP_NO_PRINT to disable the print out.
 *
 * \param[in] cameraId ID of the desired camera whose IP configuration is to be updated
 * \param[in] ip       For setting the IP configuration to:
                                   1. a persistent IP: the desired IP address
                                   2.            DHCP: the string "dhcp"
                                   3.             LLA: a NULL pointer
 * \param[in] subnet   The desired subnet mask in the case of setting a persistent IP, otherwise a NULL pointer
 * \param[in] gateway  The desired gateway in the case of setting a persistent IP. Optional, can be a NULL pointer.
 *
 * \return error code reporting the success of the operation
*/
VmbError_t ConfigIp(const char* const cameraId, const char* const ip, const char* const subnet, const char* const gateway);

#endif // CONFIG_IP_H_
