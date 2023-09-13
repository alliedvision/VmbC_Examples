/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef VMB_STDATOMIC_H_
#define VMB_STDATOMIC_H_

#if defined(_MSC_VER) && !defined(__STDC_NO_ATOMICS__) // <stdatomic.h> only provided by MSVC if /std:c11 or higher is set (added in MSVC 1928)
#   define __STDC_NO_ATOMICS__
#endif

#ifdef __STDC_NO_ATOMICS__
#ifdef _WIN32
#   include "VmbStdatomic_Windows.h"
#else
#   error Functionality not implemented on the current system
#endif
    struct atomic_flag;
    typedef struct atomic_flag atomic_flag;

    _Bool atomic_flag_test_and_set(volatile atomic_flag* obj);

    void atomic_flag_clear(volatile atomic_flag* obj);
#else
#   include <stdatomic.h>
#endif

#endif
