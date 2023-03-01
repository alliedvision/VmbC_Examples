/*=============================================================================
  Copyright (C) 2014 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ActionCommands.c

  Description: The ActionCommands example will grab images asynchronously.
               The acquisition of each image is triggered by an Action Command.

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

#include "ActionCommands.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>

#include <VmbC/VmbC.h>

/**
 * \brief number of frames used for the acquisition
 */
#define NUM_FRAMES ((size_t)5)

/**
 * \brief feature name of custom command for choosing the packet size provided by the AVT GigETL
 */
#define ADJUST_PACKAGE_SIZE_COMMAND "GVSPAdjustPacketSize"

VmbBool_t               g_vmbStarted               = VmbBoolFalse;      // Remember if Vmb is started
VmbBool_t               g_streaming                = VmbBoolFalse;      // Remember if Vmb is streaming
VmbBool_t               g_acquiring                = VmbBoolFalse;      // Remember if Vmb is acquiring
VmbHandle_t             g_cameraHandle             = NULL;              // A handle to our camera
VmbFrame_t              g_frames[NUM_FRAMES];                           // The frames we capture into

/**
 *\brief called from Vmb if a frame is ready for user processing
 * 
 * \param[in] cameraHandle handle to camera that supplied the frame
 * \param[in] streamHandle handle to stream that supplied the frame
 * \param[in] frame pointer to frame structure that can hold valid data
 */
//void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, const VmbHandle_t streamHandle, VmbFrame_t* frame)
//{
    //
    // from here on the frame is under user control until returned to Vmb by requeuing it
    // if you want to have smooth streaming keep the time you hold the frame short
    //

    //
    // Note:    If VmbCaptureEnd is called asynchronously, while this callback is running, VmbCaptureEnd blocks,
    //          until the callback returns.
    //

    //Irgendeine Ausgabe das der Frame empfangen wurde

    // requeue the frame so it can be filled again
//    VmbCaptureFrameQueue(cameraHandle, frame, &FrameCallback);
//}

