#include <tchar.h>
#include <iostream>

#include <ListFeatures.h>

int _tmain( int argc, _TCHAR* argv[] )
{
    std::cout << std::endl;
    std::cout << "///////////////////////////////////////////" << std::endl;
    std::cout << "/// AVT Vimba API List Features Example ///" << std::endl;
    std::cout << "///////////////////////////////////////////" << std::endl;
    std::cout << std::endl;

    if( 2 < argc )
    {
        std::cout << "Usage: ListFeatures.exe [CameraID]" << std::endl;
        std::cout << std::endl;
        std::cout << "Parameters:   CameraID    ID of the camera to use (using first camera if not specified)" << std::endl;
    }
    else if ( 2 == argc )
    {
        ListFeatures( (const char*)argv[1] );
    }
    else
    {
        ListFeatures( NULL );
    }

    std::cout << std::endl;
}