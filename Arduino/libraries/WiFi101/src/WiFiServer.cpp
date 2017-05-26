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

extern "C" {
	#include "socket/include/socket.h"
}

#include "WiFi101.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

#define READY	(_flag & SOCKET_BUFFER_FLAG_BIND)

WiFiServer::WiFiServer(uint16_t port)
{
	_port = port;
	_flag = 0;
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

	_flag = 0;

	// Initialize socket address structure.
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(_port);
	addr.sin_addr.s_addr = 0;

	// Open TCP server socket.
	if ((_socket = socket(AF_INET, SOCK_STREAM, opt)) < 0) {
		return 0;
	}

	// Add socket buffer handler:
	socketBufferRegister(_socket, &_flag, 0, 0, 0);

	// Bind socket:
	if (bind(_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
		close(_socket);
		_socket = -1;
		return 0;
	}
	
	// Wait for connection or timeout:
	unsigned long start = millis();
	while (!READY && millis() - start < 2000) {
		m2m_wifi_handle_events(NULL);
	}
	if (!READY) {
		close(_socket);
		_socket = -1;
		return 0;
	}
	_flag &= ~SOCKET_BUFFER_FLAG_BIND;

	return 1;
}

WiFiClient WiFiServer::available(uint8_t* status)
{
	uint32_t flag;
	
	m2m_wifi_handle_events(NULL);
	if (_flag & SOCKET_BUFFER_FLAG_SPAWN) {
		flag = _flag;
		_flag &= ~SOCKET_BUFFER_FLAG_SPAWN_SOCKET_MSK;
		_flag &= ~SOCKET_BUFFER_FLAG_SPAWN;
		if (status != NULL) {
			*status = 0;
		}
		return WiFiClient(((flag & SOCKET_BUFFER_FLAG_SPAWN_SOCKET_MSK) >> SOCKET_BUFFER_FLAG_SPAWN_SOCKET_POS), _socket + 1);
	} else {
		WiFiClient *client;

		for (int sock = 0; sock < TCP_SOCK_MAX; sock++) {
			client = WiFi._client[sock];
			if (client && client->_flag & SOCKET_BUFFER_FLAG_CONNECTED) {
				if (((client->_flag >> SOCKET_BUFFER_FLAG_PARENT_SOCKET_POS) & 0xff) == (uint8)(_socket + 1)) {
					return *client;
				}
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
	size_t n = 0;
	WiFiClient *client;

	for (int sock = 0; sock < TCP_SOCK_MAX; sock++) {
		client = WiFi._client[sock];
		if (client && client->_flag & SOCKET_BUFFER_FLAG_CONNECTED) {
			if (((client->_flag >> SOCKET_BUFFER_FLAG_PARENT_SOCKET_POS) & 0xff) == (uint8)(_socket + 1)) {
				n += client->write(buffer, size);
			}
		}
	}
	return n;
}
