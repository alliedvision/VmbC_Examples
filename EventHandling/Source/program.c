/*=============================================================================
  Copyright (C) 2012 - 2019 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.cpp

  Description: Implementation of main entry point of EventHandling example of
               VimbaC.

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

#include <EventHandling.h>

int main( int argc, char* argv[] )
{
    VmbError_t err = VmbErrorSuccess;

    char*           pCameraID   = NULL;         // The ID of the camera to use
    unsigned char   bPrintHelp  = 0;            // Output help?
    int             i           = 0;            // Counter for some iteration
    char*           pParameter  = NULL;         // The command line parameter

    printf( "////////////////////////////////////////\n" );
    printf( "/// Vimba API Event Handling Example ///\n" );
    printf( "////////////////////////////////////////\n\n" );

    //////////////////////
    //Parse command line//
    //////////////////////

    for( i = 1; i < argc; ++i )
    {
        pParameter = argv[i];
        if( 0 == strlen( pParameter ))
        {
            err = VmbErrorBadParameter;
            break;
        }

        if( '/' == pParameter[0] )
        {
            if( 0 == strcmp( pParameter, "/h" ))
            {
                if(( NULL != pCameraID ) ||  ( bPrintHelp ))
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                bPrintHelp = 1;
            }
            else
            {
                err = VmbErrorBadParameter;
                break;
            }
        }
        else
        {
            if( NULL != pCameraID )
            {
                err = VmbErrorBadParameter;
                break;
            }

            pCameraID = pParameter;
        }
    }

    //Write out an error if we could not parse the command line
    if ( VmbErrorBadParameter == err )
    {
        printf( "Invalid parameters!\n\n" );
        bPrintHelp = 1;
    }

    //Print out help and end program
    if ( bPrintHelp )
    {
        printf( "Usage: EventHandling [CameraID] [/h]\n" );
        printf( "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n" );
        printf( "              /h          Print out help\n" );
    }
    else
    {
        err = RunVimbaSystem( pCameraID );
    }

    return err;
}
