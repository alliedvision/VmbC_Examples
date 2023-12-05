/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AsynchronousGrab.h"

#include <VmbCExamplesCommon/ListCameras.h>
#include <VmbCExamplesCommon/PrintVmbVersion.h>
#include <VmbCExamplesCommon/VmbStdatomic.h>
#include <VmbCExamplesCommon/VmbThreads.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <time.h>
#endif

#include <VmbC/VmbC.h>

#include <VmbImageTransform/VmbTransform.h>


#define NUM_FRAMES ((size_t)5)
#define FRAME_CONTEXT_OPTIONS_INDEX ((size_t)0)
#define FRAME_CONTEXT_STREAM_STATISTICS_INDEX ((size_t)1)

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

VmbBool_t               g_bUseAllocAndAnnouce      = VmbBoolFalse;      // Holds the optional decision about frame alloc and announce mode to access it from StopContinuousImageAcquisition()


#ifdef _WIN32
double          g_frequency                = 0.0;              //Frequency of tick counter in _WIN32
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

    // Set the `Data` pointer for the conversion source to the start of the image data in the recorded frame
    sourceImage.Data = pFrame->imageData;

    VmbImage destinationImage = {
        .Size = sizeof(destinationImage) // image transformation functions require the size to specified correctly
    };

    // set destination image info from frame size and string for RGB8 (rgb24)
    result = VmbSetImageInfoFromString("RGB8", width, height, &destinationImage);
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
double GetTime(void)
{
#ifdef _WIN32
    LARGE_INTEGER nCounter;
    QueryPerformanceCounter(&nCounter);
    return ((double)nCounter.QuadPart) / g_frequency;
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return ((double)now.tv_sec) + ((double)now.tv_nsec) / 1000000000.0;
#endif //_WIN32
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

    //
    // Note:    If VmbCaptureEnd is called asynchronously, while this callback is running, VmbCaptureEnd blocks,
    //          until the callback returns.
    //

    AsynchronousGrabOptions const* options = (AsynchronousGrabOptions const*) frame->context[FRAME_CONTEXT_OPTIONS_INDEX];
    StreamStatistics* streamStatistics = (StreamStatistics*) frame->context[FRAME_CONTEXT_STREAM_STATISTICS_INDEX];

    VmbBool_t showFrameInfos = VmbBoolFalse;
    double fps = 0.0;
    VmbBool_t fpsValid = VmbBoolFalse;

    if(FrameInfos_Off != options->frameInfos)
    {
        if(FrameInfos_Show == options->frameInfos)
        {
            showFrameInfos = VmbBoolTrue;
        }

        if (thrd_success == mtx_lock(&g_frameInfoMutex))
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
                        printf("%s error %llu missing frame(s) detected\n", __FUNCTION__, missingFrameCount);
                    }
                }
                g_frameID = frame->frameID;                     // store current frame id to calculate missing frames in the next calls
                g_frameIdValid = VmbBoolTrue;

                double frameTime = GetTime();                   // get current time to calculate frames per second
                if ((g_frameTimeValid)                          // only if the last time was valid
                    && (0 == missingFrameCount))                // and the frame is not missing
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
                streamStatistics->framesMissing += missingFrameCount;
            }
            else
            {
                showFrameInfos = VmbBoolTrue;
                g_frameIdValid = VmbBoolFalse;
                g_frameTimeValid = VmbBoolFalse;
            }

            switch (frame->receiveStatus)
            {
            case VmbFrameStatusComplete:
                streamStatistics->framesComplete++;
                break;
            case VmbFrameStatusIncomplete:
                streamStatistics->framesIncomplete++;
                break;
            case VmbFrameStatusTooSmall:
                streamStatistics->framesTooSmall++;
                break;
            case VmbFrameStatusInvalid:
                streamStatistics->framesInvalid++;
                break;
            }

            mtx_unlock(&g_frameInfoMutex);
        }
        // print the frame infos in case the frame is not complete
        if(VmbFrameStatusComplete != frame->receiveStatus)
        {
            showFrameInfos = VmbBoolTrue;
        }
    }

    if(showFrameInfos)
    {
        VmbBool_t frameIdAvailable = VmbFrameFlagsFrameID & frame->receiveFlags;
        VmbBool_t sizeAvailable = VmbFrameFlagsDimension & frame->receiveFlags;

        const char* status = "?";
        switch(frame->receiveStatus)
        {
        case VmbFrameStatusComplete:
            status = "Complete";
            break;

        case VmbFrameStatusIncomplete:
            status = "Incomplete";
            break;

        case VmbFrameStatusTooSmall:
            status = "Too small";
            break;

        case VmbFrameStatusInvalid:
            status = "Invalid";
            break;

        default:
            status = "?";
            break;
        }

        printf("Frame ID: %4llu Status: %s Size: %ux%u Format: 0x%08X FPS: %.2f\n",
            frameIdAvailable ? frame->frameID : 0,
            status,
            sizeAvailable ? frame->width : 0,
            sizeAvailable ? frame->height : 0,
            frame->pixelFormat,
            fpsValid ? fps : 0.0);
    }

    if (options->showRgbValue && frame->receiveStatus == VmbFrameStatusComplete)
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

VmbError_t StartContinuousImageAcquisition(AsynchronousGrabOptions* options, StreamStatistics* statistics)
{
    VmbError_t          err                 = VmbErrorSuccess;      // The function result
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
        g_bUseAllocAndAnnouce      = options->allocAndAnnounce;

#ifdef _WIN32
        LARGE_INTEGER nFrequency;
        QueryPerformanceFrequency(&nFrequency);
        g_frequency = (double)nFrequency.QuadPart;
#endif  //_WIN32

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
                    {
                        nStreamBufferAlignment = 1;
                    }

                    // We enforce an alignment of sizeof(void*) since aligned_alloc for macOS does not accept 1 as alignment value
                    size_t alignment = (((sizeof(void*)-1) + ((nStreamBufferAlignment > 0) ? nStreamBufferAlignment : 1)) / sizeof(void*)) * sizeof(void*);

                    if (!options->allocAndAnnounce)
                    {
                        printf("StreamBufferAlignment=%lld (%lld)\n", nStreamBufferAlignment, alignment);
                    }

                    if (VmbErrorSuccess == err)
                    {
                        VmbUint32_t payloadSize = 0;

                        // determine the required buffer size
                        err = VmbPayloadSizeGet(g_cameraHandle, &payloadSize);
                        if (VmbErrorSuccess == err)
                        {
                            size_t alignedPayloadSize = (((sizeof(void*)-1) + payloadSize) / sizeof(void*)) * sizeof(void*);

                            for (size_t i = 0; i < NUM_FRAMES; i++)
                            {
                                if (options->allocAndAnnounce)
                                {
                                    g_frames[i].buffer = NULL;
                                }
                                else
                                {
#ifdef _WIN32
                                    g_frames[i].buffer = (unsigned char*)_aligned_malloc(alignedPayloadSize, alignment);
#else
                                    g_frames[i].buffer = (unsigned char*)aligned_alloc(alignment, alignedPayloadSize);
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
    }
}

void StopContinuousImageAcquisition(void)
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
}

