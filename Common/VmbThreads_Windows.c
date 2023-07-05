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
    switch (type)
    {
    case mtx_plain:
    case mtx_recursive:
    {
        HANDLE h = CreateMutex(NULL, FALSE, NULL);
        if (h != NULL)
        {
            mutex->handle = h;
            return thrd_success;
        }
        break;
    }
    // other mutex types not implemented yet
    }
    return thrd_error;
}

int mtx_lock(mtx_t* mutex)
{
    if (mutex != NULL)
    {
        if (WAIT_OBJECT_0 == WaitForSingleObject(mutex->handle, INFINITE))
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
        if (ReleaseMutex(mutex->handle))
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
        CloseHandle(mutex->handle);
    }
}

#endif
