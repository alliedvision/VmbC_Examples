/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        VmbThreads_Linux.h

  Description: Provide functionality that should be provided by <threads.h>
               for systems that don't provide this functionality.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
