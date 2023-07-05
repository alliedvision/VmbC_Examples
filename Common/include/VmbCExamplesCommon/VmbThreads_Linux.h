/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef VMB_THREADS_LINUX_H_
#define VMB_THREADS_LINUX_H_

#include <pthread.h>

enum
{
    thrd_success = 0,
    thrd_nomem,
    thrd_timedout,
    thrd_busy,
    thrd_error
};

enum
{
    mtx_plain = 0,
    mtx_recursive = 1,
    mtx_timed = 2
};

typedef struct VmbMtx
{
    pthread_mutex_t mutex;
} mtx_t;


#endif
