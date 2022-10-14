/**
 * \date 2022
 * \copyright Allied Vision Technologies.  All Rights Reserved.
 *
 * \copyright Redistribution of this file, in original or modified form, without
 *            prior written consent of Allied Vision Technologies is prohibited.
 *
 * \warning THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
 * NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \brief Entry point of the Force IP example using the VmbC API
 */

#include <stdio.h>
#include <string.h>

#include "ForceIpProg.h"

int main( int argc, char* argv[] )
{
    printf("\n/////////////////////////////////\n");
    printf("/// VmbC API Force IP Example ///\n");
    printf("/////////////////////////////////\n\n");

    if (4 > argc
        || 6 < argc)
    {
        printf("Usage: ForceIp_VmbC [tl] <MAC> <IP> <Subnet> [<Gateway>]\n\n");
        printf("Parameters:\n");
        printf("tl         If absent, the operation is performed using the camera's interface.\n");
        printf("           If present, the camera's transport layer is used.\n");
        printf("<MAC>      The MAC address of the camera whose IP address shall be changed.\n");
        printf("           Either hexadecimal with preceding 0x or decimal.\n");
        printf("<IP>       The new IPv4 address of the camera in numbers and dots notation.\n");
        printf("<Subnet>   The new network mask of the camera in numbers and dots notation.\n");
        printf("<Gateway>  The address of a possible gateway if the camera is not connected\n");
        printf("           to the host PC directly.\n\n");
        printf("For example to change the IP address of a camera with the MAC address 0x0F3101D540\nto 169.254.1.1 in a class B network using an interface call:\n\n");
        printf("ForceIp_VmbC 0x0F3101D540 169.254.1.1 255.255.0.0\n\n");

        return 1;
    }

    if (strcmp(argv[1], "tl") == 0)
    {
        return ForceIpProg(argv[1], argv[2], argv[3], argv[4], (argc == 6) ? argv[5] : NULL);
    }
    else
    {
        return ForceIpProg(NULL, argv[1], argv[2], argv[3], (argc == 5) ? argv[4] : NULL);
    }
}
