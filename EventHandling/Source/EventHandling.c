/*=============================================================================
  Copyright (C) 2012 - 2019 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        EventHandling.cpp

  Description: The EventHandling example will activate event notification on
               the camera and provide event handling by a callback function.
               See method EventHandling() for the details.

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

#ifdef WIN32
#include <windows.h>
#else
#include <cstring>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#define TRUE true
#define FALSE false
#endif

#include <VimbaC/Include/VimbaC.h>

#include "../../Common/PrintVimbaVersion.h"
#include "../../Common/DiscoverGigECameras.h"
#include "EventHandling.h"


// locking methods for thread synchronization
#ifdef WIN32
HANDLE g_Event = INVALID_HANDLE_VALUE; // global event handle

// Creates a signal that is set later on when the eventhandling is finished.
//
// Returns:
//  VmbBool_t to indicate if the signal creation succeeded.
VmbBool_t CreateSignal()
{
    g_Event = CreateEvent(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        TEXT( "EventHandlingDone" )  // object name
    );

    return g_Event != NULL;
}

void DeleteSignal()
{
    CloseHandle( g_Event );
}

void SetSignal()
{
    SetEvent( g_Event );
}

VmbBool_t WaitForSignal()
{
    return WAIT_OBJECT_0 == WaitForSingleObject( g_Event, INFINITE );
}
#else
pthread_mutex_t g_hMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_Cond = PTHREAD_COND_INITIALIZER;
bool CreateSignal() {return true;}
void DeleteSignal() {}

void SetSignal()
{
    pthread_mutex_lock( &g_hMutex );
    pthread_cond_signal( &g_Cond );
    pthread_mutex_unlock( &g_hMutex );
}

VmbBool_t WaitForSignal()
{
    return pthread_cond_wait( &g_Cond, &g_hMutex ) == 0;
}
#endif


// Callback function to perform the event handling.
// Prints out the name of the triggered event.
//
// For event names and thier associated data fields resort to the GenICam SFNC standard (chapter 15)
// https://www.emva.org/wp-content/uploads/GenICam_SFNC_v2_5.pdf
//
// Attention: This callback is not executed in the main thread.
// Do not spend too much time in this function; it will prevent the feature values
// from being updated from any other thread or the lower-level drivers.
//
// Parameters:
//  [in]    handle    The handle of the camera the event occured on.
//  [in]    name      The name of the event.
//  [in]    context   A pointer to user provided context data provided as parameter for VmbFeatureInvalidationRegister().
//                    Take care of propper locking.
//
void VMB_CALL EventHandlerCB( VmbHandle_t handle, const char* name, void* context )
{
    char featureName[64] = { '\0' };
    VmbInt64_t timeStamp = -1;
    VmbInt64_t frameId = -1;
    VmbError_t err = VmbErrorSuccess;

    printf( "Handling event: %s\n", name );

    // read FrameID (feature name = <EventName>+"FrameID")
    strcat( featureName, name );
    strcat( featureName, "FrameID" );
    err = VmbFeatureIntGet( handle, featureName, &frameId );
    if ( VmbErrorSuccess != err )
    {
        printf( "\tFailed to get FrameID data for event. Error code: %d\n", err );
    }
    else
    {
        printf( "\tFrameID: %lld\n", frameId );
    }

    // read Timestamp (feature name = <EventName>+"Timestamp")
    featureName[0] = '\0';
    strcat( featureName, name );
    strcat( featureName, "Timestamp" );
    err = VmbFeatureIntGet( handle, featureName, &timeStamp );
    if ( VmbErrorSuccess != err )
    {
        printf( "\tFailed to get Timestamp data for event. Error code: %d\n", err);
    }
    else
    {
        printf( "\tTimestamp: %lld\n", timeStamp );
    }

    if ( strcmp( name, "EventAcquisitionEnd" ) == 0 )
    {
        printf( "EventHandler done\n" );

        // signal main thread that event handling is done
        SetSignal();
    }
}


// Checks if all features required to run the example are provided by the camera
//
// Parameters:
//  [in]    cameraHandle     The handle of the camera that should be checked
//
// Returns:
// VmbBool_t true when all required features were found
VmbBool_t CheckRequiredEventFeatures( const VmbHandle_t cameraHandle )
{
    VmbError_t err = VmbErrorSuccess;
    const char* const requiredFeatures[] = {
        "AcquisitionStart",
        "EventAcquisitionEnd",
        "EventAcquisitionEndFrameID",
        "EventAcquisitionEndTimestamp",
        "EventAcquisitionStart",
        "EventAcquisitionStartFrameID",
        "EventAcquisitionStartTimestamp",
        "EventNotification",
        "EventSelector",
        "EventsEnable1",
        "GVSPAdjustPacketSize",
        "PayloadSize",
        "UserSetLoad",
        "UserSetSelector"
    };
    VmbFeatureInfo_t featureInfo;
    VmbBool_t featuresOk = VmbBoolTrue;
    unsigned int i = 0;
    const unsigned int n = sizeof( requiredFeatures ) / sizeof( char* );
    for ( ; i < n; ++i )
    {
        err = VmbFeatureInfoQuery( cameraHandle, requiredFeatures[i], &featureInfo, sizeof( VmbFeatureInfo_t ) );

        if ( VmbErrorNotFound == err )
        {
            printf( "Feature missing: %s\n\n", requiredFeatures[i] );
            featuresOk = VmbBoolFalse;
        }
        else if ( VmbErrorSuccess != err )
        {
            printf( "Failed to query feature %s. Error: %d\n\n", requiredFeatures[i], err );
            featuresOk = VmbBoolFalse;
        }
    }
    return featuresOk;
}


// Captures a frame
//
// Parameters:
//  [in]    cameraHandle     The handle of the camera
void CaptureFrame( const VmbHandle_t cameraHandle )
{
    VmbFrame_t          frame;                    // The frame we capture
    VmbInt64_t          nPayloadSize = 0;       // The size of one frame
    const VmbUint32_t   nTimeout = 2000;        // Timeout for Grab
    VmbError_t err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );
    if (VmbErrorSuccess != err)
    {
        printf("Could not get payload size. Error code: %d\n", err);
        return;
    }

    frame.buffer = (unsigned char*)malloc( (VmbUint32_t)nPayloadSize );
    if (NULL == frame.buffer)
    {
        printf("Failed to allocate frame buffer.");
        return;
    }

    frame.bufferSize = (VmbUint32_t)nPayloadSize;

    // Announce Frame
    err = VmbFrameAnnounce(cameraHandle, &frame, (VmbUint32_t)sizeof(VmbFrame_t));
    if (VmbErrorSuccess == err)
    {
        // Start Capture Engine
        err = VmbCaptureStart(cameraHandle);
        if (VmbErrorSuccess == err)
        {
            // Queue Frame
            err = VmbCaptureFrameQueue(cameraHandle, &frame, NULL);
            if (VmbErrorSuccess == err)
            {
                // Start Acquisition
                err = VmbFeatureCommandRun(cameraHandle, "AcquisitionStart");
                if (VmbErrorSuccess == err)
                {
                    // Capture one frame synchronously
                    err = VmbCaptureFrameWait(cameraHandle, &frame, nTimeout);
                    if (VmbErrorSuccess == err)
                    {
                        // Evaluate recieved frame status
                        if (VmbFrameStatusComplete == frame.receiveStatus)
                        {
                            printf("Frame successfully received.\n");
                        }
                        else
                        {
                            printf("Frame not successfully received. Error code: %d\n", frame.receiveStatus);
                        }
                    }
                    else
                    {
                        printf("Could not capture frame. Error code: %d\n", err);
                    }

                    // Stop Acquisition
                    err = VmbFeatureCommandRun(cameraHandle, "AcquisitionStop");
                    if (VmbErrorSuccess != err)
                    {
                        printf("Could not stop acquisition. Error code: %d\n", err);
                    }
                }
                else
                {
                    printf("Could not start acquisition. Error code: %d\n", err);
                }
            }
            else
            {
                printf("Could not queue frame. Error code: %d\n", err);
            }

            // Stop Capture Engine
            err = VmbCaptureEnd(cameraHandle);
            if (VmbErrorSuccess != err)
            {
                printf("Could not end capture. Error code: %d\n", err);
            }
        }
        else
        {
            printf("Could not start capture. Error code: %d\n", err);
        }

        // Revoke frame
        err = VmbFrameRevoke(cameraHandle, &frame);
        if (VmbErrorSuccess != err)
        {
            printf("Could not revoke frame. Error code: %d\n", err);
        }
    }
    else
    {
        printf("Could not announce frame. Error code: %d\n", err);
    }

    free(frame.buffer);
    frame.buffer = NULL;
}


// Loads the default user set settings in the camear
//
// Parameters:
//  [in]    cameraHandle     The handle of the camera
// Returns:
//  VmbBool_t: TRUE if default settings are loaded successfully.
VmbBool_t LoadDefaultSettings( const VmbHandle_t cameraHandle )
{
    VmbError_t err = VmbFeatureEnumSet( cameraHandle, "UserSetSelector", "Default" );
    if ( VmbErrorSuccess != err )
    {
        printf( "Failed to select default user set.\n\n" );
        return FALSE;
    }
    else
    {
        err = VmbFeatureCommandRun(cameraHandle, "UserSetLoad");
        if (VmbErrorSuccess != err)
        {
            printf("Failed to run UserSetLoad command to load default camera settings.\n\n");
            return FALSE;
        }
    }
    return TRUE;
}


// Set the GeV packet size to the highest possible value
// (In this example we do not test whether this cam actually is a GigE cam)
// Parameters:
//  [in]    cameraHandle     The handle of the camera
// Returns:
//  VmbBool_t: TRUE if GVSP packet size was adjusted successfully.
VmbBool_t AdjustGVSPPacketSize( const VmbHandle_t cameraHandle )
{
    VmbBool_t bIsCommandDone = VmbBoolFalse; // Indicates if the command has finished execution
    VmbError_t err = VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" );
    if (VmbErrorSuccess != err)
    {
        printf("Could not adjust packet size. Error code: %d\n", err);
        return FALSE;
    }

    do
    {
        err = VmbFeatureCommandIsDone(cameraHandle, "GVSPAdjustPacketSize", &bIsCommandDone);
        if ( VmbErrorSuccess !=  err )
        {
            printf("Could not get GVSPAdjustPacketSize command status. Error code: %d\n", err);
            return FALSE;
        }
    } while ( VmbBoolFalse == bIsCommandDone );

    return TRUE;
}


// Finds the first listed camera
// Returns:
//  const char* handle to the first camera, NULL if no camera was found.
const char* FindFirstCamera()
{
    VmbError_t          err = VmbErrorSuccess;  // Error code for most of the subsequent calls
    VmbCameraInfo_t*    pCameras = NULL;        // A list of camera details
    VmbUint32_t         nCount = 0;             // Number of found cameras
    VmbUint32_t         nFoundCount = 0;        // Change of found cameras
    const char*         pCameraID = NULL;       // The ID of the first discovered camera

    // Get the amount of known cameras
    err = VmbCamerasList(NULL, 0, &nCount, sizeof * pCameras);
    if (VmbErrorSuccess != err && 0 >= nCount)
    {
        printf("Could not list cameras or no cameras present. Error code: %d\n", err);
        return NULL;
    }
    pCameras = (VmbCameraInfo_t*)malloc(nCount * sizeof(*pCameras));
    if (NULL == pCameras)
    {
        printf("Could not allocate camera list.\n");
        return NULL;
    }

    // Actually query all static details of all known cameras without having to open the cameras
    // If a new camera was connected since we queried the amount of cameras (nFoundCount > nCount) we can ignore that one
    err = VmbCamerasList(pCameras, nCount, &nFoundCount, sizeof * pCameras);
    if (VmbErrorSuccess != err && VmbErrorMoreData != err)
    {
        printf("Could not list cameras. Error code: %d\n", err);
    }
    else
    {
        // Use the first camera
        if (nFoundCount != 0)
        {
            pCameraID = pCameras[0].cameraIdString;
        }
        else
        {
            pCameraID = NULL;
            err = VmbErrorNotFound;
            printf("Camera lost.\n");
        }
    }

    free(pCameras);
    pCameras = NULL;

    return pCameraID;
}


// This method demonstrates how to enabel events and register an event handler for them.
// In detail the performed steps are:
// - Checks wether the camrea provides all features required to run the example
// - Enable events for AcquisitionStart and AcquisitionEnd
// - Register an event handler callback for AcquisitionStart and AcquisitionEnd
// - Sets the maximum possible Ethernet packet size
// - Captures a frame to trigger the registered events
// - Wait for the event handler to finish before closing the camera and shutting down Vimba
//
// Parameters:
//  [in]    cameraHandle: The handle of the camera to work on.
//
// Returns:
//  An API status code
VmbError_t EventHandling( const VmbHandle_t cameraHandle)
{
    VmbError_t          err = VmbErrorSuccess;  // Error code for most of the subsequent calls
    const char* pPixelFormat = NULL;            // The pixel format we use for acquisition

    if (FALSE == CheckRequiredEventFeatures(cameraHandle))
    {
        return VmbErrorNotFound;
    }

    if (FALSE == LoadDefaultSettings(cameraHandle))
    {
        return VmbErrorOther;
    }

    // disable all events by bitmask
    err = VmbFeatureIntSet(cameraHandle, "EventsEnable1", 0);
    if (VmbErrorSuccess != err)
    {
        printf("Failed to clear enabled events bitmask.\n\n");
        return err;
    }

    // Select "AcquisitionStart" event ...
    err = VmbFeatureEnumSet(cameraHandle, "EventSelector", "AcquisitionStart");
    if (VmbErrorSuccess != err)
    {
        printf("Failed to select AcquisitionStart event.\n\n");
        return err;
    }

    // ... and switch on notification for selected event
    err = VmbFeatureEnumSet(cameraHandle, "EventNotification", "On");
    if (VmbErrorSuccess != err)
    {
        printf("Failed to enable event notification.\n\n");
        return err;
    }

    // Select "AcquisitionEnd" event ...
    err = VmbFeatureEnumSet(cameraHandle, "EventSelector", "AcquisitionEnd");
    if (VmbErrorSuccess != err)
    {
        printf("Failed to select AcquisitionStart event.\n\n");
        return err;
    }

    // ... and switch on notification for selected event
    err = VmbFeatureEnumSet(cameraHandle, "EventNotification", "On");
    if (VmbErrorSuccess != err)
    {
        printf("Failed to enable event notification.\n\n");
        return err;
    }

    // Register the callback for the events
    err = VmbFeatureInvalidationRegister(cameraHandle, "EventAcquisitionStart", EventHandlerCB, NULL);
    if (VmbErrorSuccess != err)
    {
        printf("Failed to set callback for feature invalidation.\n\n");
        return err;
    }

    err = VmbFeatureInvalidationRegister(cameraHandle, "EventAcquisitionEnd", EventHandlerCB, NULL);
    if (VmbErrorSuccess != err)
    {
        printf("Failed to set callback for feature invalidation.\n\n");
        return err;
    }

    if (FALSE == AdjustGVSPPacketSize(cameraHandle))
    {
        printf("Failed to adjust GVSP packet size.\n");
        return VmbErrorOther;
    }

    // Create a signal for thread synchronization
    if (FALSE == CreateSignal())
    {
        printf("Failed to CreateEvent.\n");
        return VmbErrorResources;
    }

    // Capture a frame to trigger events
    CaptureFrame(cameraHandle);

    // Wait until event handling thread has finished before shutting down Vimba and terminating the app
    printf("Main thread waiting for event handler to finish...\n");
    WaitForSignal();
    DeleteSignal();

    return VmbErrorSuccess;
}


// Opens the given camera, if no camera ID was provided, the first camera found will be used
// run EventHandling() example code on the opened camera
//
// Parameters:
//  [in]    pCameraID           The ID of the camera to work on. Can be NULL.
//
// Returns:
//  An API status code
VmbError_t OpenCamera( const char* pCameraID )
{
    VmbError_t              err = VmbErrorSuccess;  // Error code for most of the subsequent calls
    const VmbAccessMode_t   cameraAccessMode = VmbAccessModeFull; // We open the camera with full access
    VmbHandle_t             cameraHandle = NULL;    // A handle to our camera
    const char*             pPixelFormat = NULL;            // The pixel format we use for acquisition

    // Is Vimba connected to a GigE transport layer?
    err = DiscoverGigECameras();
    if (VmbErrorSuccess != err)
    {
        printf("Failed to discover GigE cameras. Error code: %d\n", err);
        return err;
    }

    // If no camera ID was provided use the first camera found
    if (NULL == pCameraID)
    {
        pCameraID = FindFirstCamera();
        if (NULL == pCameraID)
        {
            printf("Failed to find camera.\n");
            return VmbErrorNotFound;
        }
    }

    // Open camera
    err = VmbCameraOpen(pCameraID, cameraAccessMode, &cameraHandle);
    if (VmbErrorSuccess == err)
    {
        printf("Camera ID: %s\n\n", pCameraID);

        err = EventHandling(cameraHandle);
        if (VmbErrorSuccess != err)
        {
            printf("Failed to perform event handling.\n");
        }

        err = VmbCameraClose(cameraHandle);
        if (VmbErrorSuccess != err)
        {
            printf("Could not close camera. Error code: %d\n", err);
        }
    }
    else
    {
        printf("Could not open camera. Error code: %d\n", err);
    }

    return err;
}


// Runs Vimba system
// passes control to OpenCamera()
//
// Parameters:
//  [in]    pCameraID           The ID of the camera to use later on. Can be NULL.
//
// Returns:
//  An API status code
VmbError_t RunVimbaSystem( const char* const pCameraID )
{
    VmbError_t err = VmbStartup(); // Initialize the Vimba API

    PrintVimbaVersion();

    if (VmbErrorSuccess != err)
    {
        printf("Could not start system. Error code: %d\n", err);
        return err;
    }

    err = OpenCamera(pCameraID);

    VmbShutdown();

    return err;
}
