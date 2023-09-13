/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef VMB_STDATOMIC_WINDOWS_H_
#define VMB_STDATOMIC_WINDOWS_H_

#include <stdbool.h>

#include <Windows.h>

struct atomic_flag
{
    LONG value;
};

#define ATOMIC_FLAG_INIT {.value=false}

#endif
