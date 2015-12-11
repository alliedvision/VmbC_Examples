/*=============================================================================
  Copyright (C) 2014 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ErrorCodeToMessage.h

  Description: Convert the error codes to a self-explanatory message.

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

#include "ErrorCodeToMessage.h"

//
// Translates Vimba error codes to readable error messages
//
// Parameters:
//  [in]    eError      The error code to be converted to string
//
// Returns:
//  A descriptive string representation of the error code
//
const char* ErrorCodeToMessage( VmbError_t eError )
{

    switch(eError)
    {
    case VmbErrorSuccess:           return "Success.";
    case VmbErrorInternalFault:     return "Unexpected fault in VmbApi or driver.";
    case VmbErrorApiNotStarted:     return "API not started.";
    case VmbErrorNotFound:          return "Not found.";
    case VmbErrorBadHandle:         return "Invalid handle ";
    case VmbErrorDeviceNotOpen:     return "Device not open.";
    case VmbErrorInvalidAccess:     return "Invalid access.";
    case VmbErrorBadParameter:      return "Bad parameter.";
    case VmbErrorStructSize:        return "Wrong DLL version.";
    case VmbErrorMoreData:          return "More data returned than memory provided.";
    case VmbErrorWrongType:         return "Wrong type.";
    case VmbErrorInvalidValue:      return "Invalid value.";
    case VmbErrorTimeout:           return "Timeout.";
    case VmbErrorOther:             return "TL error.";
    case VmbErrorResources:         return "Resource not available.";
    case VmbErrorInvalidCall:       return "Invalid call.";
    case VmbErrorNoTL:              return "TL not loaded.";
    case VmbErrorNotImplemented:    return "Not implemented.";
    case VmbErrorNotSupported:      return "Not supported.";
    default:                        return "Unknown";
    }
}

