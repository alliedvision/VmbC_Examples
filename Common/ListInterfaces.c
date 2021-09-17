/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListInterfaces.c

  Description: Get the list of the interfaces.

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
