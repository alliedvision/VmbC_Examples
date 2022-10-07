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
 * The configuration will be lost after a power-cycle of the camera.
 * Assumes that the VmbC API is already started.
 *
 * By default the progress and error messages are printed out. Define _VMB_FORCE_IP_NO_PRINT to disable the print out.
 *
 * \param[in] mac       mac address of the camera whoose ip configuration is to be updated
 * \param[in] ip        the desired ip address
 * \param[in] subnet    the desired subnet mask
 * \param[in] gateway   the desired gateway. Optional, can be 0.
 *
 * \return error code reporting the success of the operation
*/
VmbError_t ForceIp(const char* const mac, const char* const ip, const char* const subnet, const char* const gateway);

#endif // FORCE_IP_H_