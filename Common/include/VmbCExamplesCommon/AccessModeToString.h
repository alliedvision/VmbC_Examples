/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef ACCESS_MODE_TO_STRING_H_
#define ACCESS_MODE_TO_STRING_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Translates Vmb access modes to a readable string
 *
 * \param[in] eMode The access mode to be converted to string
 *
 * \return A descriptive string representation of the access mode
 */
const char* AccessModesToString( VmbAccessMode_t eMode );

#endif // ACCESS_MODE_TO_STRING_H_
