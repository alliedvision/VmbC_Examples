/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListCameras.c

  Description: Get the list of the cameras.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
