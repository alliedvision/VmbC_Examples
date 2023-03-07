/*=============================================================================
  Copyright (C) 2014 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ImageAcquisition.c

  Description: Functions used to prepare, start and stop the image acquisition.

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
#include <stdlib.h>
#include <string.h>

#include "ImageAcquisition.h"

#include <VmbCExamplesCommon/ErrorCodeToMessage.h>

#include <VmbC/VmbC.h>

//Number of frame buffers used for streaming
#define FRAME_COUNT ((size_t)5)

VmbFrame_t g_frames[FRAME_COUNT];

/**
 * \brief   The used frame callback, printing information about the receive frame
 */
void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, const VmbHandle_t streamHandle, VmbFrame_t* frame)
{
    printf("New frame received - ");

    printf("FrameID: ");
    if (VmbFrameFlagsFrameID & frame->receiveFlags)
    {
        printf("%llu", frame->frameID);
    }
    else
    {
        printf("?");
    }

    printf(" Status: ");
    switch (frame->receiveStatus)
    {
        case VmbFrameStatusComplete:
        {
            printf("Complete");
            break;
        }
        case VmbFrameStatusIncomplete:
        {
            printf("Incomplete");
            break;
        }
        case VmbFrameStatusTooSmall:
        {
            printf("Too small");
            break;
        }
        case VmbFrameStatusInvalid:
        {
            printf("Invalid");
            break;
        }
        default:
        {
            printf("?");
            break;
        }
    }

    printf("\n");

    fflush(stdout);

    VmbCaptureFrameQueue(cameraHandle, frame, &FrameCallback);
}

void AdjustPacketSize(VmbHandle_t cameraHandle)
{
    //Query the camera information to get the cameras stream handle
    VmbCameraInfo_t info;
    VmbError_t error = VmbCameraInfoQueryByHandle(cameraHandle, &info, sizeof(info));
    if (error != VmbErrorSuccess)
    {
        printf("Could not query camera info. Reason: %s\n", ErrorCodeToMessage(error));
        return;
    }

    VmbHandle_t stream = info.streamHandles[0];

    /*
    Adjust the packet size used during streaming.
    Ignore any error because the feature is only implemented by the Allied Vision GigETL.
    */

    error = VmbFeatureCommandRun(stream, "GVSPAdjustPacketSize");
    if (error != VmbErrorSuccess)
    {
        return;
    }

    // Wait until the packet size adjustment is completed.

    VmbBool_t isCommandDone = VmbBoolFalse;
    do
    {
        error = VmbFeatureCommandIsDone(stream, "GVSPAdjustPacketSize", &isCommandDone);
    } while ((VmbBoolFalse == isCommandDone) && (error == VmbErrorSuccess));

    //Read and print the adjusted packet size

    VmbInt64_t packetSize = 0;
    error = VmbFeatureIntGet(stream, "GVSPPacketSize", &packetSize);
    if (error != VmbErrorSuccess)
    {
        return;
    }

    printf("GVSPPacketSize adjusted to: %lld\n", packetSize);
}

VmbError_t StartStream(VmbHandle_t cameraHandle)
{
    AdjustPacketSize(cameraHandle);

    // Read the current payload size to allocate the correct buffer size

    VmbUint32_t payloadSize = 0;
    VmbError_t error = VmbPayloadSizeGet(cameraHandle, &payloadSize);
    if (error != VmbErrorSuccess)
    {
        printf("Could not query payload size. Reason %s\n", ErrorCodeToMessage(error));
        return error;
    }

    // Allocate the needed frame buffers

    for (size_t i = 0; (i < FRAME_COUNT) && (error == VmbErrorSuccess); i++)
    {
        g_frames[i].buffer = malloc((size_t)payloadSize);
        if (g_frames[i].buffer == NULL)
        {
            error = VmbErrorResources;
        }
        else
        {
            g_frames[i].bufferSize = payloadSize;
        }
    }

    if (error == VmbErrorResources)
    {
        printf("Could not allocate all buffers.\n");
        return error;
    }

    // Announce the frames to the API
    for (size_t i = 0; (i < FRAME_COUNT) && (error == VmbErrorSuccess); i++)
    {
        error = VmbFrameAnnounce(cameraHandle, &(g_frames[i]), (VmbUint32_t)sizeof(VmbFrame_t));
    }

    if (error != VmbErrorSuccess)
    {
        printf("Could not announce all frames. Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    // Start the capturing
    error = VmbCaptureStart(cameraHandle);
    if (error != VmbErrorSuccess)
    {
        printf("Could not start capture. Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    // Queue the prepared frames
    for (size_t i = 0; (i < FRAME_COUNT) && (error == VmbErrorSuccess); i++)
    {
        error = VmbCaptureFrameQueue(cameraHandle, &(g_frames[i]), FrameCallback);
    }

    if (error != VmbErrorSuccess)
    {
        printf("Could not queue all frames. Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    // Run the Acquisition Start feature on the camera
    error = VmbFeatureCommandRun(cameraHandle, "AcquisitionStart");
    if (error != VmbErrorSuccess)
    {
        printf("Could not run feature command \"AcquisitionStart\". Reason: %s\n", ErrorCodeToMessage(error));
        return error;
    }

    return error;
}

VmbError_t StopStream(VmbHandle_t cameraHandle)
{
    // Revert the steps previously done during StartStream

    VmbFeatureCommandRun(cameraHandle, "AcquisitionStop");

    VmbCaptureEnd(cameraHandle);

    VmbCaptureQueueFlush(cameraHandle);

    // Try to revoke the frames until the are not used anymore internally
    VmbError_t error;
    do
    {
        error = VmbFrameRevokeAll(cameraHandle);
    } while (error == VmbErrorInUse);

    // Free the allocated frame buffers
    for (size_t i = 0; i < FRAME_COUNT; i++)
    {
        if (g_frames[i].buffer != NULL)
        {
            free(g_frames[i].buffer);
        }
        memset(&g_frames[i], 0, sizeof(VmbFrame_t));
    }

    return error;
}
