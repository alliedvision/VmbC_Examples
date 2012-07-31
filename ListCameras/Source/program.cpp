#include <tchar.h>
#include <iostream>

#include <ListCameras.h>

int _tmain( int argc, _TCHAR* argv[] )
{
    std::cout << "//////////////////////////////////////////" << std::endl;
    std::cout << "/// AVT Vimba API List Cameras Example ///" << std::endl;
    std::cout << "//////////////////////////////////////////" << std::endl << std::endl;

    if ( 1 < argc )
    {
        std::cout << "No parameters expected. Execution will not be affected by the provided parameter(s)." << std::endl << std::endl;
    }
    
    ListCameras();
    
    return 0;
}