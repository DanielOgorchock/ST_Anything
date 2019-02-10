/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
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

#include "utility/WiFiSocket.h"

#include "WiFiClient.h"
#include "WiFiServer.h"

WiFiServer::WiFiServer(uint16_t port) :
	_socket(-1)
{
	_port = port;
}

void WiFiServer::begin()
{
	begin(0);
}

uint8_t WiFiServer::beginSSL()
{
	return begin(SOCKET_FLAGS_SSL);
}

uint8_t WiFiServer::begin(uint8_t opt)
{
	struct sockaddr_in addr;

	// Initialize socket address structure.
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(_port);
	addr.sin_addr.s_addr = 0;

	if (_socket != -1 && WiFiSocket.listening(_socket)) {
		WiFiSocket.close(_socket);
		_socket = -1;
	}

	// Open TCP server socket.
	if ((_socket = WiFiSocket.create(AF_INET, SOCK_STREAM, opt)) < 0) {
		return 0;
	}

	// Bind socket:
	if (!WiFiSocket.bind(_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
		WiFiSocket.close(_socket);
		_socket = -1;
		return 0;
	}

	// Listen socket:
	if (!WiFiSocket.listen(_socket, 0)) {
		WiFiSocket.close(_socket);
		_socket = -1;
		return 0;
	}

	return 1;
}

WiFiClient WiFiServer::available(uint8_t* status)
{
	if (status != NULL) {
		*status = 0;
	}

	if (_socket != -1 && !WiFiSocket.listening(_socket)) {
		_socket = -1;
	}

	if (_socket != -1) {
		SOCKET child = WiFiSocket.accepted(_socket);

		if (child > -1) {
			return WiFiClient(child);
		}

		for (SOCKET s = 0; s < TCP_SOCK_MAX; s++) {
			if (WiFiSocket.hasParent(_socket, s) && WiFiSocket.available(s)) {
				return WiFiClient(s);
			}
		}
	}

	return WiFiClient();
}

uint8_t WiFiServer::status() {
	// Deprecated.
	return 0;
}

size_t WiFiServer::write(uint8_t b)
{
	return write(&b, 1);
}

size_t WiFiServer::write(const uint8_t *buffer, size_t size)
{
	if (_socket == -1) {
		return 0;
	}

	size_t n = 0;

	for (int sock = 0; sock < TCP_SOCK_MAX; sock++) {
		if (WiFiSocket.hasParent(_socket, sock)) {
			n += WiFiSocket.write(sock, buffer, size);
		}
	}

	return n;
}
