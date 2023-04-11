/**
 * \date 2022-2023
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
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
