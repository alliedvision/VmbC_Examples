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
 * \brief Definition of the function which implements the Config IPs example
 */

#include <stdio.h>

#include "ConfigIpProg.h"
#include "ConfigIp.h"

#include <VmbC/VmbC.h>

#include <VmbCExamplesCommon/PrintVmbVersion.h>
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

int ConfigIpProg(const char* const cameraId, const char* const ip, const char* const subnet, const char* const gateway)
{
    /*
     * Initialize the VmbC API
     */
    VmbError_t err = VmbStartup(NULL);
    VmbBool_t apiStartFailed = (VmbErrorSuccess != err);
    if (apiStartFailed)
    {
        printf("VmbStartup failed. %s Error code: %d.", ErrorCodeToMessage(err), err);
        return 1;
    }

    PrintVmbVersion();

    err = ConfigIp(cameraId, ip, subnet, gateway);

    VmbShutdown();

    return (err == VmbErrorSuccess) ? 0 : 1;
}
