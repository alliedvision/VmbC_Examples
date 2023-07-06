/*=============================================================================
  Copyright (C) 2022 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif


unsigned int IpAddressToHostByteOrderedInt(const char* const strIp)
{
    unsigned long ip = inet_addr(strIp);

    if (ip == INADDR_NONE)
    {
        return INADDR_NONE;
    }

#ifdef _LITTLE_ENDIAN
    ip = ntohl(ip);
#endif

    return (unsigned int)ip;
}
