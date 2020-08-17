/*
  Udp.cpp - UDP class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "UDPFast.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <errno.h>
#include "debug.h"
#include "../ethernet/ethernet.h"

//#include “FreeRTOS_sockets.h”
#define FREERTOS_SO_UDP_MAX_RX_PACKETS (16)

#define UDPFASTMTU 3100 //1460

#undef write
#undef read

UDPFast::UDPFast()
    : udp_server(-1), server_port(0), remote_port(0), tx_buffer(0), tx_buffer_len(0), rx_buffer(0)
{
}

UDPFast::~UDPFast()
{
    stop();
}

uint8_t UDPFast::begin(IPAddress address, uint16_t port)
{
    stop();

    server_port = port;

    tx_buffer = new char[1];
    if (!tx_buffer)
    {
        log_e("could not create tx buffer: %d", errno);
        return 0;
    }

    if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        log_e("could not create socket: %d", errno);
        return 0;
    }

    int yes = 1;
    if (setsockopt(udp_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        log_e("could not set socket option: %d", errno);
        stop();
        return 0;
    }

    //  BaseType_t maxFrames = 1;
    //  if (setsockopt(udp_server,SOL_SOCKET,FREERTOS_SO_UDP_MAX_RX_PACKETS,&maxFrames,sizeof(maxFrames)) < 0) {
    //      log_e("could not set socket option: %d", errno);
    //      Serial.printf("Error setting FREERTOS_SO_UDP_MAX_RX_PACKETS %d\n",errno);
    //      stop();
    //      return 0;
    //  }

    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = (in_addr_t)address;
    if (bind(udp_server, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        log_e("could not bind socket: %d", errno);
        stop();
        return 0;
    }
    fcntl(udp_server, F_SETFL, O_NONBLOCK);
    return 1;
}

uint8_t UDPFast::begin(uint16_t p)
{
    return begin(IPAddress(INADDR_ANY), p);
}

uint8_t UDPFast::beginMulticast(IPAddress a, uint16_t p)
{
    if (begin(IPAddress(INADDR_ANY), p))
    {
        if (a != 0)
        {
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = (in_addr_t)a;
            mreq.imr_interface.s_addr = INADDR_ANY;
            if (setsockopt(udp_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
            {
                log_e("could not join igmp: %d", errno);
                stop();
                return 0;
            }
            multicast_ip = a;
        }
        return 1;
    }
    return 0;
}

void UDPFast::stop()
{
    if (tx_buffer)
    {
        delete[] tx_buffer;
        tx_buffer = NULL;
    }
    tx_buffer_len = 0;
    if (rx_buffer)
    {
        cbuf *b = rx_buffer;
        rx_buffer = NULL;
        delete b;
    }
    if (udp_server == -1)
        return;
    if (multicast_ip != 0)
    {
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = (in_addr_t)multicast_ip;
        mreq.imr_interface.s_addr = (in_addr_t)0;
        setsockopt(udp_server, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
        multicast_ip = IPAddress(INADDR_ANY);
    }
    close(udp_server);
    udp_server = -1;
}

int UDPFast::beginMulticastPacket()
{
    if (!server_port || multicast_ip == IPAddress(INADDR_ANY))
        return 0;
    remote_ip = multicast_ip;
    remote_port = server_port;
    return beginPacket();
}

// int UDPFast::beginPacket()
// {
//     if (!remote_port)
//         return 0;

//     // allocate tx_buffer if is necessary
//     if (!tx_buffer)
//     {
//         tx_buffer = new char[1460];
//         if (!tx_buffer)
//         {
//             log_e("could not create tx buffer: %d", errno);
//             return 0;
//         }
//     }

//     tx_buffer_len = 0;

//     // check whereas socket is already open
//     if (udp_server != -1)
//         return 1;

//     if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
//     {
//         log_e("could not create socket: %d", errno);
//         return 0;
//     }

//     fcntl(udp_server, F_SETFL, O_NONBLOCK);

//     return 1;
// }

// int UDPFast::beginPacket(IPAddress ip, uint16_t port)
// {
//     remote_ip = ip;
//     remote_port = port;
//     return beginPacket();
// }

// int UDPFast::beginPacket(const char *host, uint16_t port)
// {
//     struct hostent *server;
//     server = gethostbyname(host);
//     if (server == NULL)
//     {
//         log_e("could not get host from dns: %d", errno);
//         return 0;
//     }
//     return beginPacket(IPAddress((const uint8_t *)(server->h_addr_list[0])), port);
// }

// int UDPFast::endPacket()
// {
//     struct sockaddr_in recipient;
//     recipient.sin_addr.s_addr = (uint32_t)remote_ip;
//     recipient.sin_family = AF_INET;
//     recipient.sin_port = htons(remote_port);
//     int sent = sendto(udp_server, tx_buffer, tx_buffer_len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
//     if (sent < 0)
//     {
//         log_e("could not send data: %d", errno);
//         return 0;
//     }
//     return 1;
// }

int UDPFast::sendPacketFast(const char* hostname, uint16_t port, uint8_t* data, int len)
{
    if (!Ethernet::isConnected())
        return 0;

    if (udp_server == -1)
    {
        if ((udp_server = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            //log_e("could not create socket: %d", errno);
            Debug.println("Cannot open socket");
            return 0;
        }

    }

    IPAddress ip = Ethernet::Resolve(hostname);
    if (ip==IPAddress((uint32_t)0))
        return 0;

    fcntl(udp_server, F_SETFL, O_NONBLOCK);

    struct sockaddr_in recipient;
    recipient.sin_addr.s_addr = ip;
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(port);
    int sent = sendto(udp_server, data, len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
    if (sent < 0)
        return 0;
    return 1;
}

// size_t UDPFast::write(uint8_t data)
// {
//     if (tx_buffer_len == 1460)
//     {
//         endPacket();
//         tx_buffer_len = 0;
//     }
//     tx_buffer[tx_buffer_len++] = data;
//     return 1;
// }

// size_t UDPFast::write(const uint8_t *buffer, size_t size)
// {
//     size_t i;
//     for (i = 0; i < size; i++)
//         write(buffer[i]);
//     return i;
// }

boolean UDPFast::setMaxRxPackets(int value)
{

    BaseType_t xValue = value;

    if (setsockopt(udp_server,
                   SOL_SOCKET,
                   FREERTOS_SO_UDP_MAX_RX_PACKETS,
                   (void *)&xValue,
                   sizeof(xValue)) < 0)
    {
        Serial.printf("could not set socket option: %d\n", errno);
        return false;
    }

    return true;
}

int UDPFast::parsePacketFast(uint8_t *buf)
{
    struct sockaddr_in si_other;
    int slen = sizeof(si_other), len;

    if ((len = recvfrom(udp_server, buf, UDPFASTMTU, MSG_DONTWAIT, (struct sockaddr *)&si_other, (socklen_t *)&slen)) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            return 0;
        }
        log_e("could not receive data: %d", errno);
        return 0;
    }
    return len;
}

boolean UDPFast::setReceiveTimeout(int value)
{

    struct timeval xValue;
    xValue.tv_sec = 0;
    xValue.tv_usec = value * 1000;

    if (setsockopt(udp_server,
                   SOL_SOCKET,
                   SO_RCVTIMEO,
                   (void *)&xValue,
                   sizeof(xValue)) < 0)
    {
        //Serial.printf("could not set socket option setReceiveTimeout: %d\n", errno);
        return false;
    }

    return true;
}

int UDPFast::waitPacketFast(uint8_t *buf)
{
    struct sockaddr_in si_other;
    int slen = sizeof(si_other), len;

    if ((len = recvfrom(udp_server, buf, UDPFASTMTU, 0, (struct sockaddr *)&si_other, (socklen_t *)&slen)) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            //Serial.printf("waitpacket wouldblock: %d\n", errno);
            return -1;
        }
        log_e("could not receive data: %d", errno);
        return 0;
    }
    return len;
}

#define FREERTOS_ZERO_COPY (1)

void UDPFast::flushAllPackets()
{
    //    struct sockaddr_in xSourceAddress;
    //    int slen = sizeof(xSourceAddress);
    //    uint8_t *pucReceivedUDPPayload;
    //    int32_t iReturned=1;
    //
    //  while (iReturned)
    //  {
    //
    //
    //    /* Receive using the zero copy semantics.  The address of the
    //    pucReceivedUDPPayload pointer is passed in the pvBuffer parameter. */
    //    iReturned = recvfrom(
    //                                        /* The socket being received from. */
    //                                        udp_server,
    //                                        /* pucReceivedUDPPayload will get
    //                                        set to points to the received data. */
    //                                        &pucReceivedUDPPayload,
    //                                        /* Ignored because the pvBuffer parameter
    //                                        does not point to a buffer. */
    //                                        0,
    //                                        /* ulFlags with the FREERTOS_ZERO_COPY bit set. */
    //                                        FREERTOS_ZERO_COPY,
    //                                        (struct sockaddr *) &xSourceAddress,
    //                                        (socklen_t *)&slen
    //                                    );
    //
    //    if( iReturned > 0 )
    //    {
    //        /* Data was received from the socket.  Convert the IP address to a
    //        string. */
    ////        FreeRTOS_inet_ntoa( xSourceAddress.sin_addr, ( char * ) cIPAddressString );
    ////
    ////        /* Print out details of the data source. */
    ////        printf( “Received %d bytes from IP address %s port number %drn”,
    ////                    iReturned, /* The number of bytes received. */
    ////                    cIPAddressString, /* The IP address that sent the data. */
    ////                    FreeRTOS_ntohs( xSourceAddress.sin_port ) ); /* The source port. */
    ////
    ////                    /* pucReceivedUDPPayload now points to the received data.  For
    ////        example, *pucReceivedUDPPayload (or pucReceivedUDPPayload[ 0 ]) is the first
    ////        received byte.  *(pucReceivedUDPPayload + 1 ) (or pucReceivedUDPPayload[ 1 ])
    ////        is the second received byte, etc.
    ////
    ////        The application writer is now responsible for the buffer.  To prevent
    ////        memory and network buffer leaks the buffer *must* be returned to the IP
    ////        stack when it is no longer required.  The following call is used to
    ////        return the buffer. */
    //        FreeRTOS_ReleaseUDPPayloadBuffer( ( void * ) pucReceivedUDPPayload );
    //        Serial.println("skipped a packet")
    //    }_
    //  }
}

