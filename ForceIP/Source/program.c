/*=============================================================================
  Copyright (C) 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.cpp

  Description: Main entry point of ForceIP example of VimbaC.

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
#include <string.h>
#ifndef WIN32
    #include <arpa/inet.h>
#else
    #include <windows.h>
#endif

#include <ForceIP.h>

int main( int argc, char* argv[] )
{
    printf( "\n//////////////////////////////////\n" );
    printf( "/// Vimba API Force IP Example ///\n" );
    printf( "//////////////////////////////////\n\n" );

    if(     4 > argc
        ||  5 < argc )
    {
        printf( "Usage: ForceIP MAC IP Subnet [Gateway]\n\n" );
        printf( "Parameters:\n" );
        printf( "MAC      The MAC address of the camera whose IP address shall be changed.\n" );
        printf( "         Either hexadecimal with preceding 0x or decimal.\n" );
        printf( "IP       The new IPv4 address of the camera in numbers and dots notation.\n" );
        printf( "Subnet   The new network mask of the camera in numbers and dots notation.\n" );
        printf( "Gateway  The address of a possible gateway if the camera is not connected\n" );
        printf( "         to the host PC directly.\n\n" );
        printf( "For example to change the IP address of a camera with the MAC address 0x0F3101D540\nto 169.254.1.1 in a class B network call:\n\n" );
        printf( "ForceIP 0x0F3101D540 169.254.1.1 255.255.0.0\n\n" );
#ifndef WIN32
        printf( "To alter cameras in foreign subnets run the program with sudo -E / su -m. \n\n" );
#endif
    }
    else
    {
        ForceIP( argv[1], argv[2], argv[3], argc == 4 ? argv[4] : NULL );
    }

    return 0;
}
