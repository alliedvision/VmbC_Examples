/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef LIST_TRANSPORT_LAYERS_H_
#define LIST_TRANSPORT_LAYERS_H_

#include <VmbC/VmbCTypeDefinitions.h>

/**
 * \brief get a list of transport layers
 *
 * \param[out] transportLayers an array of transport layers allocated using malloc
 * \param[out] count the number of transport layers; instead of assigning 0 ::VmbErrorNotFound is returned
 */
VmbError_t ListTransportLayers(VmbTransportLayerInfo_t** transportLayers, VmbUint32_t* count);

#endif
