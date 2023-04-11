/**
 * \date 2022
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Declaration of the function which implements the Config IP example
 */

#ifndef CONFIG_IP_PROG_H_
#define CONFIG_IP_PROG_H_

/**
 * \brief set an IP configuration for a camera identified by its ID
 *
 * Starts the VmbC API. Writes the IP configuration into the camera's IP configuration registers.
 * The new configuration will be retained and applied after a power-cycle of the camera.
 *
 * \param[in] cameraId ID of the desired camera whose IP configuration is to be updated
 * \param[in] ip       For setting the IP configuration to:
                                    1. a persistent IP: the desired IP address
                                    2.            DHCP: the string "dhcp"
                                    3.             LLA: a NULL pointer
 * \param[in] subnet   The desired subnet mask in the case of setting a persistent IP, otherwise a NULL pointer
 * \param[in] gateway  The desired gateway in the case of setting a persistent IP. Optional, can be a NULL pointer.
 *
 * \return a code to return from main()
*/
int ConfigIpProg(const char* const cameraId, const char* const ip, const char* const subnet, const char* const gateway);

#endif // CONFIG_IP_PROG_H_
