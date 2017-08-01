/*=============================================================================
  Copyright (C) 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ActionCommands.cpp

  Description: see header file for description


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

#include <ActionCommands.h>

// socket library to check IP addresses
#ifdef _WIN32
    #include <conio.h>
    #include <WinSock.h>
#else
    #include <arpa/inet.h>
#endif

// number of frame buffers to be used by example
#define NUM_FRAMES 3

//
// Frame callback, which will be called when Vimba receives a frame from the camera
//
// Parameters:
//  [in]    aCamera     Camera handle
//  [in]    aFrame      Received frame
//
void VMB_CALL FrameCallback( const VmbHandle_t aCamera, VmbFrame_t* aFrame )
{
    VmbError_t lError = VmbErrorSuccess;

    // proceed only if given pointer are valid
    if( (NULL != aCamera) && (NULL != aFrame) )
    {
        // check if received frame is complete
        if( VmbFrameStatusComplete == aFrame->receiveStatus )
        {
            printf( "......Frame has been received\n" );
        }

        // re-queue received frame (buffer) to the capture queue (output buffer queue of GigE TL)
        lError = VmbCaptureFrameQueue( aCamera, aFrame, &FrameCallback );
        if( VmbErrorSuccess != lError )
        {
            printf( "[F]...Could not re-queue received buffer\n" );
        }
    }
}

//
// Called when any failure occurs within the example.
// Ensures to close camera and shutdown Vimba
//
// Parameters:
//  [in]    aCamera     Camera handle
//
void FailureShutdown( VmbHandle_t aCamera )
{
    VmbError_t lError = VmbErrorSuccess;

    // close camera
    lError = VmbCameraClose( aCamera );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not close camera. Reason: %i", lError );
    }

    printf( "......Camera has been closed\n" );

    // shutdown Vimba
    VmbShutdown();

    printf( "......Vimba has been stopped\n" );
}

//
// Convert given string to IP address,
// using respective socket library (Winsock/Arpa)
//
// Parameters:
//  [in]    aString     String to be converted
//  [out]   aIPAddress  Decimal representation of given IP address string
//
VmbError_t ConvertStringToIPAddress( const char* aString, VmbUint32_t* aIPAddress )
{
    VmbError_t  lError  = VmbErrorSuccess;
    VmbUint32_t lIP     = 0;

    // check parameter
    if( (NULL == aString) || (NULL == aIPAddress) )
    {
        printf( "[F]...Invalid parameter given.\n" );
        return VmbErrorBadParameter;
    }

    // convert given string to IP struct
    lIP = inet_addr( aString );
    if( -1 == lIP )
    {
        lError = VmbErrorInvalidValue;
    }
    else
    {
        *aIPAddress = lIP;
    }

    return lError;
}

//
// Start Vimba and open camera with given string
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [out]   aHandle     Handle to camera, if opened successfully
//
VmbError_t PrepareCamera( const char* aCamera, VmbHandle_t* aHandle )
{
    VmbError_t  lError          = VmbErrorSuccess;
    VmbHandle_t lCameraHandle   = NULL;

    // check parameter
    if( (NULL == aCamera) || (NULL == aHandle) )
    {
        printf( "[F]...Invalid parameter given.\n" );
        return VmbErrorBadParameter;
    }

    // start Vimba
    lError = VmbStartup();
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not start Vimba API. Reason: %i\n", lError );
        return lError;
    }

    printf( "......Vimba has been started\n" );

    // -check if GigE TL is present
    // -set API waiting time for discovery response to 250ms
    // -send discovery packet once, to all connected cameras
    lError = DiscoverGigECameras();
    if( VmbErrorSuccess != lError )
    {
        VmbShutdown();
        return lError;
    }

    printf( "......GigE Devices have been discovered\n" );

    // open camera with given string (could be device ID or IP address)
    lError = VmbCameraOpen( aCamera, VmbAccessModeFull, &lCameraHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not open camera '%s'. Reason: %i\n", aCamera, lError );
        VmbShutdown();
        return lError;
    }

    printf( "......Camera has been opened (%s)\n", aCamera );

    // return camera handle
    *aHandle = lCameraHandle;

    return lError;
}

//
// Close camera and shutdown Vimba
//
// Parameters:
//  [in]    aHandle     Handle to camera
//
VmbError_t StopCamera( VmbHandle_t aHandle )
{
    VmbError_t lError = VmbErrorSuccess;

    // close camera
    lError = VmbCameraClose( aHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not close camera. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Camera has been closed\n" );

    // shutdown Vimba
    VmbShutdown();

    printf( "......Vimba has been stopped\n" );

    return lError;
}

//
// Prepare trigger settings for given camera
//
// Parameters:
//  [in]    aHandle     Handle to camera
//
VmbError_t PrepareTrigger( VmbHandle_t aHandle )
{
    VmbError_t lError = VmbErrorSuccess;

    // check parameter
    if( NULL == aHandle )
    {
        printf( "[F]...Invalid parameter given.\n" );
        return VmbErrorBadParameter;
    }

    // select FrameStart trigger via TriggerSelector feature
    lError = VmbFeatureEnumSet( aHandle, "TriggerSelector", "FrameStart" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set TriggerSelector to FrameStart. Reason: %i", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // set trigger source to Action0
    lError = VmbFeatureEnumSet( aHandle, "TriggerSource", "Action0" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set TriggerSource to 'Action0'. Reason: %i\nProbably this camera does not support Action Commands.\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // enable trigger
    lError = VmbFeatureEnumSet( aHandle, "TriggerMode", "On" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not enable TriggerMode for FrameStart. Reason: %i", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Trigger FrameStart has been activated and set to Action0\n" );

    return lError;
}

//
// Set Action Command information to given handle.
// This could be a handle to Vimba system, interface
// or a camera handle
//
// Parameters:
//  [in]    aHandle     Handle to either Vimba system, interface or camera
//  [in]    aCommand    Action Command struct (device key, group key, group mask)
//
VmbError_t PrepareActionCommand( VmbHandle_t aHandle, tActionCommand* aCommand )
{
    VmbError_t lError = VmbErrorSuccess;

    // check parameter
    if( (NULL == aHandle) || (NULL == aCommand) )
    {
        printf( "[F]...Invalid parameter given.\n" );
        return VmbErrorBadParameter;
    }

    // set device key
    lError = VmbFeatureIntSet( aHandle, "ActionDeviceKey", aCommand->mDeviceKey );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set ActionDeviceKey. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // set group key
    lError = VmbFeatureIntSet( aHandle, "ActionGroupKey", aCommand->mGroupKey );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set ActionGroupKey. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // set group mask
    lError = VmbFeatureIntSet( aHandle, "ActionGroupMask", aCommand->mGroupMask );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set ActionGroupMask. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Action Command has been set (%i,%i,%i)\n", aCommand->mDeviceKey, aCommand->mGroupKey, aCommand->mGroupMask );

    return lError;
}

//
// Prepare streaming settings in the camera,
// like allocating the buffers, start capture engine, etc.
//
// Parameters:
//  [in]    aHandle     Handle to camera
//  [in]    aFrameArray Array of frames to be used for streaming
//
VmbError_t PrepareStreaming( VmbHandle_t aHandle, VmbFrame_t* aFrameArray )
{
    VmbError_t  lError          = VmbErrorSuccess;
    VmbBool_t   lFlag           = VmbBoolFalse;
    VmbInt64_t  lGVSPSize       = 0;
    VmbInt64_t  lPayloadSize    = 0;
    int         lIter           = 0;

    // check parameter
    if( NULL == aHandle )
    {
        printf( "[F]...Invalid parameter given.\n" );
        return VmbErrorBadParameter;
    }

    // set GVSP packet size to max value (MTU)
    // and wait until command is done
    lError = VmbFeatureCommandRun( aHandle, "GVSPAdjustPacketSize" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not set GVSP packet size. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }
    else
    {
        do
        {
            // check if operation is done
            lError = VmbFeatureCommandIsDone( aHandle, "GVSPAdjustPacketSize", &lFlag );
            if( VmbErrorSuccess != lError )
            {
                break;
            }

        } while( VmbBoolFalse == lFlag );
    }

    // get GVSP packet size, which was actually set in the camera
    lError = VmbFeatureIntGet( aHandle, "GVSPPacketSize", &lGVSPSize );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not get GVSP packet size. Reason: %i\n", lError );
    }

    printf( "......GVSP packet size has been set to maximum (%i)\n", (int)lGVSPSize );

    // get camera payload size (necessary for allocating the buffer memory)
    lError = VmbFeatureIntGet( aHandle, "PayloadSize", &lPayloadSize );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not get payload size. Reason: %i", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Camera has payload size of '%i'\n", (int)lPayloadSize );

    // initialize frame array with 0
    memset( aFrameArray, 0, sizeof(aFrameArray) );

    // allocate buffer memory for each frame
    for( lIter = 0; lIter < NUM_FRAMES; ++lIter )
    {
        // allocate buffer
        aFrameArray[lIter].buffer = (unsigned char*)malloc((VmbUint32_t)lPayloadSize);
        if( NULL == aFrameArray[lIter].buffer )
        {
            lError = VmbErrorResources;
            break;
        }

        // set buffer size
        aFrameArray[lIter].bufferSize = (VmbUint32_t)lPayloadSize;
    }

    // in case any failure occured, free allocated space
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Failure during memory allocation for buffers (%i/%i). Reason: %i", lIter, NUM_FRAMES, lError );

        for( lIter = 0; lIter < NUM_FRAMES; ++lIter )
        {
            free( aFrameArray[lIter].buffer );
            memset( aFrameArray, 0, sizeof(aFrameArray) );
        }

        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Allocated memory for frame buffers\n" );

    // announce frame buffers (move them to buffer input pool of GigE TL)
    for( lIter = 0; lIter < NUM_FRAMES; ++lIter )
    {
        lError = VmbFrameAnnounce( aHandle, &aFrameArray[lIter], sizeof(VmbFrame_t) );
        if( VmbErrorSuccess != lError )
        {
            break;
        }
    }

    // in case any failure occured, revoke all announced frames
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Failure during announcing frame buffers (%i/%i). Reason: %i", lIter, NUM_FRAMES, lError );

        lError = VmbFrameRevokeAll( aHandle );
        if( VmbErrorSuccess != lError )
        {
            printf( "[F]...Could not revoke all frame buffers. Reason: %i", lError );
        }

        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Buffers have been announced and moved to input pool\n" );

    // start capture engine
    lError = VmbCaptureStart( aHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not start capture engine. Reason: %i", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Capture engine has been started\n" );

    // move announced frames to output buffer queue
    for( lIter = 0; lIter < NUM_FRAMES; ++lIter )
    {
        lError = VmbCaptureFrameQueue( aHandle, &aFrameArray[lIter], &FrameCallback );
        if( VmbErrorSuccess != lError )
        {
            printf( "[F]...Could not queue frame to output buffer queue. Reason: %i\n", lError );
            FailureShutdown( aHandle );
            return lError;
        }
    }

    printf( "......Buffers have been moved to output queue\n" );

    // start acquisition in the camera
    lError = VmbFeatureCommandRun( aHandle, "AcquisitionStart" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not start camera acquisition. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    printf( "......Camera acquisition has been started\n" );

    return lError;
}

//
// End streaming
//
// Parameters:
//  [in]    aHandle     Handle to camera
//  [in]    aFrameArray Array of frames to be used for streaming
//
VmbError_t StopStreaming( VmbHandle_t aHandle, VmbFrame_t* aFrameArray )
{
    VmbError_t  lError  = VmbErrorSuccess;
    int         lIter   = 0;

    // call acquisition stop feature in camera
    lError = VmbFeatureCommandRun( aHandle, "AcquisitionStop" );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not run AcquisitionStop feature. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // stop capture engine
    lError = VmbCaptureEnd( aHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not stop capture engine. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // flush buffer output queue
    lError = VmbCaptureQueueFlush( aHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not flush output buffer queue. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // revoke all announced buffers
    lError = VmbFrameRevokeAll( aHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not flush input buffer pool. Reason: %i\n", lError );
        FailureShutdown( aHandle );
        return lError;
    }

    // free allocated space for buffers
    for( lIter = 0; lIter < NUM_FRAMES; ++lIter )
    {
        free( aFrameArray[lIter].buffer );
        memset( aFrameArray, 0, sizeof(aFrameArray) );
    }

    return lError;
}

//
// Send Action Command  on system level.
// This command will be broadcasted on all network interfaces.
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [in]    aCommand    Action Command to be set in the camera
//
VmbError_t SendActionCommandOnAllInterfaces( const char* aCamera, tActionCommand aCommand )
{
    VmbError_t  lError          = VmbErrorSuccess;
    VmbBool_t   lFlag           = VmbBoolFalse;
    VmbHandle_t lCameraHandle   = NULL;
    VmbUint32_t lIP             = 0;
    int         lKey            = 0;
    VmbFrame_t  lFrames[NUM_FRAMES];

    // check parameter
    if( NULL == aCamera )
    {
        printf( "[F]...Invalid parameter given!\n" );
        return VmbErrorBadParameter;
    }

    // -start Vimba
    // -discover GigE devices
    // -open camera in full access mode and get handle
    lError = PrepareCamera( aCamera, &lCameraHandle );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -select FrameStart trigger feature
    // -set source to Action0
    // -enable trigger
    lError = PrepareTrigger( lCameraHandle );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // Set Action Command to camera
    // -set device key
    // -set group key
    // -set group mask
    lError = PrepareActionCommand( lCameraHandle, &aCommand );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -adjust GVSP packet size
    // -get payload size
    // -allocate memory for frame buffers
    // -announce frames and move them to buffer input pool
    // -start capture engine
    // -move frames to capture queue (buffer output queue)
    // -call start acquisition feature in the camera
    lError = PrepareStreaming( lCameraHandle, lFrames );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // determine if Action Command shall be send as uni- or broadcast
    // if IP address was given, send it as unicast
    lError = ConvertStringToIPAddress( aCamera, &lIP );
    if( VmbErrorSuccess == lError )
    {
        // set IP address to Vimba
        lError = VmbFeatureIntSet( gVimbaHandle, "GevActionDestinationIPAddress", lIP );
        if( VmbErrorSuccess != lError )
        {
            printf( "[F]...Could not set IP address '%s' to Vimba. Reason: %i\n", aCamera, lError );
        }

        printf( "......Action Command will be send as unicast to IP '%s (%i)'\n", aCamera, lIP );
    }

    #ifdef _WIN32
        printf( "\n<< Please hit 'a' to send prepared Action Command. To stop example hit 'q' >>\n\n" );
    #else
        printf( "\n<< Please enter 'a' and return to send prepared Action Command. To stop example enter 'q' and return >>\n\n" );
    #endif

    // repeat this until user hits ESC
    do
    {
        // wait for user input
        #ifdef _WIN32
            lKey = _getch();
        #else
            lKey = getchar();
        #endif

        if( 97 == lKey )
        {
            // set Action Command to Vimba system
            // -device key
            // -group key
            // -group mask
            lError = PrepareActionCommand( gVimbaHandle, &aCommand );
            if( VmbErrorSuccess == lError )
            {
                // send Action Command by calling command feature
                lError = VmbFeatureCommandRun( gVimbaHandle, "ActionCommand" );
                if( VmbErrorSuccess != lError )
                {
                    printf( "[F]...Could not send Action Command. Reason: %i\n", lError );
                    FailureShutdown( lCameraHandle );
                    return lError;
                }

                printf( "......Action Command has been sent\n" );
            }
            else
            {
                return lError;
            }
        }

    } while( 113 != lKey );

    // stop streaming
    lError = StopStreaming( lCameraHandle, lFrames );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -close camera
    // -shutdown Vimba
    lError = StopCamera( lCameraHandle );

    return lError;
}

//
// Send Action Command on interface level.
// This command will be broadcasted on given network interface.
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [in]    aInterface  The network interface on which the Action Command
//                      will be sent out
//  [in]    aCommand    Action Command to be used by Vimba and camera
//
VmbError_t SendActionCommandOnInterface( const char* aCamera, const char* aInterface, tActionCommand aCommand )
{
    VmbError_t          lError              = VmbErrorSuccess;
    VmbHandle_t         lCameraHandle       = NULL;
    VmbInterfaceInfo_t* lInterfaces         = NULL;
    VmbUint32_t         lCount              = 0;
    VmbHandle_t         lInterfaceHandle    = NULL;
    VmbBool_t           lFound              = VmbBoolFalse;
    int                 lInterfaceIndex     = 0;
    VmbUint32_t         lIP                 = 0;
    int                 lKey                = 0;
    VmbFrame_t          lFrames[NUM_FRAMES];
    int                 lIter               = 0;

    // check parameter
    if( NULL == aCamera || NULL == aInterface )
    {
        printf( "[F]...Invalid parameter given!\n" );
        return VmbErrorBadParameter;
    }

    // -start Vimba
    // -discover GigE devices
    // -open camera in full access mode and get handle
    lError = PrepareCamera( aCamera, &lCameraHandle );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -select FrameStart trigger feature
    // -set source to Action0
    // -enable trigger
    lError = PrepareTrigger( lCameraHandle );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // Set Action Command to camera
    // -set device key
    // -set group key
    // -set group mask
    lError = PrepareActionCommand( lCameraHandle, &aCommand );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -adjust GVSP packet size
    // -get payload size
    // -allocate memory for frame buffers
    // -announce frames and move them to buffer input pool
    // -start capture engine
    // -move frames to capture queue (buffer output queue)
    // -call start acquisition feature in the camera
    lError = PrepareStreaming( lCameraHandle, lFrames );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // get number of available interfaces
    lError = VmbInterfacesList( NULL, lCount, &lCount, 0 );
    if( (VmbErrorSuccess != lError) || (0 == lCount) )
    {
        printf( "[F]...Could not retrieve number of interfaces. Reason: %i\n", lError );
        FailureShutdown( lCameraHandle );
        return lError;
    }

    // allocate space for interface info list
    lInterfaces = (VmbInterfaceInfo_t*)malloc( lCount * sizeof(VmbInterfaceInfo_t) );
    if( NULL == lInterfaces )
    {
        printf( "[F]...Could not allocate space for interface info list.\n" );
        FailureShutdown( lCameraHandle );
        return lError;
    }

    // get interface info list
    lError = VmbInterfacesList( lInterfaces, lCount, &lCount, sizeof(*lInterfaces) );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not retrieve interface info list. Reason: %i\n", lError );
        FailureShutdown( lCameraHandle );
        return lError;
    }

    // print interface list
    printf( "......following interfaces were found:\n" );
    for( lIter = 0; lIter<lCount; ++lIter )
    {
        printf( "..........[%i] %s\n", lIter, lInterfaces[lIter].interfaceIdString );

        // compare given interface ID with current one
        if( 0 == strcmp(aInterface, lInterfaces[lIter].interfaceIdString) )
        {
            // if interface ID matches, keep index
            lFound = VmbBoolTrue;
            lInterfaceIndex = lIter;
        }
    }

    // if no interface with given ID was found, return
    if( VmbBoolFalse == lFound )
    {
        printf( "[F]...Given interface with ID '%s' was not found!\n", aInterface );
        FailureShutdown( lCameraHandle );
        return VmbErrorBadParameter;
    }

    // check interface type
    if( VmbInterfaceEthernet != lInterfaces[lInterfaceIndex].interfaceType )
    {
        printf( "[F]...Selected interface is non-GigE interface!\n" );
        FailureShutdown( lCameraHandle );
        return VmbErrorBadParameter;
    }

    // open network interface with given index
    lError = VmbInterfaceOpen( aInterface, &lInterfaceHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not open interface with ID '%s'. Reason: %i\n", aInterface, lError );
        FailureShutdown( lCameraHandle );
        return lError;
    }

    printf( "......Interface '%s' has been opened.\n", aInterface );

    // determine if Action Command shall be send as uni- or broadcast
    // if IP address was given, send it as unicast
    lError = ConvertStringToIPAddress( aCamera, &lIP );
    if( VmbErrorSuccess == lError )
    {
        // set IP address to Vimba
        lError = VmbFeatureIntSet( gVimbaHandle, "GevActionDestinationIPAddress", lIP );
        if( VmbErrorSuccess != lError )
        {
            printf( "[F]...Could not set IP address '%s' to Vimba. Reason: %i\n", aCamera, lError );
        }

        printf( "......Action Command will be send as unicast to IP '%s (%i)'\n", aCamera, lIP );
    }

    #ifdef _WIN32
        printf( "\n<< Please hit 'a' to send prepared Action Command. To stop example hit 'q' >>\n\n" );
    #else
        printf( "\n<< Please enter 'a' and return to send prepared Action Command. To stop example enter 'q' and return >>\n\n" );
    #endif

    // repeat this until user hits ESC
    do
    {
        // wait for user input
        #ifdef _WIN32
            lKey = _getch();
        #else
            lKey = getchar();
        #endif

        if( 97 == lKey )
        {
            // set Action Command to Vimba interface
            // -device key
            // -group key
            // -group mask
            lError = PrepareActionCommand( lInterfaceHandle, &aCommand );
            if( VmbErrorSuccess == lError )
            {
                // send Action Command by calling command feature
                lError = VmbFeatureCommandRun( lInterfaceHandle, "ActionCommand" );
                if( VmbErrorSuccess != lError )
                {
                    printf( "[F]...Could not send Action Command. Reason: %i\n", lError );
                    FailureShutdown( lCameraHandle );
                    return lError;
                }

                printf( "......Action Command has been sent\n" );
            }
            else
            {
                return lError;
            }
        }

    } while( 113 != lKey );

    // close interface
    lError = VmbInterfaceClose( lInterfaceHandle );
    if( VmbErrorSuccess != lError )
    {
        printf( "[F]...Could not close interface. Reason: %i\n", lError );
        FailureShutdown( lCameraHandle );
        return lError;
    }

    // stop streaming
    lError = StopStreaming( lCameraHandle, lFrames );
    if( VmbErrorSuccess != lError )
    {
        return lError;
    }

    // -close camera
    // -shutdown Vimba
    lError = StopCamera( lCameraHandle );

    return lError;
}
