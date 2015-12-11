/*=============================================================================
  Copyright (C) 2013 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ForceIP.h

  Description: The ForceIP example will assign an IP address to a camera
               that is identified by its MAC address. It utilizes only
               GenTL features. This approach is useful when a camera has
               an invalid IP configuration and cannot be accessed through
               the network anymore.

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

#ifndef FORCE_IP_H_
#define FORCE_IP_H_

//
// Starts Vimba API
// Seeks a GigE camera by its MAC address on the network
// Sets the found camera's 
// - IP address
// - subnet mask
// - gateway
//
// Parameters:
//  [in]    strMAC          The MAC address of the camera to work on in decimal
//                          or hex (with preceding 0x) representation
//  [in]    strIP           The desired IP address for the camera
//  [in]    strSubnet       The desired subnet mask of the IP address
//  [in]    strGateway      The desired gateway. Optional, can be 0
//
void ForceIP( char* strMAC, char* strIP, char* strSubnet, char* strGateway );

#endif
