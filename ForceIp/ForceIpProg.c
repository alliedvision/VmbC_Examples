/**
 * \date 2022
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Definition of the function which implements the force ip example
 */

#include <stdio.h>

#include "ForceIpProg.h"
#include "ForceIp.h"

#include <VmbC/VmbC.h>

#include <VmbCExamplesCommon/PrintVmbVersion.h>
#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

int ForceIpProg(const char* const strMAC, const char* const strIP, const char* const strSubnet, const char* const strGateway)
{
    /*
     * Initialize the VmbC API
     */
    VmbError_t err = VmbStartup(NULL);
    const VmbBool_t apiStartFailed = (VmbErrorSuccess != err);
    if (apiStartFailed)
    {
        printf("VmbStartup failed. %s Error code: %d.", ErrorCodeToMessage(err), err);
        return 1;
    }

    PrintVmbVersion();

    err = ForceIp(strMAC, strIP, strSubnet, strGateway);

    VmbShutdown();

    return (err == VmbErrorSuccess) ? 0 : 1;
}
