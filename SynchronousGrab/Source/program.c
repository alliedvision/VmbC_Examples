/*=============================================================================
  Copyright (C) 2012 - 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.cpp

  Description: Implementation of main entry point of SynchronousGrab example of
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

#include <SynchronousGrab.h>

unsigned char StartsWith(const char *pString, const char *pStart)
{
    if(NULL == pString)
    {
        return 0;
    }
    if(NULL == pStart)
    {
        return 0;
    }

    if(strlen(pString) < strlen(pStart))
    {
        return 0;
    }

    if(memcmp(pString, pStart, strlen(pStart)) != 0)
    {
        return 0;
    }

    return 1;
}

int main( int argc, char* argv[] )
{
    VmbError_t err = VmbErrorSuccess;

    char*           pCameraID   = NULL;         // The ID of the camera to use
    const char*     pFileName   = NULL;         // The filename for the bitmap to save
    unsigned char   bPrintHelp  = 0;            // Output help?
    int             i           = 0;            // Counter for some iteration
    char*           pParameter  = 0;            // The command line parameter

    printf( "//////////////////////////////////////////\n" );
    printf( "/// Vimba API Synchronous Grab Example ///\n" );
    printf( "//////////////////////////////////////////\n\n" );

    //////////////////////
    //Parse command line//
    //////////////////////

    for( i = 1; i < argc; ++i )
    {
        pParameter = argv[i];
        if( 0 > strlen( pParameter ))
        {
            err = VmbErrorBadParameter;
            break;
        }

        if( '/' == pParameter[0] )
        {
            if( StartsWith( pParameter, "/f:" ))
            {
                if( NULL != pFileName )
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                pFileName = pParameter + 3;
                if( 0 >= strlen( pFileName ))
                {
                    err = VmbErrorBadParameter;
                    break;
                }
            }
            else if( 0 == strcmp( pParameter, "/h" ))
            {
                if(     ( NULL != pCameraID )
                    ||  ( NULL != pFileName )
                    ||  ( bPrintHelp ))
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
        printf( "Usage: SynchronousGrab [CameraID] [/h] [/f:FileName]\n" );
        printf( "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n" );
        printf( "              /h          Print out help\n" );
        printf( "              /f:FileName File name for operation\n" );
        printf( "                          (default \"SynchronousGrab.bmp/.dat\" if not specified)\n" );
    }
    else
    {
        if ( NULL == pFileName )
        {
            pFileName = "SynchronousGrab.bmp";
        }

        err = SynchronousGrab( pCameraID, pFileName );
    }

    return err;
}
