/*******************************************************************************
* Copyright (c) 2014 IBM Corp.
*
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* and the Eclipse Distribution License is available at
*   http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    Ian Craggs - initial API and implementation and/or initial documentation
*    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
*******************************************************************************/

//#include <sys/types.h>
#include "includes.h"

#if !defined(SOCKET_ERROR)
/** error in socket operation */
#define SOCKET_ERROR -1
#endif

//#if defined(WIN32)
///* default on Windows is 64 - increase to make Linux and Windows the same */
//#define FD_SETSIZE 1024
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#define MAXHOSTNAMELEN 256
//#define EAGAIN WSAEWOULDBLOCK
//#define EINTR WSAEINTR
//#define EINVAL WSAEINVAL
//#define EINPROGRESS WSAEINPROGRESS
//#define EWOULDBLOCK WSAEWOULDBLOCK
//#define ENOTCONN WSAENOTCONN
//#define ECONNRESET WSAECONNRESET
//#define ioctl ioctlsocket
//#define socklen_t int
//#else
#define INVALID_SOCKET SOCKET_ERROR
//#include <sys/socket.h>
//#include <sys/param.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <fcntl.h>
//#include <string.h>
//#include <stdlib.h>
//#endif

//#if defined(WIN32)
//#include <Iphlpapi.h>
//#else
//#include <sys/ioctl.h>
//#include <net/if.h>
//#endif

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
//static int mysock = INVALID_SOCKET;
//char LineBuf[128];



int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
  int rc = 0;
  
  if(Esp8266_GetTcpStatus())
  {
    rc = Esp8266_Tcp_Send(sock, buf, buflen);
  }
  else
  {
    transport_close(0);
    rc = -1;
  }
  return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
  int rc = MQTT_RB_Read(buf, count);
  return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
  //    int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
  /* this call will return after the timeout set on initialization if no bytes;
  in your system you will use whatever you use to get whichever outstanding
  bytes your socket equivalent has ready to be extracted right now, if any,
  or return immediately */
  int rc;
  //	int rc = recv(sock, buf, count, 0);	//5555
  if (rc == -1) 
  {
    /* check error conditions from your system here, and return -1 */
    return 0;
  }
  return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
//int transport_open(char* addr, int port)
//{
//    int sock;
//    sock = GPRS_Tcp_Connect(addr, port);
//    return sock;
//}


int transport_close(int sock)
{
  int rc = 0;
  if(g_esp_status_t.esp_net_work_e == ESP_NETWORK_SUCCESS)
  {
    rc = 1;
  }
  return rc;
}

