/**
 * \date 2022
 * \copyright Allied Vision Technologies. All Rights Reserved.
 *
 * \copyright Subject to the BSD 3-Clause License.
 *
 * \brief Entry point of the Force IP example using the VmbC API
 */

#include <stdio.h>

#include "ForceIpProg.h"

int main( int argc, char* argv[] )
{
    printf("\n/////////////////////////////////\n");
    printf("/// VmbC API Force IP Example ///\n");
    printf("/////////////////////////////////\n\n");

    if (4 > argc
        || 5 < argc)
    {
        printf("Usage: ForceIp_VmbC <MAC> <IP> <Subnet> [<Gateway>]\n\n");
        printf("Parameters:\n");
        printf("<MAC>      The MAC address of the camera whose IP address shall be changed.\n");
        printf("           Either hexadecimal with preceding 0x or decimal.\n");
        printf("<IP>       The new IPv4 address of the camera in numbers and dots notation.\n");
        printf("<Subnet>   The new network mask of the camera in numbers and dots notation.\n");
        printf("<Gateway>  The address of a possible gateway if the camera is not connected\n");
        printf("           to the host PC directly.\n\n");
        printf("For example to change the IP address of a camera with the MAC address 0x0F3101D540\nto 169.254.1.1 in a class B network call:\n\n");
        printf("ForceIp_VmbC 0x0F3101D540 169.254.1.1 255.255.0.0\n\n");

        return 1;
    }

    return ForceIpProg(argv[1], argv[2], argv[3], (argc == 5) ? argv[4] : NULL);
}
