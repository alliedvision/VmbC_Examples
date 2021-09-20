/*=============================================================================
  Copyright (C) 2014 - 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        AsynchronousGrab.c

  Description: The AsynchronousGrab example will grab images asynchronously
               using VmbC.

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

#include "AsynchronousGrab.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>
#include <VmbCExamplesCommon/VmbStdatomic.h>
#include <VmbCExamplesCommon/VmbThreads.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <time.h>
#endif

#include <VmbC/VmbC.h>

#include <VmbImageTransform/VmbTransform.h>


#define NUM_FRAMES ((size_t)3)
#define FRAME_CONTEXT_OPTIONS_INDEX ((size_t)0)

/**
 * \brief feature name of custom command for choosing the packet size provided by AVT GigE cameras
 */
#define ADJUST_PACKAGE_SIZE_COMMAND "GVSPAdjustPacketSize"

VmbBool_t               g_vmbStarted               = VmbBoolFalse;      // Remember if Vmb is started
VmbBool_t               g_streaming                = VmbBoolFalse;      // Remember if Vmb is streaming
VmbBool_t               g_acquiring                = VmbBoolFalse;      // Remember if Vmb is acquiring
VmbHandle_t             g_cameraHandle             = NULL;              // A handle to our camera
VmbFrame_t              g_frames[NUM_FRAMES];                           // The frames we capture into

double                  g_frameTime                = 0.0;               // Timestamp of last frame
VmbBool_t               g_frameTimeValid           = VmbBoolFalse;      // Remember if there was a last timestamp
VmbUint64_t             g_frameID                  = 0;                 // ID of last frame
VmbBool_t               g_frameIdValid             = VmbBoolFalse;      // Remember if there was a last ID
mtx_t                   g_frameInfoMutex;                               // mutex guarding access to the frame global info

volatile atomic_flag    g_shutdown                 = ATOMIC_FLAG_INIT;  // flag set to true, if a thread initiates the shutdown 

#ifdef WIN32
double          g_frequency                = 0.0;              //Frequency of tick counter in Win32
#else
#endif

/**
 * \brief Purpose: convert frames to RGB24 format and apply color processing if desired
 * 
 * \param[in] pFrame frame to process data might be destroyed dependent on transform function used
 */
VmbError_t ProcessFrame(VmbFrame_t * pFrame, VmbBool_t doColorProcessing)
{
    VmbTransformInfo    transformInfo;                         // if desired the transform information is constructed here

    // check if we can get data
    if(NULL == pFrame || NULL == pFrame->buffer)
    {
        printf("%s error invalid frame\n", __FUNCTION__);
        return VmbErrorBadParameter;
    }

    VmbUint32_t width   = pFrame->width;
    VmbUint32_t height  = pFrame->height;

    VmbError_t result               = VmbErrorSuccess;
    VmbUint32_t transformInfoCount  = 0;
    if(doColorProcessing)   // if color processing is desired set the transform matrix
    {
        const static VmbFloat_t matrix[9] = {                                   // matrix to swap red and blue component
                                                0.0, 0.0, 1.0,
                                                0.0, 1.0, 0.0,
                                                1.0, 0.0, 0.0
                                            };
        result = VmbSetColorCorrectionMatrix3x3(matrix, &transformInfo);        // initialize transform info
        if(VmbErrorSuccess != result)
        {
            printf("%s error could not set transform matrix; Error: %d\n", __FUNCTION__, result);
            return result;
        }
        transformInfoCount = 1;
    }

    VmbImage sourceImage = {
        .Size = sizeof(sourceImage) // image transformation functions require the size to specified correctly
    };

    // set the image information from the frames pixel format and size
    result = VmbSetImageInfoFromPixelFormat(pFrame->pixelFormat, width, height, &sourceImage);
    if(VmbErrorSuccess != result)
    {
        printf("%s error could not set source image info; Error: %d\n", __FUNCTION__, result);
        return result;
    }

    // the frame buffer will be the images data buffer
    sourceImage.Data = pFrame->buffer;

    VmbImage destinationImage = {
        .Size = sizeof(destinationImage) // image transformation functions require the size to specified correctly
    };

    // set destination image info from frame size and string for RGB8 (rgb24)
    result = VmbSetImageInfoFromString("RGB8", 4, width, height, &destinationImage);
    if(VmbErrorSuccess != result)
    {
        printf("%s error could not set destination image info; Error: %d\n", __FUNCTION__, result);
        return result;
    }
    // allocate buffer for destination image size is width * height * size of rgb
    VmbRGB8_t* destinationBuffer = (VmbRGB8_t*) malloc(width * height * sizeof(VmbRGB8_t));
    if(NULL == destinationBuffer)
    {
        printf("%s error could not allocate rgb buffer for width: %d and height: %d\n", __FUNCTION__, width, height);
        return VmbErrorResources;
    }

    // set the destination buffer to the data buffer of the image
    destinationImage.Data = destinationBuffer;

    // transform source to destination if color processing was enabled transformInfoCount is 1 otherwise transformInfo will be ignored
    result = VmbImageTransform(&sourceImage, &destinationImage, &transformInfo, transformInfoCount);

    // print first rgb pixel
    printf("R: %d\tG: %d\tB: %d\n", destinationBuffer->R, destinationBuffer->G, destinationBuffer->B);

    // clean image buffer
    free(destinationBuffer);

    return result;
}

