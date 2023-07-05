/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include "include/VmbCExamplesCommon/VmbThreads.h"

#ifdef __STDC_NO_THREADS__

int mtx_init(mtx_t* mutex, int type)
{
    if (mutex == NULL)
    {
        return thrd_error;
    }

    int pthreadMutexType;
    switch (type)
    {
    case mtx_plain:
        pthreadMutexType = PTHREAD_MUTEX_NORMAL;
        break;
    case mtx_recursive:
        pthreadMutexType = PTHREAD_MUTEX_RECURSIVE;
        break;
    default: // other types not implemented
        return thrd_error;
    }

    pthread_mutexattr_t mutexAttributes;
    int result = thrd_error;
    if (!pthread_mutexattr_init(&mutexAttributes))
    {
        if(!pthread_mutexattr_settype(&mutexAttributes, pthreadMutexType))
        {
            if (!pthread_mutex_init(&mutex->mutex, &mutexAttributes))
            {
                result = thrd_success;
            }
        }
        pthread_mutexattr_destroy(&mutexAttributes);
    }
    return result;
}

int mtx_lock(mtx_t* mutex)
{
    if (mutex != NULL)
    {
        if (!pthread_mutex_lock(&mutex->mutex))
        {
            return thrd_success;
        }
    }
    return thrd_error;
}

int mtx_unlock(mtx_t* mutex)
{
    if (mutex != NULL)
    {
        if(!pthread_mutex_unlock(&mutex->mutex))
        {
            return thrd_success;
        }
    }
    return thrd_error;
}

void mtx_destroy(mtx_t* mutex)
{
    if (mutex != NULL)
    {
        pthread_mutex_destroy(&mutex->mutex);
    }
}

#endif
