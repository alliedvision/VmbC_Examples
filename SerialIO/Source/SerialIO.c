/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision is prohibited.

-------------------------------------------------------------------------------

  File:         SerialIO.c

  Description:  The SerialIO example will read and write from the cameras
                serial port.
                To setup and use the serial IO, it is needed to use the direct
                register access of the camera.

          Camera                              PC
                                             ____
         _________        GigE Cable        | __ |
      __| Allied  |~~~~~~~~~~~~~~~~~~~~~~~~~|    |
     (__|  Vision |       RS232 Cable       |    |
        |_________|~~~~~~~~~~~~~~~~~~~~~~~~~|____|
           GigE

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

#include <stdio.h>
#include <stdlib.h>

#include <SerialIO.h>
#include <ComPort.h>

#include <../../Common/PrintVimbaVersion.h>
#include <../../Common/DiscoverGigECameras.h>

const VmbUint64_t   SERIAL_INQUIRY_ADDRESS          = 0x16000;
const VmbUint64_t   SERIAL_ERROR_STATUS_ADDRESS     = 0x16010;
const VmbUint64_t   SERIAL_MODE_INQUIRY_ADDRESS     = 0x16100;
const VmbUint64_t   SERIAL_MODE_ADDRESS             = 0x16104;
const VmbUint64_t   SERIAL_TX_INQUIRY_ADDRESS       = 0x16120;
const VmbUint64_t   SERIAL_TX_STATUS_ADDRESS        = 0x16124;
const VmbUint64_t   SERIAL_TX_CONTROL_ADDRESS       = 0x16128;
const VmbUint64_t   SERIAL_TX_LENGTH_ADDRESS        = 0x1612C;
const VmbUint64_t   SERIAL_RX_INQUIRY_ADDRESS       = 0x16140;
const VmbUint64_t   SERIAL_RX_STATUS_ADDRESS        = 0x16144;
const VmbUint64_t   SERIAL_RX_CONTROL_ADDRESS       = 0x16148;
const VmbUint64_t   SERIAL_RX_LENGTH_ADDRESS        = 0x1614C;
const VmbUint64_t   SERIAL_TX_BUFFER_ADDRESS        = 0x16400;
const VmbUint64_t   SERIAL_RX_BUFFER_ADDRESS        = 0x16800;

// Purpose: Serial IO Test Function.
//
// Parameter:
// [in ]    const char* pStrID          The ID of the camera to use. May be NULL.
// [in ]    const char* pComPortName    The name of the used com port. May be NULL.
void SerialIO( const char* pStrID, const char* pComPortName )
{
    VmbError_t                  err                         = VmbErrorSuccess;
    VmbHandle_t                 cameraHandle                = NULL;                                 // A handle to our camera
    VmbCameraInfo_t*            pCameras                    = NULL;                                 // A list of camera infos
    VmbUint32_t                 count                       = 0;                                    // The number of found cameras
    VmbUint32_t                 i                           = 0;
    VmbUint32_t                 readDataCompleted           = 0;
    VmbUint32_t                 writeDataCompleted          = 0;
    char                        bytesToTransmit[]           = "Allied Vision";
    char                        bytesToReceive[255]         = {0};

    // Register variables to handle the camera register layout
    SerialInquiryRegister       serialInquiryRegister       = {0};
    SerialModeInquiryRegister   serialModeInquiryRegister   = {0};
    SerialModeRegister          serialModeRegister          = {0};
    SerialTxInquiryRegister     serialTxInquiryRegister     = {0};
    SerialTxStatusRegister      serialTxStatusRegister      = {0};
    SerialTxControlRegister     serialTxControlRegister     = {0};
    SerialTxLengthRegister      serialTxLengthRegister      = {0};
    SerialRxInquiryRegister     serialRxInquiryRegister     = {0};
    SerialRxStatusRegister      serialRxStatusRegister      = {0};
    SerialRxControlRegister     serialRxControlRegister     = {0};
    SerialRxLengthRegister      serialRxLengthRegister      = {0};

    err = VmbStartup();                                                                             // Initialize the Vimba API
    PrintVimbaVersion();                                                                            // Print Vimba Version
    if( VmbErrorSuccess == err )
    {
        DiscoverGigECameras();                                                                      // Seek for GigE cameras on the network

        if( NULL == pStrID )                                                                        // If no ID was provided use the first camera
        {
            err = VmbCamerasList (  NULL,                                                           // Get the amount of known cameras
                                    0,
                                    &count,
                                    sizeof *pCameras );
            if(     VmbErrorSuccess == err
                &&  0 < count )
            {
                pCameras = (VmbCameraInfo_t*)malloc( count * sizeof *pCameras );
                if( NULL != pCameras )
                {
                    err = VmbCamerasList(   pCameras,                                               // Get all known cameras
                                            count,
                                            &count,
                                            sizeof *pCameras );
                    if(     VmbErrorSuccess == err
                        ||  VmbErrorMoreData == err )                                               // If a new camera was connected since we queried
                    {                                                                               // for the amount of cameras, we can ignore that one
                        if( 0 < count )
                        {
                            err = VmbCameraOpen(    pCameras[0].cameraIdString,                     // Finally open the first one
                                                    VmbAccessModeFull,
                                                    &cameraHandle );
                        }
                        else
                        {
                            printf( "Camera lost. Error code: %d\n", err );
                        }
                    }
                    else
                    {
                        printf( "Could not list cameras. Error code: %d\n", err );
                    }

                    free( pCameras );
                    pCameras = NULL;
                }
                else
                {
                    printf( "Could not allocate camera list.\n" );
                }
            }
            else
            {
                printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
            }
        }
        else
        {
            err = VmbCameraOpen(    pStrID,                                                         // Open the camera with the given ID
                                    VmbAccessModeFull,
                                    &cameraHandle );
        }

        if( VmbErrorSuccess == err )
        {
            if( 0 == ComPort_Init( pComPortName ) )
            {
                err = VmbRegistersRead( cameraHandle, 1, &SERIAL_INQUIRY_ADDRESS, &serialInquiryRegister.RawData, &readDataCompleted );

                if( VmbErrorSuccess == err )
                {
                    // Setup the serial port of the camera with a baud rate of 115.2 kBaud, 8 data bits, no parity and 1 stop bit
                    serialModeRegister.BaudRate         = VmbSerialBaudRate115200;
                    serialModeRegister.CharacterLength  = VmbSerialCharLength8Bits;
                    serialModeRegister.Parity           = VmbSerialParityNone;
                    serialModeRegister.StopBits         = VmbSerialStopBits1;

                    err = VmbRegistersWrite( cameraHandle, 1, &SERIAL_MODE_ADDRESS, &serialModeRegister.RawData, &writeDataCompleted );

                    if( VmbErrorSuccess == err )
                    {
                        if( VmbInquiryYes == serialInquiryRegister.TransmitterInquiry )
                        {
                            serialTxControlRegister.TransmitterEnable = 1;                          // Enable the transmitter of the camera serial port

                            err = VmbRegistersWrite( cameraHandle, 1, &SERIAL_TX_CONTROL_ADDRESS, &serialTxControlRegister.RawData, &writeDataCompleted );
                            if( VmbErrorSuccess == err )
                            {
                                err = VmbMemoryWrite( cameraHandle, SERIAL_TX_BUFFER_ADDRESS, sizeof( bytesToTransmit ), bytesToTransmit, &writeDataCompleted );

                                if( VmbErrorSuccess == err )
                                {
                                    serialTxLengthRegister.DataLength = sizeof( bytesToTransmit );  // Write the register, how much data has to be written on the serial port

                                    err = VmbRegistersWrite( cameraHandle, 1, &SERIAL_TX_LENGTH_ADDRESS, &serialTxLengthRegister.RawData, &writeDataCompleted );

                                    if( VmbErrorSuccess == err )
                                    {
                                        if( 0 == ComPort_Read( bytesToReceive, sizeof( bytesToTransmit ) ) )
                                        {
                                            printf( "Received message on the serial port: '%s'\n", bytesToReceive );
                                        }
                                        else
                                        {
                                            printf( "Could not read on the serial port.\n" );
                                        }                                
                                    }
                                    else
                                    {
                                        printf( "Could not write the length of the written buffer. Error code: %d\n", err );
                                    }
                                }
                                else
                                {
                                    printf( "Could not fill the transmit buffer. Error code: %d\n", err );
                                }
                            }
                            else
                            {
                                printf( "Could not enable the transmitter within the camera. Error code: %d\n", err );
                            }
                        }
                        else
                        {
                            printf( "The selected camera has no serial transmitter.\n" );
                        }

                        for( i = 0; i < sizeof( bytesToReceive ); i++ )
                        {
                            bytesToReceive[i] = 0;
                        }

                        if( VmbInquiryYes == serialInquiryRegister.ReceiverInquiry )
                        {
                            serialRxControlRegister.ReceiverReset   = 1;                            // Reset the receiver to clear the RxBuffer
                            serialRxControlRegister.ReceiverEnable  = 1;                            // Enable the receiver of the camera serial port

                            err = VmbRegistersWrite( cameraHandle, 1, &SERIAL_RX_CONTROL_ADDRESS, &serialRxControlRegister.RawData, &writeDataCompleted );

                            if( VmbErrorSuccess == err )
                            {
                                if( 0 == ComPort_Write( bytesToTransmit, sizeof( bytesToTransmit ) ) )
                                {
                                    err = VmbRegistersRead( cameraHandle, 1, &SERIAL_RX_LENGTH_ADDRESS, &serialRxLengthRegister.RawData, &readDataCompleted );

                                    if( VmbErrorSuccess == err )
                                    {
                                        err = VmbMemoryRead( cameraHandle, SERIAL_RX_BUFFER_ADDRESS, (VmbUint32_t)serialRxLengthRegister.DataLength, bytesToReceive, &readDataCompleted );

                                        if( VmbErrorSuccess == err)
                                        {
                                             printf( "Received message on the camera serial: '%s'\n", bytesToReceive );
                                        }
                                        else
                                        {
                                            printf( "Could not read the receive buffer from the camera. Error code: %d\n", err );
                                        }
                                    }
                                    else
                                    {
                                        printf( "Could not read the length of the received data. Error code: %d\n", err );
                                    }
                                }
                                else
                                {
                                    printf( "Could not write on the serial port.\n" );
                                }
                            }
                            else
                            {
                                printf( "Could not enable the receiver within the camera. Error code: %d\n", err );
                            }
                        }
                        else
                        {
                            printf( "The selected camera has no serial receiver.\n" );
                        }
                    }
                    else
                    {
                        printf( "Could not setup the serial mode on the camera. Error code: %d\n", err );
                    }
                }
                else
                {
                    printf( "Could not read inquiry register. Error code: %d\n", err );
                }

                ComPort_Close();                                                                    // Close the com port
            }

            VmbCameraClose( cameraHandle );                                                         // Close the camera
        }
        else
        {
            printf( "Could not open camera. Error code: %d\n", err );
        }
        
        VmbShutdown();                                                                              // Close Vimba
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
}
