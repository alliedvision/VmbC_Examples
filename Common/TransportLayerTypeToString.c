/*=============================================================================
  Copyright (C) 2021-2023 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#include "include/VmbCExamplesCommon/TransportLayerTypeToString.h"


char const* TransportLayerTypeToString(VmbTransportLayerType_t tlType)
{
    switch (tlType)
    {
    case VmbTransportLayerTypeCL:
        return "Camera Link";
    case VmbTransportLayerTypeCLHS:
        return "Camera Link HS";
    case VmbTransportLayerTypeCustom:
        return "Custom";
    case VmbTransportLayerTypeCXP:
        return "CoaXPress";
    case VmbTransportLayerTypeEthernet:
        return "Generic Ethernet";
    case VmbTransportLayerTypeGEV:
        return "GigE Vision";
    case VmbTransportLayerTypeIIDC:
        return "IIDC 1394";
    case VmbTransportLayerTypeMixed:
        return "Mixed";
    case VmbTransportLayerTypePCI:
        return "PCI / PCIe";
    case VmbTransportLayerTypeU3V:
        return "USB 3 Vision";
    case VmbTransportLayerTypeUVC:
        return "USB video class";
    default:
        return "[Unknown]";
    }
}
