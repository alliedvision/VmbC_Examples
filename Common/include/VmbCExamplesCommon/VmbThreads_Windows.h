/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef VMB_THREADS_WINDOWS_H_
#define VMB_THREADS_WINDOWS_H_

#include <Windows.h>

enum
{
    thrd_success = ERROR_SUCCESS,
    thrd_nomem,
    thrd_timedout,
    thrd_busy,
    thrd_error
};

enum
{
    mtx_plain,
    mtx_recursive,
    mtx_timed
};

typedef struct VmbMtx
{
    HANDLE handle;
} mtx_t;


#endif
