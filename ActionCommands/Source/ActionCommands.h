/*=============================================================================
  Copyright (C) 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ActionCommands.h

  Description: This example will create an Action Command and send it to any
               camera, given by parameter. The following can be set up with
               parameters as well:
                -send Action Command as broadcast on specific network interface
                -send Action Command as broadcast to all network interfaces
                -send Action Command to specific IP address (unicast)


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

#ifndef ACTION_COMMANDS_H_
#define ACTION_COMMANDS_H_

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/DiscoverGigECameras.h>

// struct representing an Action Command
typedef struct tActionCommand
{
    VmbUint32_t     mDeviceKey;
    VmbUint32_t     mGroupKey;
    VmbUint32_t     mGroupMask;

} tActionCommand;

//
// Called when any failure occurs within the example.
// Ensures to close camera and shutdown Vimba
//
// Parameters:
//  [in]    aCamera     Camera handle
//
void FailureShutdown( VmbHandle_t aCamera );

//
// Convert given string to IP address,
// using respective socket library (Winsock/Arpa)
//
// Parameters:
//  [in]    aString     String to be converted
//  [out]   aIPAddress  Decimal representation of given IP address string
//
VmbError_t ConvertStringToIPAddress( const char* aString, VmbUint32_t* aIPAddress );

//
// Start Vimba and open camera with given string
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [out]   aHandle     Handle to camera, if opened successfully
//
VmbError_t PrepareCamera( const char* aCamera, VmbHandle_t* aHandle );

//
// Close camera and shutdown Vimba
//
// Parameters:
//  [in]    aHandle     Handle to camera
//
VmbError_t StopCamera( VmbHandle_t aHandle );

//
// Prepare trigger settings for given camera
//
// Parameters:
//  [in]    aHandle     Handle to camera
//
VmbError_t PrepareTrigger( VmbHandle_t aHandle );

//
// Set Action Command information to given handle.
// This could be a handle to Vimba system, interface
// or a camera handle
//
// Parameters:
//  [in]    aHandle     Handle to either Vimba system, interface or camera
//  [in]    aCommand    Action Command struct (device key, group key, group mask)
//
VmbError_t PrepareActionCommand( VmbHandle_t aHandle, tActionCommand* aCommand );

//
// Prepare streaming settings in Vimba and the camera,
// like allocating the buffers, start capture engine, etc.
//
// Parameters:
//  [in]    aHandle     Handle to camera
//  [in]    aFrameArray Array of frames to be used for streaming
//
VmbError_t PrepareStreaming( VmbHandle_t aHandle, VmbFrame_t* aFrameArray );

//
// End streaming
//
// Parameters:
//  [in]    aHandle     Handle to camera
//  [in]    aFrameArray Array of frames to be used for streaming
//
VmbError_t StopStreaming( VmbHandle_t aHandle, VmbFrame_t* aFrameArray );

//
// Send Action Command  on system level.
// This command will be broadcasted on all network interfaces.
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [in]    aCommand    Action Command to be used by Vimba and camera
//
VmbError_t SendActionCommandOnAllInterfaces( const char* aCamera, tActionCommand aCommand );

//
// Send Action Command on interface level.
// This command will be broadcasted on given network interface.
//
// Parameters:
//  [in]    aCamera     The ID or IP address of the camera to work with
//  [in]    aInterface  The network interface on which the Action Command
//                      will be send out
//  [in]    aCommand    Action Command to be used by Vimba and camera
//
VmbError_t SendActionCommandOnInterface( const char* aCamera, const char* aIndex, tActionCommand aCommand );

#endif
