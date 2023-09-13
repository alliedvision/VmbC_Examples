/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef HELPER_H_
#define HELPER_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Searches for a camera which can be used by this example.
 *
 * \param[in]   needsAvtGigETL    Sending Action Commands via the Transport Layer module requires an Allied Vision GigE camera
 * \param[in]   pCameraId         Camera ID provided by the user (NULL if not provided). It is checked if the camera can be used by this example.
 * \param[out]  pCameraInfo       User provided struct which will be filled with information about the found camera.
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t FindCamera(const VmbBool_t needsAvtGigETL, const char* const pCameraId, VmbCameraInfo_t* const pCameraInfo);

/**
 * \brief Starts the API and prints version information about the API.
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t StartApi(void);

#endif
