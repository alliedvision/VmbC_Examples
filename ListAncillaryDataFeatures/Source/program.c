/*=============================================================================
  Copyright (C) 2014 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.c

  Description: Main entry point of ListAncillaryDataFeatures example of VimbaC.

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

#include <ListAncillaryDataFeatures.h>

int main( int argc, char* argv[] )
{
    printf( "\n" );
    printf( "//////////////////////////////////////////////////////\n" );
    printf( "/// Vimba API List Ancillary Data Features Example ///\n" );
    printf( "//////////////////////////////////////////////////////\n" );
    printf( "\n" );

    if( 2 < argc )
    {
        printf( "Usage: ListAncillaryDataFeatures [CameraID]\n\n" );
        printf( "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)\n" );
    }
    else if( 2 == argc )
    {
        ListAncillaryDataFeatures( (const char*)argv[1] );
    }
    else
    {
        ListAncillaryDataFeatures( NULL );
    }

    printf( "\n" );
}
