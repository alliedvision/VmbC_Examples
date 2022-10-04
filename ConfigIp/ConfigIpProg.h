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
