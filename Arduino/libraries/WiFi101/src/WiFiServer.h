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

#ifndef WIFISERVER_H
#define WIFISERVER_H

#include <Arduino.h>
#include <Server.h>

class WiFiClient;

class WiFiServer : public Server {

private:
	SOCKET _socket;
	uint32_t _flag;
	uint16_t _port;
	uint8_t begin(uint8_t opt);

public:
	WiFiServer(uint16_t);
	WiFiClient available(uint8_t* status = NULL);
	void begin();
	uint8_t beginSSL();
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);
	uint8_t status();

	using Print::write;

};

#endif /* WIFISERVER_H */
