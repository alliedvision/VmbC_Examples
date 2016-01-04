/*=============================================================================
  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        program.c

  Description: Main entry point of LoadSaveSettings example of VimbaC.

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

#include <ApiController.h>

int main( int argc, char *argv[] )
{
	VmbError_t err = VmbErrorSuccess;

    printf( "\n" );
	printf( "########################################\n" );
	printf( "#                                      #\n" );
	printf( "# Vimba API Load/Save Settings Example #\n" );
	printf( "#                                      #\n" );
	printf( "########################################\n" );
    printf( "\n" );

	err = StartVimba();
	if( VmbErrorSuccess != err )
	{
		return -1;		
	}
	printf( "--> VimbaC has been started\n" );

	err = OpenCamera();
	if( VmbErrorSuccess != err )
	{
		StopVimba();
		return -1;		
	}
	printf( "--> Camera has been opened\n" );

	err = SaveCameraSettings( "current_settings.xml" );
	if( VmbErrorSuccess != err )
	{
		StopVimba();
		return -1;
	}
	printf( "--> Camera settings have been saved\n" );

	err = LoadCameraDefault();
	if( VmbErrorSuccess != err )
	{
		StopVimba();
		return -1;
	}
	printf( "--> All feature values have been restored to default\n" );

	err = LoadCameraSettings( "current_settings.xml" );
	if( VmbErrorSuccess != err )
	{
		StopVimba();
		return -1;
	}
	printf( "--> Feature values have been loaded from given XML file\n" );

	err = CloseCamera();
	if( VmbErrorSuccess != err )
	{
		StopVimba();
		return -1;
	}
	printf( "--> Camera has been closed\n" );

	StopVimba();
	printf( "--> VimbaC has been stopped\n" );
	
	return 0;
}