/*=============================================================================
  Copyright (C) 2021 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        TransportLayerTypeToString.c

  Description: Convert VmbTransportLayerType_t to string.

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
