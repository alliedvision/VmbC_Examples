/*=============================================================================
  Copyright (C) 2014-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include "include/VmbCExamplesCommon/AccessModeToString.h"

const char* AccessModesToString(VmbAccessMode_t eMode)
{
    switch (eMode)
    {
        case VmbAccessModeFull:
        {
            return "Full access";
        }
        case VmbAccessModeNone:
        {
            return "No access";
        }
        case VmbAccessModeRead:
        {
            return "Readonly access";
        }
        case (VmbAccessModeRead | VmbAccessModeFull):
        {
            return "Full access, readonly access";
        }
        case VmbAccessModeUnknown:
        default:
        {
            return "Unknown";
        }
    }
}
