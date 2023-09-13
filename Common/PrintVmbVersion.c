/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include "include/VmbCExamplesCommon/PrintVmbVersion.h"

#include "include/VmbCExamplesCommon/ErrorCodeToMessage.h"

#include <stdio.h>

#include <VmbC/VmbC.h>

void PrintVmbVersion(void)
{
    VmbVersionInfo_t    versionInfo;
    VmbError_t          result = VmbVersionQuery(&versionInfo, sizeof(versionInfo));
    if(VmbErrorSuccess == result)
    {
        printf("Vmb Version Major: %u Minor: %u Patch: %u\n\n", versionInfo.major, versionInfo.minor, versionInfo.patch);
    }
    else
    {
        printf("VmbVersionQuery failed with reason: %s\n\n", ErrorCodeToMessage(result));
    }
}


