/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>

#include "include/VmbCExamplesCommon/ListTransportLayers.h"

#include "include/VmbCExamplesCommon/ArrayAlloc.h"

#include <VmbC/VmbC.h>


VmbError_t ListTransportLayers(VmbTransportLayerInfo_t** transportLayers, VmbUint32_t* count)
{
    VmbUint32_t tlCount = 0;
    VmbError_t err = VmbTransportLayersList(NULL, 0, &tlCount, sizeof(VmbTransportLayerInfo_t)); // get the number of transport layers
    if (err != VmbErrorSuccess)
    {
        return err;
    }
    if (tlCount == 0)
    {
        printf("no cameras found\n");
        return VmbErrorNotFound;
    }

    VmbTransportLayerInfo_t* res = VMB_MALLOC_ARRAY(VmbTransportLayerInfo_t, tlCount); // get the transport layer info
    if (res == NULL)
    {
        printf("insufficient memory available");
        return VmbErrorResources;
    }

    VmbUint32_t countNew = 0;
    err = VmbTransportLayersList(res, tlCount, &countNew, sizeof(VmbTransportLayerInfo_t));
    if (err == VmbErrorSuccess || (err == VmbErrorMoreData && tlCount < countNew))
    {
        if (countNew == 0)
        {
            err = VmbErrorNotFound;
        }
        else
        {
            *transportLayers = res;
            *count = tlCount < countNew ? tlCount : countNew;
            return VmbErrorSuccess;
        }
    }

    free(res);

    return err;
}
