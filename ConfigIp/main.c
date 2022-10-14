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
 * \brief Entry point of the Config IP example using the VmbC API
 */

#include <stdio.h>
#include <string.h>

#include "ConfigIpProg.h"

int main(int argc, char* argv[])
{
    printf("\n/////////////////////////////////////////\n");
    printf("/// VmbC API IP Configuration Example ///\n");
    printf("/////////////////////////////////////////\n\n");

    if (argc == 2)
    {
        return ConfigIpProg(argv[1], NULL, NULL, NULL);
    }
    else if (argc == 3 && strcmp(argv[2], "dhcp") == 0)
    {
        return ConfigIpProg(argv[1], argv[2], NULL, NULL);
    }
    else if (argc == 4 || argc == 5)
    {
        return ConfigIpProg(argv[1], argv[2], argv[3], argc == 5 ? argv[4] : NULL);
    }
    else
    {
        printf("Usage: ConfigIp_VmbC <cameraId> [dhcp | <IP> <Subnet> [<Gateway>]]\n\n");
        printf("Parameters:\n");
        printf("<cameraId> The ID of the camera whose IP configuration shall be changed. The camera must be on the hosts subnet.\n");
        printf("dhcp       Enable DHCP in the camera.\n");
        printf("<IP>       For enabling persistent IP: The new IPv4 address of the camera in numbers and dots notation.\n");
        printf("<Subnet>   For enabling persistent IP: The new network mask of the camera in numbers and dots notation.\n");
        printf("<Gateway>  For enabling persistent IP: The address of a possible gateway if the camera is not connected\n");
        printf("                                       to the host PC directly.\n\n");
        printf("For example, to enable persistent IP with the IP address of a camera with the camera ID DEV_000F315B91EF\n");
        printf("set to 169.254.1.1 in a class B network call:\n\n");
        printf("ConfigIp_VmbC DEV_000F315B91EF 169.254.1.1 255.255.0.0\n\n");
        printf("To enable DHCP in the same camera call:\n\n");
        printf("ConfigIp_VmbC DEV_000F315B91EF dhcp\n\n");
        printf("To disable both DHCP and Persistent IP (in which case LLA will be used) call:\n\n");
        printf("ConfigIp_VmbC DEV_000F315B91EF\n\n");

        return 1;
    }
}