/**
 * \brief get time indicator
 *
 * \return time indicator in seconds for differential measurements
 */
double GetTime()
{
#ifdef WIN32
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter(&nCounter);
    return ((double)nCounter.QuadPart) / g_frequency;
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return ((double)now.tv_sec) + ((double)now.tv_nsec) / 1000000000.0;
#endif //WIN32
}

/**
 *\brief called from Vmb if a frame is ready for user processing
 * 
 * \param[in] cameraHandle handle to camera that supplied the frame
 * \param[in] streamHandle handle to stream that supplied the frame
 * \param[in] frame pointer to frame structure that can hold valid data
 */
void VMB_CALL FrameCallback(const VmbHandle_t cameraHandle, const VmbHandle_t streamHandle, VmbFrame_t* frame)
{
    //
    // from here on the frame is under user control until returned to Vmb by requeuing it
    // if you want to have smooth streaming keep the time you hold the frame short
    //

    AsynchronousGrabOptions const* options = (AsynchronousGrabOptions const*) frame->context[FRAME_CONTEXT_OPTIONS_INDEX];

    VmbBool_t showFrameInfos = VmbBoolFalse;         // showing frame infos 
    double fps = 0.0;
    VmbBool_t fpsValid = VmbBoolFalse;

    if(FrameInfos_Off != options->frameInfos)
    {
        if(FrameInfos_Show == options->frameInfos)
        {
            showFrameInfos = VmbBoolTrue;
        }

        if (mtx_lock(&g_frameInfoMutex))
        {
            if (VmbFrameFlagsFrameID & frame->receiveFlags)
            {
                VmbUint64_t missingFrameCount = 0;
                if (g_frameIdValid)
                {
                    if (frame->frameID != (g_frameID + 1))
                    {
                        // get difference between current frame and last received frame to calculate missing frames
                        missingFrameCount = frame->frameID - g_frameID - 1;
                        if (1 == missingFrameCount)
                        {
                            printf("%s 1 missing frame detected\n", __FUNCTION__);
                        }
                        else
                        {
                            printf("%s error %llu missing frames detected\n", __FUNCTION__, missingFrameCount);
                        }
                    }
                }
                g_frameID = frame->frameID;          // store current frame id to calculate missing frames in the next calls
                g_frameIdValid = VmbBoolTrue;

                double frameTime = GetTime();              // get current time to calculate frames per second
                if ((g_frameTimeValid)                      // only if the last time was valid
                    && (0 == missingFrameCount))           // and the frame is not missing
                {
                    double timeDiff = frameTime - g_frameTime;  // build time difference with last frames time
                    if (timeDiff > 0.0)
                    {
                        fps = 1.0 / timeDiff;
                        fpsValid = VmbBoolTrue;
                    }
                    else
                    {
                        showFrameInfos = VmbBoolTrue;
                    }
                }
                // store time for fps calculation in the next call
                g_frameTime = frameTime;
                g_frameTimeValid = VmbBoolTrue;
            }
            else
            {
                showFrameInfos = VmbBoolTrue;
                g_frameIdValid = VmbBoolFalse;
                g_frameTimeValid = VmbBoolFalse;
            }
            mtx_unlock(&g_frameInfoMutex);
        }
        // test if the frame is complete
        if(VmbFrameStatusComplete != frame->receiveStatus)
        {
            showFrameInfos = VmbBoolTrue;
        }
    }

    if(showFrameInfos)
    {
        printf("Frame ID:");
        if(VmbFrameFlagsFrameID & frame->receiveFlags)
        {
            printf("%llu", frame->frameID);
        }
        else
        {
            printf("?");
        }

        printf(" Status:");
        switch(frame->receiveStatus)
        {
        case VmbFrameStatusComplete:
            printf("Complete");
            break;

        case VmbFrameStatusIncomplete:
            printf("Incomplete");
            break;

        case VmbFrameStatusTooSmall:  
            printf("Too small");
            break;

        case VmbFrameStatusInvalid:
            printf("Invalid");
            break;

        default:
            printf("?");
            break;
        }

        printf(" Size:");
        if(VmbFrameFlagsDimension & frame->receiveFlags)
        {
            printf("%ux%u", frame->width, frame->height);
        }
        else
        {
            printf("?x?");
        }

        printf(" Format:0x%08X", frame->pixelFormat);

        printf(" FPS:");
        if(fpsValid)
        {
            printf("%.2f", fps);
        }
        else
        {
            printf("?");
        }

        printf("\n");
    }

    if (options->showRgbValue)
    {
        ProcessFrame(frame, options->enableColorProcessing);
    }
    else if (FrameInfos_Show != options->frameInfos)
    {
        // Print a dot every frame
        printf(".");
    }
    fflush(stdout);

    // requeue the frame so it can be filled again
    VmbCaptureFrameQueue(cameraHandle, frame, &FrameCallback);
}

