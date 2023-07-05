/*=============================================================================
  Copyright (C) 2022 Allied Vision Technologies. All Rights Reserved.
  Subject to the BSD 3-Clause License.
=============================================================================*/

#ifndef IP_ADDRESS_TO_HOST_BYTE_ORDERED_INT_H_
#define IP_ADDRESS_TO_HOST_BYTE_ORDERED_INT_H_

/**
 * \brief Converts a hexadecimal IP address into its decimal representation.
 * \param[in] strIp    The string representation of the IP
 *
 * \return INADDR_NONE in case of error otherwise the decimal representation of the IP address as integer in host byte order
*/
unsigned int IpAddressToHostByteOrderedInt(const char* const strIp);

#endif // IP_ADDRESS_TO_HOST_BYTE_ORDERED_INT_H_
