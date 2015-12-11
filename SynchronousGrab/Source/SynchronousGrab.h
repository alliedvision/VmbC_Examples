/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        SynchronousGrab.h

  Description: The SynchronousGrab example will grab a single image
               synchronously and save it to a file using VimbaC.

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

#ifndef SYNCHRONOUS_GRAB_H_
#define SYNCHRONOUS_GRAB_H_

#include <VimbaC/Include/VmbCommonTypes.h>

//
// Starts Vimba API
// Opens the given camera, if no camera ID was provided, the first camera found will be used
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Acquires one image synchronously
// Writes the image as bitmap to file
// Closes the camera in case of failure
//
// Parameters:
//  [in]    pCameraID           The ID of the camera to work on. Can be NULL.
//  [in]    pFileName           The path of the bitmap where the image is saved to
//
// Returns:
//  An API status code
//
VmbError_t SynchronousGrab ( const char* pCameraID, const char* pFileName );

#endif
