/*=============================================================================
  Copyright (C) 2012-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef ACTION_COMMANDS_H_
#define ACTION_COMMANDS_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Helper struct containing the provided command line options.
 */
typedef struct ActionCommandsOptions
{
    VmbBool_t           useAllInterfaces;
    VmbBool_t           sendAsUnicast;

    char const*         pCameraId;

    const VmbUint32_t   deviceKey;
    const VmbUint32_t   groupKey;
    const VmbUint32_t   groupMask;
} ActionCommandsOptions;

/**
 * \brief Send an Action Command to acquire and grab an image
 *
 * \param[in] pOptions  Provided command line options and details for the Action Command
 * \param[in] pCamera   Information about the used and already opened camera
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t SendActionCommand(const ActionCommandsOptions* const pOptions, const VmbCameraInfo_t* const pCamera);

/**
 * \brief Configures the camera to be triggered by Action Commands
 *
 * \param[in] camera    Handle to the already opened camera
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t PrepareCameraForActionCommands(const VmbHandle_t camera);

/**
 * \brief Configure the Action Command features for later sending as broadcast
 *
 * \param[in] handle    Transport Layer or Interface handle used to configure the Action Command features
 * \param[in] pOptions  Provided command line options and details for the Action Command
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t PrepareActionCommand(const VmbHandle_t handle, const ActionCommandsOptions* const pOptions);

/**
 * \brief Configure the Action Command features for later sending as unicast directly to the camera
 *
 * \param[in] handle        Transport Layer or Interface handle used to configure the Action Command features
 * \param[in] pOptions      Provided command line options and details for the Action Command
 * \param[in] pCameraInfo   Information about the used and already opened camera
 *
 * \return An error code indicating success or the type of error that occurred.
 */
VmbError_t PrepareActionCommandAsUnicast(const VmbHandle_t handle, const ActionCommandsOptions* const pOptions, const VmbCameraInfo_t* const pCameraInfo);

#endif
