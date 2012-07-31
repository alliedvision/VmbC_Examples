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
    std::cout << "/////////////////////////////////////////////" << std::endl;
    std::cout << "/// AVT Vimba API Synchronous Grab Example ///" << std::endl;
    std::cout << "/////////////////////////////////////////////" << std::endl << std::endl;
    
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

	if ( NULL == pFileName )
	{
		pFileName = "SynchronousGrab.bmp";
	}

    err = SynchronousGrab( pCameraID, pFileName );

	return err;
}