/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        VmbStdatomic.h

  Description: Provide functionality that should be provided by <stdatomic.h>
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