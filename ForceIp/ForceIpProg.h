/**
 * \date 2022
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Declaration of the function which implements the force ip example
 */

#ifndef FORCE_IP_PROG_H_
#define FORCE_IP_PROG_H_

/**
 * \brief modifies the IP configuration for a camera identified by the given mac address
 *
 * Sends a force IP command to apply the given ip configuration.
 * The operation is performed on each interface that the camera is connected to until success,
 * but if the operation fails on all these interfaces then the operation tries each Vimba X and Vimba GigE transport layer until success.
 * The configuration will be lost after a power-cycle of the camera.
 *
 * \param[in] strMAC       The mac address of the desired camera whose ip configuration is to be updated
 * \param[in] strIP        The desired ip address
 * \param[in] strSubnet    The desired subnet mask
 * \param[in] strGateway   The desired gateway. Optional, can be 0.
 *
 * \return a code to return from main()
*/
int ForceIpProg(const char* const strMAC, const char* const strIP, const char* const strSubnet, const char* const strGateway);

#endif // FORCE_IP_PROG_H_
