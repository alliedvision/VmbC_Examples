/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef IMAGE_ACQUISITION_H_
#define IMAGE_ACQUISITION_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Prepares and starts the stream.
 *
 * \param[in] cameraHandle  Handle of the alreay opened camera which should be used for streaming
 *
 * \return An error code indicating success or the type of error that occurred.
*/
VmbError_t StartStream(const VmbHandle_t cameraHandle);

/**
 * \brief Stops the stream reverting the steps done during StartStream.
 *
 * \param[in] cameraHandle  Handle of the alreay streaming camera
 *
 * \return An error code indicating success or the type of error that occurred.
*/
VmbError_t StopStream(const VmbHandle_t cameraHandle);

#endif
