/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:         SerialIO.h

  Description:  The SerialIO example will read and write from the cameras
                serial port.

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

#include <VimbaC/Include/VimbaC.h>

#ifndef SERIAL_IO_H_
#define SERIAL_IO_H_

// Purpose: Serial IO Test Function.
//
// Parameter:
// [in ]    const char* pStrID          The ID of the camera to use. May be NULL.
// [in ]    const char* pComPortName    The name of the used com port. May be NULL.
void SerialIO( const char* pStrID, const char* pComPortName );

//
// Possible inquiry types
//
typedef enum VmbInquiryType
{
    VmbInquiryNo  =  0,           // This feature is not present
    VmbInquiryYes =  1,           // This feature is present
} VmbInquiryType;

//
// Serial baud rate modes
//
typedef enum VmbSerialModeType
{
    VmbSerialBaudRate300        = 0,
    VmbSerialBaudRate600        = 1,
    VmbSerialBaudRate1200       = 2,
    VmbSerialBaudRate2400       = 3,
    VmbSerialBaudRate4800       = 4,
    VmbSerialBaudRate9600       = 5,
    VmbSerialBaudRate19200      = 6,
    VmbSerialBaudRate38400      = 7,
    VmbSerialBaudRate57600      = 8,
    VmbSerialBaudRate115200     = 9,
    VmbSerialBaudRate230400     = 10,
} VmbSerialModeType;

//
// Serial parity
//
typedef enum VmbSerialParityType
{
    VmbSerialParityNone         = 0,
    VmbSerialParityOdd          = 1,
    VmbSerialParityEven         = 2,
} VmbSerialParityType;

//
// Serial character length
//
typedef enum VmbSerialCharLengthType
{
    VmbSerialCharLength5Bits    = 0,
    VmbSerialCharLength6Bits    = 1,
    VmbSerialCharLength7Bits    = 2,
    VmbSerialCharLength8Bits    = 3,
} VmbSerialCharLengthType;

//
// Serial stop bits
//
typedef enum VmbSerialStopBitsType
{
    VmbSerialStopBits1          = 0,
    VmbSerialStopBits1_5        = 1,
    VmbSerialStopBits2          = 2,
} VmbSerialStopBitsType;

//
// Serial inquiry register layout
//
typedef union SerialInquiryRegister
{
    struct
    {
        VmbUint64_t TransmitterInquiry      : 1;
        VmbUint64_t ReceiverInquiry         : 1;
        VmbUint64_t TimestampModeInquriy    : 1;
        VmbUint64_t                         : 61;   // fill to 64 bits
    };

    VmbUint64_t RawData;
}SerialInquiryRegister;

//
// Serial mode inquiry register layout
//
typedef union SerialModeInquiryRegister
{
    struct  
    {
        VmbUint64_t InqBaud_300         : 1;
        VmbUint64_t InqBaud_600         : 1;
        VmbUint64_t InqBaud_1200        : 1;
        VmbUint64_t InqBaud_2400        : 1;
        VmbUint64_t InqBaud_4800        : 1;
        VmbUint64_t InqBaud_9600        : 1;
        VmbUint64_t InqBaud_19200       : 1;
        VmbUint64_t InqBaud_38400       : 1;
        VmbUint64_t InqBaud_57600       : 1;
        VmbUint64_t InqBaud_115200      : 1;
        VmbUint64_t InqBaud_230400      : 1;
        VmbUint64_t                     : 5;
        VmbUint64_t InqNoParity         : 1;
        VmbUint64_t InqOddParity        : 1;
        VmbUint64_t InqEvenParity       : 1;
        VmbUint64_t                     : 1;
        VmbUint64_t InqCharLength5Bits  : 1;
        VmbUint64_t InqCharLength6Bits  : 1;
        VmbUint64_t InqCharLength7Bits  : 1;
        VmbUint64_t InqCharLength8Bits  : 1;
        VmbUint64_t InqStopBit1         : 1;
        VmbUint64_t InqStopBit1_5       : 1;
        VmbUint64_t InqStopBit2         : 1;
        VmbUint64_t                     : 38;   // fill to 64 bits
    };

    VmbUint64_t RawData;
} SerialModeInquiryRegister;

//
// Serial mode register layout
//
typedef union SerialModeRegister
{
    struct 
    {
        VmbUint64_t BaudRate            : 8;
        VmbUint64_t Parity              : 2;
        VmbUint64_t CharacterLength     : 2;
        VmbUint64_t StopBits            : 2;
        VmbUint64_t                     : 50;   // fill to 64 bits
    };

    VmbUint64_t RawData;
} SerialModeRegister;

//
// Serial transmit inquiry register layout
//
typedef union SerialTxInquiryRegister
{
    struct
    {
        VmbUint64_t BufferLength        : 16;
        VmbUint64_t                     : 48;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialTxInquiryRegister;

//
// Serial transmit status register layout
//
typedef union SerialTxStatusRegister
{
    struct
    {
        VmbUint64_t TransmitterReady    : 1;
        VmbUint64_t                     : 63;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialTxStatusRegister;

//
// Serial transmit control register layout
//
typedef union SerialTxControlRegister
{
    struct
    {
        VmbUint64_t TransmitterReset    : 1;
        VmbUint64_t TransmitterEnable   : 1;
        VmbUint64_t                     : 62;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialTxControlRegister;

//
// Serial transmit length register layout
//
typedef union SerialTxLengthRegister
{
    struct
    {
        VmbUint64_t DataLength          : 16;
        VmbUint64_t                     : 48;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialTxLengthRegister;

//
// Serial receive inquiry register layout
//
typedef union SerialRxInquiryRegister
{
    struct
    {
        VmbUint64_t BufferLength        : 16;
        VmbUint64_t                     : 48;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialRxInquiryRegister;

//
// Serial receive status register layout
//
typedef union SerialRxStatusRegister
{
    struct
    {
        VmbUint64_t ReceiveOverrun      : 1;
        VmbUint64_t ReceiveFramingError : 1;
        VmbUint64_t ReceiveParityError  : 1;
        VmbUint64_t                     : 61;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialRxStatusRegister;

//
// Serial receive control register layout
//
typedef union SerialRxControlRegister
{
    struct
    {
        VmbUint64_t ReceiverReset       : 1;
        VmbUint64_t ReceiverEnable      : 1;
        VmbUint64_t                     : 62;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialRxControlRegister;

//
// Serial receive length register layout
//
typedef union SerialRxLengthRegister
{
    struct
    {
        VmbUint64_t DataLength          : 16;
        VmbUint64_t                     : 48;   // fill to 64 Bits
    };

    VmbUint64_t RawData;
} SerialRxLengthRegister;

#endif
