/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef ARRAY_ALLOC_H_
#define ARRAY_ALLOC_H_

#include <stdlib.h>

#ifndef NULL
    #define NULL 0
#endif


/**
 * \brief uses malloc to allocate memory for an array of \p size elements of type \p type
 */
#define VMB_MALLOC_ARRAY(type, size) ((type*) malloc(size * sizeof(type)))

#endif
