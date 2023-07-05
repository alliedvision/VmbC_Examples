/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef ERROR_CODE_TO_MESSAGE_H_
#define ERROR_CODE_TO_MESSAGE_H_

#include <VmbC/VmbCommonTypes.h>

/**
 * \brief Translates Vmb error codes to readable error messages
 *
 * \param[in] eError    The error code to be converted to string
 *
 * \return A descriptive string representation of the error code
 */
const char* ErrorCodeToMessage( VmbError_t eError );

#endif
