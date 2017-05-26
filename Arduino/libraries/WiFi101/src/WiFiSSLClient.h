/*
  WiFiSSLClient.cpp - Library for Arduino Wifi shield.
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

#ifndef WIFISSLCLIENT_H
#define WIFISSLCLIENT_H

#include "WiFiClient.h"

class WiFiSSLClient : public WiFiClient {

public:
	WiFiSSLClient();
	WiFiSSLClient(uint8_t sock, uint8_t parentsock = 0);
	WiFiSSLClient(const WiFiSSLClient& other);

	virtual int connect(IPAddress ip, uint16_t port);
	virtual int connect(const char* host, uint16_t port);
};

#endif /* WIFISSLCLIENT_H */
