/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef LIST_INTERFACES_H_
#define LIST_INTERFACES_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief Get the list of interfaces
 *
 * \param[out] interfaces an array of interfaces allocated using malloc
 * \param[out] count the number of interfaces; instead of assigning 0 ::VmbErrorNotFound is returned
 */
VmbError_t ListInterfaces(VmbInterfaceInfo_t** interfaces, VmbUint32_t* count);

#endif
