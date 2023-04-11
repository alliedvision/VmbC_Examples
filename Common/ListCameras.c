/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>

#include "include/VmbCExamplesCommon/ListCameras.h"

#include "include/VmbCExamplesCommon/ArrayAlloc.h"

#include <VmbC/VmbC.h>


VmbError_t ListCameras(VmbCameraInfo_t** cameras, VmbUint32_t* count)
{
    VmbUint32_t camCount = 0;
    VmbError_t err = VmbCamerasList(NULL, 0, &camCount, sizeof(VmbCameraInfo_t)); // get the number of cameras
    if (err != VmbErrorSuccess)
    {
        return err;
    }

    if (camCount == 0)
    {
        printf("no cameras found\n");
        return VmbErrorNotFound;
    }

    VmbCameraInfo_t* res = VMB_MALLOC_ARRAY(VmbCameraInfo_t, camCount); // get the camera info
    if (res == NULL)
    {
        printf("insufficient memory available");
        return VmbErrorResources;
    }

    VmbUint32_t countNew = 0;
    err = VmbCamerasList(res, camCount, &countNew, sizeof(VmbCameraInfo_t));
    if (err == VmbErrorSuccess || (err == VmbErrorMoreData && camCount < countNew))
    {
        if (countNew == 0)
        {
            err = VmbErrorNotFound;
        }
        else
        {
            *cameras = res;
            *count = countNew > camCount ? camCount : countNew;
            return VmbErrorSuccess;
        }
    }

    free(res);

    return err;
}
