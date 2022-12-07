/*=============================================================================
  Copyright (C) 2012 - 2022 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ChunkAccessProg.c

  Description: The ChunkAccess example will receive chunk data of first camera
               that is found by VmbC.

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
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

#include "ChunkAccessProg.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>

#define NUM_FRAMES ((size_t)5)


VmbError_t VMB_CALL ChunkCallback(VmbHandle_t featureAccessHandle, void* userContext)
{
    VmbError_t err = VmbErrorSuccess;
    VmbInt64_t id = 0, ts = 0, w = 0, h = 0;

    err = VmbFeatureIntGet(featureAccessHandle, "ChunkWidth", &w);

    err = VmbFeatureIntGet(featureAccessHandle, "ChunkHeight", &h);

    err = VmbFeatureIntGet(featureAccessHandle, "ChunkFrameID", &id);

    err = VmbFeatureIntGet(featureAccessHandle, "ChunkTimestamp", &ts);

    printf("  Chunk Data: id=%2.2lld ts=%lld width=%lld height=%lld\n", id, ts, w, h);

    return err;
}


void VMB_CALL FrameDoneCallback(const VmbHandle_t hCamera, const VmbHandle_t stream, VmbFrame_t* pFrame)
{
    VmbError_t err;

    if (VmbFrameStatusComplete == pFrame->receiveStatus)
    {
        printf("  Frame Done: id=%2.2lld ts=%lld  complete\n", pFrame->frameID, pFrame->timestamp);

        if (pFrame->chunkDataPresent)
        {
            err = VmbChunkDataAccess(pFrame, ChunkCallback, 0);
        }
    }
    else
    {
        printf("  Frame Done: id=%2.2lld not successfully received. Error code: %d %s\n", pFrame->frameID, pFrame->receiveStatus, pFrame->receiveStatus == VmbFrameStatusIncomplete ? "(incomplete)" : "");
    }

    err = VmbCaptureFrameQueue(hCamera, pFrame, FrameDoneCallback);
}


int ChunkAccessProg()
{
    VmbError_t err = VmbStartup(NULL);
    PrintVmbVersion();

    if (err == VmbErrorSuccess)
    {
        VmbUint32_t cameraCount = 0;
        VmbCameraInfo_t* cameras = NULL;
        VmbCameraInfo_t* camera = NULL;
        VmbHandle_t hCamera = NULL;

        err = ListCameras(&cameras, &cameraCount);
        if (err == VmbErrorSuccess)
        {
            // use first camera in list
            camera = cameras + 0;

            err = VmbCameraOpen(camera->cameraIdString, VmbAccessModeFull, &hCamera);
            if (err == VmbErrorSuccess)
            {
                VmbCameraInfo_t cameraInfo;
                err = VmbCameraInfoQueryByHandle(hCamera, &cameraInfo, sizeof(VmbCameraInfo_t));
                if (err == VmbErrorSuccess)
                {
                    printf(
                        "Camera id    : %s\n"
                        "Camera name  : %s\n"
                        "Model name   : %s\n"
                        "Serial Number: %s\n\n",
                        cameraInfo.cameraIdString,
                        cameraInfo.cameraName,
                        cameraInfo.modelName,
                        cameraInfo.serialString);

                    // activate chunk features
                    err = VmbFeatureBoolSet(hCamera, "ChunkModeActive", VmbBoolFalse);
                    err = VmbFeatureEnumSet(hCamera, "ChunkSelector", "FrameID");
                    err = VmbFeatureBoolSet(hCamera, "ChunkEnable", VmbBoolTrue);
                    err = VmbFeatureEnumSet(hCamera, "ChunkSelector", "Timestamp");
                    err = VmbFeatureBoolSet(hCamera, "ChunkEnable", VmbBoolTrue);
                    err = VmbFeatureEnumSet(hCamera, "ChunkSelector", "Width");
                    err = VmbFeatureBoolSet(hCamera, "ChunkEnable", VmbBoolTrue);
                    err = VmbFeatureEnumSet(hCamera, "ChunkSelector", "Height");
                    err = VmbFeatureBoolSet(hCamera, "ChunkEnable", VmbBoolTrue);
                    err = VmbFeatureBoolSet(hCamera, "ChunkModeActive", VmbBoolTrue);

                    // show camera setup
                    VmbInt64_t w = -1, h = -1, PLS = 0;
                    VmbBool_t cma = VmbBoolFalse;
                    double e = 1000.;
                    const char* pf;
                    err = VmbFeatureFloatGet(hCamera, "ExposureTime", &e);
                    err = VmbFeatureIntGet  (hCamera, "Width", &w);
                    err = VmbFeatureIntGet  (hCamera, "Height", &h);
                    err = VmbFeatureEnumGet (hCamera, "PixelFormat", &pf);
                    err = VmbFeatureBoolGet (hCamera, "ChunkModeActive", &cma);
                    err = VmbFeatureIntGet  (hCamera, "PayloadSize", &PLS);

                    printf("ExposureTime   : %.0f us\n", e);
                    printf("PixelFormat    : %s\n", pf);
                    printf("Width * Height : %lld * %lld\n", w, h);
                    printf("Payload Size   : %lld byte\n", PLS);
                    printf("ChunkModeActive: %d\n\n", cma);

                    // Try to execute custom command available to Allied Vision GigE Cameras to ensure the packet size is chosen well
                    if (VmbErrorSuccess == VmbFeatureCommandRun(cameraInfo.streamHandles[0], "GVSPAdjustPacketSize"))
                    {
                        VmbBool_t isCommandDone = VmbBoolFalse;
                        do
                        {
                            if (VmbErrorSuccess != VmbFeatureCommandIsDone(cameraInfo.streamHandles[0],
                                "GVSPAdjustPacketSize",
                                &isCommandDone))
                            {
                                break;
                            }
                        } while (VmbBoolFalse == isCommandDone);
                        VmbInt64_t packetSize = 0;
                        VmbFeatureIntGet(cameraInfo.streamHandles[0], "GVSPPacketSize", &packetSize);
                        printf("GVSPAdjustPacketSize: %lld\n", packetSize);
                    }

                    // allocate and announce frame buffer
                    VmbFrame_t frames[NUM_FRAMES];
                    VmbUint32_t payloadSize = 0;
                    err = VmbPayloadSizeGet(hCamera, &payloadSize);

                    // Evaluate required alignment for frame buffer in case announce frame method is used
                    VmbInt64_t nStreamBufferAlignment = 1;  // Required alignment of the frame buffer
                    if (VmbErrorSuccess != VmbFeatureIntGet(cameraInfo.streamHandles[0], "StreamBufferAlignment", &nStreamBufferAlignment))
                        nStreamBufferAlignment = 1;

                    if (nStreamBufferAlignment < 1)
                        nStreamBufferAlignment = 1;

                    for (int i = 0; i < NUM_FRAMES; ++i)
                    {
#ifdef _WIN32
                        frames[i].buffer = (unsigned char*)_aligned_malloc((size_t)payloadSize, (size_t)nStreamBufferAlignment);
#else
                        frames[i].buffer = (unsigned char*)aligned_alloc((size_t)nStreamBufferAlignment, (size_t)payloadSize);
#endif      
                        frames[i].bufferSize = payloadSize;
                        err = VmbFrameAnnounce(hCamera, &frames[i], sizeof(VmbFrame_t));
                    }

                    err = VmbCaptureStart(hCamera);

                    if (err == VmbErrorSuccess)
                    {

                        // Queue frames and register FrameDoneCallback
                        for (int i = 0; i < NUM_FRAMES; ++i)
                        {
                            err = VmbCaptureFrameQueue(hCamera, &frames[i], FrameDoneCallback);
                        }

                        // Start acquisition on the camera for 1sec
                        printf("AcquisitionStart...\n");
                        err = VmbFeatureCommandRun(hCamera, "AcquisitionStart");

                        printf("Wait 1000ms...\n");
#ifdef _WIN32
                        Sleep(1000);
#else
                        usleep(100000);
#endif
                        // Stop acquisition on the camera
                        printf("AcquisitionStop...\n");
                        err = VmbFeatureCommandRun(hCamera, "AcquisitionStop");

                        // Cleanup
                        printf("VmbCaptureEnd...\n");
                        err = VmbCaptureEnd(hCamera);

                        printf("VmbCaptureQueueFlush...\n");
                        err = VmbCaptureQueueFlush(hCamera);

                        printf("VmbFrameRevoke...\n");
                        for (int i = 0; i < NUM_FRAMES; ++i)
                        {
                            err = VmbFrameRevoke(hCamera, frames + i);
#ifdef _WIN32
                            _aligned_free(frames[i].buffer);
#else
                            free(frames[i].buffer);
#endif
                        }
                    }
                    else
                    {
                        printf("Error %d in VmbCaptureStart\n", err);
                    }

                    err = VmbCameraClose(hCamera);
                }
                else
                {
                    printf("Error %d in VmbCameraInfoQueryByHandle\n", err);
                }
            }
            else
            {
                printf("Error %d in VmbCameraOpen\n", err);
            }
        }

        free(cameras);
        printf("VmbShutdown...\n");
        VmbShutdown();
    }

    return err == VmbErrorSuccess ? 0 : 1;
}
