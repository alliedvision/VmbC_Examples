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
 * \brief Declaration of the function which implements the force ip configuration
 */

#ifndef FORCE_IP_H_
#define FORCE_IP_H_

#include <VmbC/VmbC.h>

/**
 * \brief modifies the IP configuration for a camera identified by the given mac address
 *
 * Sends a force IP command to apply the given ip configuration.
 * The operation is performed on each interface that the camera is connected to until success,
 * but if the operation fails on all these interfaces then the operation tries each Vimba X and Vimba GigE transport layer until success.
 * The configuration will be lost after a power-cycle of the camera.
 * Assumes that the VmbC API is already started.
 *
 * By default the progress and error messages are printed out. Define _VMB_FORCE_IP_NO_PRINT to disable the print out.
 *
 * \param[in] mac     The desired mac address
 * \param[in] ip      The desired ip address
 * \param[in] subnet  The desired subnet mask
 * \param[in] gateway The desired gateway. Optional, can be 0.
 *
 * \return error code reporting the success of the operation
*/
VmbError_t ForceIp(const char* const mac, const char* const ip, const char* const subnet, const char* const gateway);

#endif // FORCE_IP_H_
