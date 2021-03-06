/*
 *  Udp.cpp: Library to send/receive UDP packets.
 *
 * NOTE: UDP is fast, but has some important limitations (thanks to Warren Gray for mentioning these)
 * 1) UDP does not guarantee the order in which assembled UDP packets are received. This
 * might not happen often in practice, but in larger network topologies, a UDP
 * packet can be received out of sequence.
 * 2) UDP does not guard against lost packets - so packets *can* disappear without the sender being
 * aware of it. Again, this may not be a concern in practice on small local networks.
 * For more information, see http://www.cafeaulait.org/course/week12/35.html
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#ifndef _WiFiUDPFast_H_
#define _WiFiUDPFast_H_

#define UDP_MAX_RX_PACKETS 1
//#include <FreeRTOS_Sockets.h>

#define UDPFASTMTU 3100 //1460

#include <Arduino.h>
#include <Udp.h>
#include <cbuf.h>

class UDPFast : public UDP {
private:
  int udp_server;
  IPAddress multicast_ip;
  IPAddress remote_ip;
  uint16_t server_port;
  uint16_t remote_port;
  char * tx_buffer;
  size_t tx_buffer_len;
  cbuf * rx_buffer;
public:
  UDPFast();
  ~UDPFast();
  uint8_t begin(IPAddress a, uint16_t p);
  uint8_t begin(uint16_t p);
  uint8_t beginMulticast(IPAddress a, uint16_t p);
  void stop();
  int beginMulticastPacket();
  int beginPacket(){return 0;};
  int beginPacket(IPAddress ip, uint16_t port){return 0;};
  int beginPacket(const char *host, uint16_t port){return 0;};
  int endPacket(){return 0;};
  size_t write(uint8_t){return 0;};
  size_t write(const uint8_t *buffer, size_t size){return 0;};

  int sendPacketFast(IPAddress ip, uint16_t port, uint8_t* data, int len);
  int sendPacketFast(const char* hostname, uint16_t port, uint8_t* data, int len);

  bool setReceiveTimeout(int value);
  //int waitPacketFast(uint8_t* buf, int maxsize=UDPFASTMTU);
  int parsePacketFast(uint8_t* buffer, int maxsize=UDPFASTMTU);
  boolean setMaxRxPackets(int max);
  void flushAllPackets();
  int parsePacket();
  int available();
  int read();
  int read(unsigned char* buffer, size_t len);
  int read(char* buffer, size_t len);
  int peek();
  void flush();
  IPAddress remoteIP();
  uint16_t remotePort();
};

#endif /* _WiFiUDPFastFast_H_ */