VmbError_t StartContinuousImageAcquisition(ActionCommandsOptions* options)
{
    /*
    VmbError_t          err                 = VmbErrorSuccess;      // The function result
    VmbUint32_t         nCount              = 0;                    // Number of found cameras
    VmbUint32_t         nFoundCount         = 0;                    // Change of found cameras
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeFull;    // We open the camera with full access

    if(!g_vmbStarted)
    {
        // initialize global state
        g_streaming                = VmbBoolFalse;
        g_acquiring                = VmbBoolFalse;
        g_cameraHandle             = NULL;
        memset(g_frames, 0, sizeof(g_frames));

        err = VmbStartup(NULL);

        PrintVmbVersion();

        if (VmbErrorSuccess != err)
        {
            return err;
        }

        // Passende Kamera finden bzw. gewünschte Kamera besorgen der Infos (evtl. passender TL oder passende ID, immer GigE)

        // Kamera Öffnen

        // Payload Size und Alignment checken

        // Buffers anlegen und an API übergeben

        // Action Cmd Stuff Setzen

        // Stream Starten

        // Immer bei X Action Cmd senden

        // Wenn q oder so stream beenden / runterfahren
            g_vmbStarted = VmbBoolTrue;
            
            VmbUint32_t cameraCount = 0;
            char const* cameraId = options->cameraId;

            if (cameraId == NULL)
            {
                VmbCameraInfo_t* cameras = NULL;
                err = ListCameras(&cameras, &cameraCount);

                if (err == VmbErrorSuccess)
                {
                    if (cameraCount > 0)
                    {
                        cameraId = cameras->cameraIdString; // use id of the first camera
                    }
                    else
                    {
                        printf("no cameras found\n");
                        err = VmbErrorNotFound;
                    }
                }
                else
                {
                    printf("%s Could not list cameras or no cameras present. Error code: %d\n", __FUNCTION__, err);
                }
                free(cameras);
            }

            if (NULL != cameraId)
            {
                // Open camera
                printf("Opening camera with ID: %s\n", cameraId);
                err = VmbCameraOpen(cameraId, cameraAccessMode, &g_cameraHandle);

                if (VmbErrorSuccess == err)
                {
                    // Try to execute custom command available to Allied Vision GigE Cameras to ensure the packet size is chosen well
                    VmbCameraInfo_t info;
                    err = VmbCameraInfoQuery(cameraId, &info, sizeof(info));
                    VmbHandle_t stream = info.streamHandles[0];
                    if (VmbErrorSuccess == VmbFeatureCommandRun(stream, ADJUST_PACKAGE_SIZE_COMMAND))
                    {
                        VmbBool_t isCommandDone = VmbBoolFalse;
                        do
                        {
                            if (VmbErrorSuccess != VmbFeatureCommandIsDone(stream,
                                ADJUST_PACKAGE_SIZE_COMMAND,
                                &isCommandDone))
                            {
                                break;
                            }
                        } while (VmbBoolFalse == isCommandDone);
                        VmbInt64_t packetSize = 0;
                        VmbFeatureIntGet(stream, "GVSPPacketSize", &packetSize);
                        printf("GVSPAdjustPacketSize: %lld\n", packetSize);
                    }

                    // Evaluate required alignment for frame buffer in case announce frame method is used
                    VmbInt64_t nStreamBufferAlignment = 1;  // Required alignment of the frame buffer
                    if (VmbErrorSuccess != VmbFeatureIntGet(stream, "StreamBufferAlignment", &nStreamBufferAlignment))
                        nStreamBufferAlignment = 1;

                    if (nStreamBufferAlignment < 1)
                        nStreamBufferAlignment = 1;

                    if (!options->allocAndAnnounce)
                        printf("StreamBufferAlignment=%lld\n", nStreamBufferAlignment);

                    if (VmbErrorSuccess == err)
                    {
                        VmbUint32_t payloadSize = 0;

                        // determine the required buffer size
                        err = VmbPayloadSizeGet(g_cameraHandle, &payloadSize);
                        if (VmbErrorSuccess == err)
                        {
                            for (size_t i = 0; i < NUM_FRAMES; i++)
                            {
                                if (options->allocAndAnnounce)
                                {
                                    g_frames[i].buffer = NULL;
                                }
                                else
                                {
#ifdef _WIN32
                                    g_frames[i].buffer = (unsigned char*)_aligned_malloc((size_t)payloadSize, (size_t)nStreamBufferAlignment);
#else
                                    g_frames[i].buffer = (unsigned char*)aligned_alloc((size_t)nStreamBufferAlignment, (size_t)payloadSize);
#endif
                                    if (NULL == g_frames[i].buffer)
                                    {
                                        err = VmbErrorResources;
                                        break;
                                    }
                                }
                                g_frames[i].bufferSize = payloadSize;
                                g_frames[i].context[FRAME_CONTEXT_OPTIONS_INDEX] = options;
                                g_frames[i].context[FRAME_CONTEXT_STREAM_STATISTICS_INDEX] = statistics;

                                // Announce Frame
                                err = VmbFrameAnnounce(g_cameraHandle, &g_frames[i], (VmbUint32_t)sizeof(VmbFrame_t));
                                if (VmbErrorSuccess != err)
                                {
#ifdef _WIN32
                                    _aligned_free(g_frames[i].buffer);
#else
                                    free(g_frames[i].buffer);
#endif
                                    memset(&g_frames[i], 0, sizeof(VmbFrame_t));
                                    break;
                                }
                            }

                            if (VmbErrorSuccess == err)
                            {
                                // Start Capture Engine
                                err = VmbCaptureStart(g_cameraHandle);
                                if (VmbErrorSuccess == err)
                                {
                                    g_streaming = VmbBoolTrue;
                                    for (size_t i = 0; i < NUM_FRAMES; i++)
                                    {
                                        // Queue Frame
                                        err = VmbCaptureFrameQueue(g_cameraHandle, &g_frames[i], &FrameCallback);
                                        if (VmbErrorSuccess != err)
                                        {
                                            break;
                                        }
                                    }

                                    if (VmbErrorSuccess == err)
                                    {
                                        // Start Acquisition
                                        err = VmbFeatureCommandRun(g_cameraHandle, "AcquisitionStart");
                                        if (VmbErrorSuccess == err)
                                        {
                                            g_acquiring = VmbBoolTrue;
                                        }
                                    }
                                }
                                else
                                {
                                    printf("Error %d in VmbCaptureStart\n", err);
                                }
                            }
                        }
                    }
                }
                else
                {
                    printf("Error %d in VmbCameraOpen\n", err);
                }
            }

            if(VmbErrorSuccess != err)
            {
                StopContinuousImageAcquisition();
            }
        }
        return err;
    }
    else
    {
        return VmbErrorAlready;
    }*/
return 0;
}

void StopContinuousImageAcquisition()
{
    /*
    int i = 0;

    _Bool const shutdownDone = atomic_flag_test_and_set(&g_shutdown);

    if(!shutdownDone)
    {
        if (g_vmbStarted)
        {
            if (NULL != g_cameraHandle)
            {
                if (g_acquiring)
                {
                    // Stop Acquisition
                    VmbFeatureCommandRun(g_cameraHandle, "AcquisitionStop");
                    g_acquiring = VmbBoolFalse;
                }

                if (g_streaming)
                {
                    // Stop Capture Engine
                    VmbCaptureEnd(g_cameraHandle);
                    g_streaming = VmbBoolFalse;
                }

                // Flush the capture queue
                VmbCaptureQueueFlush(g_cameraHandle);

                while (VmbErrorSuccess != VmbFrameRevokeAll(g_cameraHandle))
                {
                }

                for (i = 0; i < NUM_FRAMES; i++)
                {
                    if (NULL != g_frames[i].buffer && !g_bUseAllocAndAnnouce)
                    {
#ifdef _WIN32
                        _aligned_free(g_frames[i].buffer);
#else
                        free(g_frames[i].buffer);
#endif
                        memset(&g_frames[i], 0, sizeof(VmbFrame_t));
                    }
                }
                // Close camera
                VmbCameraClose(g_cameraHandle);
                g_cameraHandle = NULL;
            }
            VmbShutdown();
            g_vmbStarted = VmbBoolFalse;
        }
        mtx_destroy(&g_frameInfoMutex);
    }
    */
}

