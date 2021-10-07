/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        VmbThreads.h

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

#ifndef VMB_THREADS_H_
#define VMB_THREADS_H_

#if defined(_MSC_VER) && !defined(__STDC_NO_THREADS__) // <threads.h> only provided by MSVC if /std:c11 or higher is set (added in MSVC 1928)
#   define __STDC_NO_THREADS__
#endif

#ifdef __STDC_NO_THREADS__
#ifdef _WIN32
#   include "VmbThreads_Windows.h"
#else
#   ifdef __linux__
#   include "VmbThreads_Linux.h"
#   else
#       error Functionality not implemented on the current system
#   endif
#endif
    int mtx_init(mtx_t* mutex, int type);

    int mtx_lock(mtx_t* mutex);

    int mtx_unlock(mtx_t* mutex);

    void mtx_destroy(mtx_t* mutex);
#else
#   include <threads.h>
#endif

#endif
