/*
  WiFiUdp.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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

#include <string.h>

#include "utility/WiFiSocket.h"

#include "WiFi101.h"
#include "WiFiUdp.h"

/* Constructor. */
WiFiUDP::WiFiUDP()
{
	_socket = -1;
	_sndSize = 0;
	_parsedPacketSize = 0;
}

/* Start WiFiUDP socket, listening at local port PORT */
uint8_t WiFiUDP::begin(uint16_t port)
{
	struct sockaddr_in addr;
	uint32 u32EnableCallbacks = 0;

	_sndSize = 0;
	_parsedPacketSize = 0;

	// Initialize socket address structure.
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(port);
	addr.sin_addr.s_addr = 0;

	// Open UDP server socket.
	if ((_socket = WiFiSocket.create(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return 0;
	}

	WiFiSocket.setopt(_socket, SOL_SOCKET, SO_SET_UDP_SEND_CALLBACK, &u32EnableCallbacks, 0);

	// Bind socket:
	if (!WiFiSocket.bind(_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
		WiFiSocket.close(_socket);
		_socket = -1;
		return 0;
	}

	return 1;
}

uint8_t WiFiUDP::beginMulticast(IPAddress ip, uint16_t port)
{
	uint32_t multiIp = ip;

	if (!begin(port)) {
		return 0;
	}

	setsockopt(_socket, SOL_SOCKET, IP_ADD_MEMBERSHIP, &multiIp, sizeof(multiIp));

	return 1;
}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int WiFiUDP::available()
{
	if (_socket == -1) {
		return 0;
	}

	if (_parsedPacketSize <= 0) {
		return 0;
	}

	return WiFiSocket.available(_socket);
 }

/* Release any resources being used by this WiFiUDP instance */
void WiFiUDP::stop()
{
	if (_socket == -1) {
		return;
	}

	WiFiSocket.close(_socket);
	_socket = -1;
}

int WiFiUDP::beginPacket(const char *host, uint16_t port)
{
	IPAddress ip;

	if (WiFi.hostByName(host, ip)) {
		return beginPacket(ip, port);
	}

	return 0;
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port)
{
	_sndIP = ip;
	_sndPort = port;
	_sndSize = 0;

	return 1;
}

int WiFiUDP::endPacket()
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = _htons(_sndPort);
	addr.sin_addr.s_addr = _sndIP;

	int result = WiFiSocket.sendto(_socket, (void *)_sndBuffer, _sndSize, 0, (struct sockaddr *)&addr, sizeof(addr));

	return (result <= 0) ? 0 : 1;
}

size_t WiFiUDP::write(uint8_t byte)
{
  return write(&byte, 1);
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size)
{
	if ((size + _sndSize) > sizeof(_sndBuffer)) {
		size = sizeof(_sndBuffer) - _sndSize;
	}

	memcpy(_sndBuffer + _sndSize, buffer, size);

	_sndSize += size;

	return size;
}

int WiFiUDP::parsePacket()
{
	if (_socket == -1) {
		return 0;
	}

	if (_parsedPacketSize > 0) {
		// previously parsed data, discard data
		while (available()) {
			read();
		}
	}

	_parsedPacketSize = WiFiSocket.available(_socket);

	return _parsedPacketSize;
}

int WiFiUDP::read()
{
	uint8_t b;

	if (read(&b, sizeof(b)) != 1) {
		return -1;
	}

	return b;
}

int WiFiUDP::read(unsigned char* buf, size_t size)
{
	// sizeof(size_t) is architecture dependent
	// but we need a 16 bit data type here
	uint16_t size_tmp = available();

	if (size_tmp == 0) {
		return -1;
	}

	if (size < size_tmp) {
		size_tmp = size;
	}

	int result = WiFiSocket.read(_socket, buf, size);

	if (result > 0) {
		_parsedPacketSize -= result;
	}

	return result;
}

int WiFiUDP::peek()
{
	if (!available()) {
		return -1;
	}

	return WiFiSocket.peek(_socket);
}

void WiFiUDP::flush()
{
}

IPAddress WiFiUDP::remoteIP()
{
	if (_socket == -1) {
		return IPAddress(0, 0, 0, 0);
	}

	return WiFiSocket.remoteIP(_socket);
}

uint16_t WiFiUDP::remotePort()
{
	if (_socket == -1) {
		return 0;
	}

	return _htons(WiFiSocket.remotePort(_socket));
}