int UDPFast::parsePacket()
{
    if (rx_buffer)
        return 0;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other), len;
    char *buf = new char[1460];
    if (!buf)
    {
        return 0;
    }
    if ((len = recvfrom(udp_server, buf, 1460, MSG_DONTWAIT, (struct sockaddr *)&si_other, (socklen_t *)&slen)) == -1)
    {
        delete[] buf;
        if (errno == EWOULDBLOCK)
        {
            return 0;
        }
        log_e("could not receive data: %d", errno);
        return 0;
    }
    remote_ip = IPAddress(si_other.sin_addr.s_addr);
    remote_port = ntohs(si_other.sin_port);
    if (len > 0)
    {
        rx_buffer = new cbuf(len);
        rx_buffer->write(buf, len);
    }
    delete[] buf;
    return len;
}

int UDPFast::available()
{
    if (!rx_buffer)
        return 0;
    return rx_buffer->available();
}

int UDPFast::read()
{
    if (!rx_buffer)
        return -1;
    int out = rx_buffer->read();
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int UDPFast::read(unsigned char *buffer, size_t len)
{
    return read((char *)buffer, len);
}

int UDPFast::read(char *buffer, size_t len)
{
    if (!rx_buffer)
        return 0;
    int out = rx_buffer->read(buffer, len);
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int UDPFast::peek()
{
    if (!rx_buffer)
        return -1;
    return rx_buffer->peek();
}

void UDPFast::flush()
{
    if (!rx_buffer)
        return;
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
}

IPAddress UDPFast::remoteIP()
{
    return remote_ip;
}

uint16_t UDPFast::remotePort()
{
    return remote_port;
}