/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef VMB_THREADS_H_
#define VMB_THREADS_H_

#if defined(_MSC_VER) && !defined(__STDC_NO_THREADS__) // <threads.h> only provided by MSVC if /std:c11 or higher is set (added in MSVC 1928)
#   define __STDC_NO_THREADS__
#endif

#ifdef __STDC_NO_THREADS__
#    ifdef _WIN32
#       include "VmbThreads_Windows.h"
#    elif __linux__
#       include "VmbThreads_Linux.h"
#    elif __APPLE__
#       include "VmbThreads_Darwin.h"
#    else
#           error Functionality not implemented on the current system
#    endif

    int mtx_init(mtx_t* mutex, int type);

    int mtx_lock(mtx_t* mutex);

    int mtx_unlock(mtx_t* mutex);

    void mtx_destroy(mtx_t* mutex);
#else
#   include <threads.h>
#endif // __STDC_NO_THREADS__

#endif // VMB_THREADS_H_
