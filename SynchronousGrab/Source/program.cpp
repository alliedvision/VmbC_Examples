/*=============================================================================

  Copyright (C) 2012 Allied Vision Technologies.  All Rights Reserved.

 

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

#include <iostream>

#include <SynchronousGrab.h>

bool StartsWith(const char *pString, const char *pStart)
{
    if(NULL == pString)
    {
        return false;
    }
    if(NULL == pStart)
    {
        return false;
    }

    if(strlen(pString) < strlen(pStart))
    {
        return false;
    }

    if(memcmp(pString, pStart, strlen(pStart)) != 0)
    {
        return false;
    }

    return true;
}

int main( int argc, char* argv[] )
{
    std::cout << "//////////////////////////////////////////////" << std::endl;
    std::cout << "/// AVT Vimba API Synchronous Grab Example ///" << std::endl;
    std::cout << "//////////////////////////////////////////////" << std::endl << std::endl;
    
    VmbError_t err = VmbErrorSuccess;

    char *pCameraID = NULL;
    char *pFileName = NULL;
    bool bPrintHelp = false;

    //////////////////////
    //Parse command line//
    //////////////////////

    for(int i = 1; i < argc; i++)
    {
        char *pParameter = argv[i];
        if(strlen(pParameter) < 0)
        {
            err = VmbErrorBadParameter;
            break;
        }

        if(pParameter[0] == '/')
        {
            if(StartsWith(pParameter, "/f:"))
            {
                if(NULL != pFileName)
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                pFileName = pParameter + 3;
                if(strlen(pFileName) <= 0)
                {
                    err = VmbErrorBadParameter;
                    break;
                }
            }
            else if(strcmp(pParameter, "/h") == 0)
            {
                if(     ( pCameraID != NULL )
                    ||  ( pFileName != NULL )
                    ||  ( true == bPrintHelp ))
                {
                    err = VmbErrorBadParameter;
                    break;
                }

                bPrintHelp = true;
            }
            else
            {
                err = VmbErrorBadParameter;
                break;
            }
        }
        else
        {
            if(NULL != pCameraID)
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
        std::cout << "Invalid parameters!" << std::endl << std::endl;
        bPrintHelp = true;
    }

    //Print out help and end program
    if ( true == bPrintHelp )
    {
        std::cout << "Usage: SynchronousGrab.exe [CameraID] [/h] [/f:FileName]" << std::endl;
        std::cout << "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)" << std::endl;
        std::cout << "              /h          Print out help" << std::endl;
        std::cout << "              /f:FileName File name for operation" << std::endl;
        std::cout << "                          (default is \"SynchronousGrab.bmp\" if not specified)" << std::endl;
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