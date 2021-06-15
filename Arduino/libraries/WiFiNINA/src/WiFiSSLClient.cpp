/*
  This file is part of the WiFiNINA library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "WiFiSSLClient.h"

WiFiSSLClient::WiFiSSLClient() : 
	WiFiClient()
{
}

WiFiSSLClient::WiFiSSLClient(uint8_t sock) :
	WiFiClient(sock)
{  
}

int WiFiSSLClient::connect(IPAddress ip, uint16_t port)
{
	return WiFiClient::connectSSL(ip, port);
}

int WiFiSSLClient::connect(const char* host, uint16_t port)
{
	return WiFiClient::connectSSL(host, port);
}

WiFiBearSSLClient::WiFiBearSSLClient() :
	WiFiClient()
{
}

WiFiBearSSLClient::WiFiBearSSLClient(uint8_t sock) :
	WiFiClient(sock)
{
}

int WiFiBearSSLClient::connect(IPAddress ip, uint16_t port)
{
	return WiFiClient::connectBearSSL(ip, port);
}

int WiFiBearSSLClient::connect(const char* host, uint16_t port)
{
	return WiFiClient::connectBearSSL(host, port);
}
