/*=============================================================================
  Copyright (C) 2012 - 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ListFeatures.h

  Description: The ListFeatures example will list all available features of a
               camera that are found by VmbC.

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

#ifndef LIST_FEATURES_H_
#define LIST_FEATURES_H_

#include <stdbool.h>
#include <stddef.h>

#include "VmbC/VmbCTypeDefinitions.h"

/**
 * \brief list the features of a transport layer at a given index
 *
 * \param[in] tlIndex the zero based index of the transport layer
 * 
 * \return a code to return from main()
 */
int ListTransportLayerFeatures(size_t tlIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

/**
 * \brief list the features of a interface at a given index
 *
 * \param[in] interfaceIndex the zero based index of the transport layer
 * \param[in] printedFeatureMaximumVisibility   the maximum visibility of features to be printed
 * 
 * \return a code to return from main()
 */
int ListInterfaceFeatures(size_t interfaceIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

/**
 * \brief list the features for the camera at a given index
 * 
 * \param[in] index                             the zero based index of the camera
 * \param[in] remoteDevice                      true, if the remote device features should be printed, false otherwise
 * \param[in] printedFeatureMaximumVisibility   the maximum visibility of features to be printed
 * 
 * \return a code to return from main()
 */
int ListCameraFeaturesAtIndex(size_t index, bool remoteDevice, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

/**
 * \brief list the features for a camera with a given id
 *
 * \param[in] id            the camera id
 * \param[in] remoteDevice  true, if the remote device features should be printed, false otherwise
 * \param[in] printedFeatureMaximumVisibility   the maximum visibility of features to be printed
 * 
 * \return a code to return from main()
 */
int ListCameraFeaturesAtId(char const* id, bool remoteDevice, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

/**
 * \brief list the stream features of a camera at a given index
 * 
 * \param[in] cameraIndex   the zero based index of the camera providing the stream
 * \param[in] streamIndex   the index of the stream to print
 * \param[in] printedFeatureMaximumVisibility   the maximum visibility of features to be printed
 * 
 * \return a code to return from main()
 */
int ListStreamFeaturesAtIndex(size_t cameraIndex, size_t streamIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

/**
 * \brief list the stream features of a camera at a given index
 *
 * \param[in] cameraId      the id of the camera providing the stream
 * \param[in] streamIndex   the index of the stream to print
 * \param[in] printedFeatureMaximumVisibility   the maximum visibility of features to be printed
 * 
 * \return a code to return from main()
 */
int ListStreamFeaturesAtId(char const* cameraId, size_t streamIndex, VmbFeatureVisibility_t printedFeatureMaximumVisibility);

#endif
