#include <tchar.h>
#include <iostream>

#include <ListFeatures.h>

int _tmain( int argc, _TCHAR* argv[] )
{
    std::cout << "//////////////////////////////////////////" << std::endl;
    std::cout << "/// AVT Vimba API List Cameras Example ///" << std::endl;
    std::cout << "//////////////////////////////////////////" << std::endl << std::endl;
    
    ListFeatures();

    std::cout << std::endl << "Press any key to exit";
    return getchar();
}