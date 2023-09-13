/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef LIST_CAMERAS_H_
#define LIST_CAMERAS_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Get a list of cameras
 *
 * \param[out] cameras an array of cameras allocated using malloc
 * \param[out] count the number of cameras; instead of assigning 0 ::VmbErrorNotFound is returned
 */
VmbError_t ListCameras(VmbCameraInfo_t** cameras, VmbUint32_t* count);

#endif
