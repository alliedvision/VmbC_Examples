/*=============================================================================
  Copyright (C) 2012 - 2023 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        Helper.h

  Description: Helper functions used by the example.

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

#ifndef HELPER_H_
#define HELPER_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Searches for a camera which can be used by this example.
 *
 * \param[in]   needsAvtGigETL    Sending Action Commands via the Transport Layer module requires an Allied Vision GigE camera
 * \param[in]   pCameraId         Camera ID provided by the user (NULL if not provided). It is check if the camera can be used by this example.
 * \param[out]  pCameraInfo       User provied struct which will be filled with information about the found camera.
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t FindCamera(const VmbBool_t needsAvtGigETL, const char* const pCameraId, VmbCameraInfo_t* const pCameraInfo);

/**
 * \brief Starts the API and prints version information about the API.
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t StartApi();

#endif
