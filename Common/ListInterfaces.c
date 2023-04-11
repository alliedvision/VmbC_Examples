/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>

#include "include/VmbCExamplesCommon/ListInterfaces.h"

#include "include/VmbCExamplesCommon/ArrayAlloc.h"

#include <VmbC/VmbC.h>


VmbError_t ListInterfaces(VmbInterfaceInfo_t** interfaces, VmbUint32_t* count)
{
    VmbUint32_t interfaceCount = 0;
    VmbError_t err = VmbInterfacesList(NULL, 0, &interfaceCount, sizeof(VmbInterfaceInfo_t)); // get the number of interfaces
    if (err != VmbErrorSuccess)
    {
        return err;
    }
    if (interfaceCount == 0)
    {
        printf("no interfaces found\n");
        return VmbErrorNotFound;
    }

    VmbInterfaceInfo_t* res = VMB_MALLOC_ARRAY(VmbInterfaceInfo_t, interfaceCount); // get the interface info
    if (res == NULL)
    {
        printf("insufficient memory available");
        return VmbErrorResources;
    }

    VmbUint32_t countNew = 0;
    err = VmbInterfacesList(res, interfaceCount, &countNew, sizeof(VmbInterfaceInfo_t));
    if (err == VmbErrorSuccess || (err == VmbErrorMoreData && countNew > interfaceCount))
    {
        if (countNew == 0)
        {
            err = VmbErrorNotFound;
        }
        else
        {
            *interfaces = res;
            *count = countNew > interfaceCount ? interfaceCount : countNew;
            return VmbErrorSuccess;
        }
    }

    free(res);

    return err;
}
