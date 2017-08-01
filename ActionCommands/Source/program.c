/*=============================================================================
  Copyright (C) 2017 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.cpp

  Description: Main entry point of ActionCommands example of VimbaC.

               annotations:
                -local variables are prefixed with 'l' for local
                -function parameter are prefixed with 'a' for 'argument'
                -structs are prefixed with 't' for 'type'
                -global variables are prefixed with 'g' for 'global'

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

#include <VimbaC/Include/VimbaC.h>
#include <../../Common/PrintVimbaVersion.h>

#include <ActionCommands.h>

// function to print out help how to use this example application
void PrintHelp()
{
    printf( "Usage: ActionCommands <CameraID/IPAdress> <InterfaceID>\n\n" );
    printf( "Parameters:   CameraID         ID of the camera to be used\n" );
    printf( "              IPAddress        IP address of camera to react on Action Command\n" );
    printf( "              InterfaceID      ID of network interface to send out Action Command\n" );
    printf( "                               'ALL' enables broadcast on all interfaces\n\n" );
}

int main( int argc, char* argv[] )
{
    VmbError_t      lError      = VmbErrorSuccess;
    VmbUint32_t     lIPAddress  = 0;
    tActionCommand  lActionCommand;

    printf( "\n" );
    printf( "/////////////////////////////////////////\n" );
    printf( "/// Vimba API Action Commands Example ///\n" );
    printf( "/////////////////////////////////////////\n" );
    printf( "\n" );

    // show Vimba API version
    PrintVimbaVersion();

    printf( "\n" );

    // check number of arguments
    if( 3 == argc )
    {
        // define Action Command to be set in the camera
        // and used by either Vimba system or interface module
        lActionCommand.mDeviceKey   = 1;
        lActionCommand.mGroupKey    = 1;
        lActionCommand.mGroupMask   = 1;

        // check if interface index is '-1' to send out Action Command on all interfaces
        // if not, send Action Command via given network interface
        if( 0 == strcmp("ALL", argv[2]) )
        {
            lError = SendActionCommandOnAllInterfaces( argv[1], lActionCommand );
        }
        else
        {
            lError = SendActionCommandOnInterface( argv[1], argv[2], lActionCommand );
        }

    }
    else
    {
        lError = VmbErrorBadParameter;
        printf( "[F]...Invalid number of parameters given!\n\n" );
        PrintHelp();
    }

    printf( "\n" );

    return lError;
}