VmbError_t StartContinuousImageAcquisition(AsynchronousGrabOptions* options)
{
    VmbError_t          err                 = VmbErrorSuccess;      // The function result
    VmbUint32_t         nCount              = 0;                    // Number of found cameras
    VmbUint32_t         nFoundCount         = 0;                    // Change of found cameras
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeFull;    // We open the camera with full access

    if(!g_vmbStarted)
    {
        if (mtx_init(&g_frameInfoMutex, mtx_plain) != thrd_success)
        {
            return VmbErrorResources;
        }

        // initialize global state
        g_streaming                = VmbBoolFalse;
        g_acquiring                = VmbBoolFalse;
        g_cameraHandle             = NULL;
        memset(g_frames, 0, sizeof(g_frames));
        g_frameTime                = 0.0;              
        g_frameTimeValid           = VmbBoolFalse;
        g_frameID                  = 0;
        g_frameIdValid             = VmbBoolFalse;

#ifdef WIN32
        LARGE_INTEGER nFrequency;
        QueryPerformanceFrequency(&nFrequency);
        g_frequency = (double)nFrequency.QuadPart;
#endif  //WIN32

        err = VmbStartup(NULL);

        PrintVmbVersion();

        if (VmbErrorSuccess == err)
        {
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
                    if (VmbErrorSuccess == VmbFeatureCommandRun(g_cameraHandle, ADJUST_PACKAGE_SIZE_COMMAND))
                    {

                        VmbBool_t isCommandDone = VmbBoolFalse;
                        do
                        {
                            if (VmbErrorSuccess != VmbFeatureCommandIsDone(g_cameraHandle,
                                                                           ADJUST_PACKAGE_SIZE_COMMAND,
                                                                           &isCommandDone))
                            {
                                break;
                            }
                        } while (VmbBoolFalse == isCommandDone);
                    }

                    if (VmbErrorSuccess == err)
                    {
                        VmbUint32_t payloadSize = 0;

                        // determine the required buffer size
                        err = VmbPayloadSizeGet(g_cameraHandle, &payloadSize);
                        if (VmbErrorSuccess == err)
                        {
                            for(size_t i = 0; i < NUM_FRAMES; i++)
                            {
                                g_frames[i].buffer = malloc((VmbUint32_t)payloadSize);
                                if(NULL == g_frames[i].buffer)
                                {
                                    err = VmbErrorResources;
                                    break;
                                }
                                g_frames[i].bufferSize = payloadSize;
                                g_frames[i].context[FRAME_CONTEXT_OPTIONS_INDEX] = options;

                                // Announce Frame
                                err = VmbFrameAnnounce(g_cameraHandle, &g_frames[i], (VmbUint32_t)sizeof(VmbFrame_t));
                                if (VmbErrorSuccess != err)
                                {
                                    free(g_frames[i].buffer);
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
                                    for(size_t i = 0; i < NUM_FRAMES; i++)
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
                            }
                        }
                    }
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
    }
}

void StopContinuousImageAcquisition()
{
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
                    if (NULL != g_frames[i].buffer)
                    {
                        free(g_frames[i].buffer);
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
}
