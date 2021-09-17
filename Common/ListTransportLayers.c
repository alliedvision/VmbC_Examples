/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListTransportLayers.c

  Description: Get a list of the transport layers.

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
