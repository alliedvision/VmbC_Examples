/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include "include/VmbCExamplesCommon/VmbStdatomic.h"

#ifdef __STDC_NO_ATOMICS__

_Bool atomic_flag_test_and_set(volatile atomic_flag* obj)
{
    return InterlockedExchange(&obj->value, (LONG)true);
}

void atomic_flag_clear(volatile atomic_flag* obj)
{
    InterlockedExchange(&obj->value, (LONG)false);
}

#endif
