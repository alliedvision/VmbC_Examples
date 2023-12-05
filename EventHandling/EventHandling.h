/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef EVENTHANDLING_H
#define EVENTHANDLING_H

#include <VmbC/VmbCommonTypes.h>

/**
* \brief Demonstrate the camera event functionality of VmbC
*/
int CameraEventDemo(char const* cameraId);

/**
* \brief Helper function to activate camera event notifications
*/
VmbErrorType ActivateNotification(VmbHandle_t cameraHandle);

/**
* \brief Helper function to register an event callback function
*/
VmbErrorType RegisterEventCallback(VmbHandle_t cameraHandle);

#endif // EVENTHANDLING_H