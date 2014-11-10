/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision is prohibited.

-------------------------------------------------------------------------------

  File:         ComPort_Linux.c

  Description:  TBD

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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <ComPort.h>


int g_ComPortHandle = -1;                                                 	// Global handle of the PC's com port


// Purpose: Open the used com port of the PC and configure it to with a
//          baud rate of 115200 Baud, data with of 8, no parity and 1 stop
//          bit.
//
// Parameter:
// [in ]    const char* pComPortName        The Name of the com port to use.
//
// Returns:
//          0 in case of success otherwise a negative which indicates the error
int ComPort_Init( const char* pComPortName )
{
    int             returnValue     = 0;
    struct termios  options         = {0};
    char            *comPortName    = NULL;

	if( NULL != pComPortName )
    {
        comPortName = (char *)malloc( sizeof( *pComPortName ) + 1 );
        if( comPortName )
        {
            strcpy( comPortName, pComPortName );
        }
        else
        {
			printf( "No valid com port given.\n" );
            returnValue = -1;
        }
    }
    else
    {
		comPortName = (char *)malloc( sizeof( "/dev/ttyS0" ) + 1 );
        strcpy( comPortName, "/dev/ttyS0" );
    }

	if( 0 == returnValue )
	{
		g_ComPortHandle = open( comPortName, O_RDWR | O_NOCTTY );// | O_NDELAY );// Open the com port connection

		if( -1 != g_ComPortHandle)
		{

			fcntl( g_ComPortHandle, F_SETFL, 0 );

			options.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
			options.c_iflag = IGNPAR | ICRNL;
			options.c_oflag = 0;
			options.c_lflag = ICANON;

	        tcflush( g_ComPortHandle , TCIFLUSH );

	        if( 0 == tcsetattr( g_ComPortHandle, TCSANOW, &options ) )
	        {
	            printf( "PC's com port initialized.\n" );
	        }
	        else
	        {
	            printf( "Could not store the serial port configuration for the PC.\n" );
	            close( g_ComPortHandle );
	            returnValue = -3;
	        }

/*		    if( 0 == tcgetattr( g_ComPortHandle, &options ) )               // Get the current options for the serial port
		    {
		        cfsetispeed( &options, (speed_t)B115200 );                  // Configure the com port to 115200 Baud and 8N1
		        cfsetospeed( &options, (speed_t)B115200 );
		        options.c_cflag     &= ~PARENB;
		        options.c_cflag     &= ~CSTOPB;
		        options.c_cflag     &= ~CSIZE;
		        options.c_cflag     &= ~CRTSCTS;
		        options.c_cc[VMIN]   =  1;
		        options.c_cc[VTIME]  =  5;
		        options.c_cflag     |=  CS8;
		        options.c_cflag     |=  ( CLOCAL | CREAD );
                options.c_oflag      =  0;

		        tcflush( g_ComPortHandle , TCIFLUSH );

		        if( 0 == tcsetattr( g_ComPortHandle, TCSANOW, &options ) )
		        {
		            printf( "PC's com port initialized.\n" );
		        }
		        else
		        {
		            printf( "Could not store the serial port configuration for the PC.\n" );
		            close( g_ComPortHandle );
		            returnValue = -4;
		        }
		    }
		    else
		    {
		        printf( "Could not get the serial port configuration from the PC.\n" );
		        close( g_ComPortHandle );
		        returnValue = -3;
		    }
*/
		}
		else
		{
		    printf( "Could not open the serial port.\n" );
		    returnValue = -2;
		}
	}

	free( comPortName );

    return returnValue;
}


// Purpose: Write the given data over the configured com port of the PC.
//
// Parameter:
// [in ]    char*   pBytesToTransmit        Char array of data to transmit over the com port.
// [in ]    int     numberOfBytesToWrite    Amount of data to write.
//
// Returns:
//          0 in case of success otherwise a negative which indicates the error
int ComPort_Write( char* pBytesToTransmit, int numberOfBytesToWrite )
{
    int     returnValue     = 0;
    int     bytesWritten    = 0;

    bytesWritten = write( g_ComPortHandle, pBytesToTransmit, numberOfBytesToWrite );

    if( bytesWritten != numberOfBytesToWrite )
    {
        returnValue = -1;
    }

    return returnValue;
}


// Purpose: Read data from the configured com port of the PC into the given array.
//
// Parameter:
// [out]    char*   pBytesToReceive         Char array buffer to write the received data into.
// [in ]    int     numberOfBytesToRead     Amount of data which has to read.
//
// Returns:
//          0 in case of success otherwise a negative which indicates the error
int ComPort_Read( char* pBytesToReceive, int numberOfBytesToRead )
{
    int     returnValue = 0;
    int     bytesRead   = 0;

    bytesRead = read( g_ComPortHandle, pBytesToReceive, numberOfBytesToRead );

    if( bytesRead != numberOfBytesToRead )
    {
        returnValue = -1;
    }

    return returnValue;
}


// Purpose: Close the com port of the PC correctly.
//
// Returns:
//          0 in case of success otherwise a negative which indicates the error
int ComPort_Close()
{
    if( 0 != g_ComPortHandle )
    {
        close( g_ComPortHandle );
    }

    return 0;
}
