// Port of CC3000 MDNS Responder to WINC1500.
// Author: Tony DiCola
//
// This MDNSResponder class implements just enough MDNS functionality to respond
// to name requests, for example 'foo.local'.  This does not implement any other
// MDNS or Bonjour functionality like services, etc.
//
// Copyright (c) 2016 Adafruit Industries.  All right reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <avr/pgmspace.h>
#ifndef ARDUINO_ARCH_AVR
#include <strings.h>
#endif

#include "Arduino.h"
#include "WiFiMDNSResponder.h"

// Important RFC's for reference:
// - DNS request and response: http://www.ietf.org/rfc/rfc1035.txt
// - Multicast DNS: http://www.ietf.org/rfc/rfc6762.txt

#define HEADER_SIZE 12
#define TTL_OFFSET 4
#define IP_OFFSET 10

const uint8_t expectedRequestHeader[HEADER_SIZE] PROGMEM = {
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x01, // questions (these 2 bytes are ignored)
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00
};

const uint8_t responseHeader[] PROGMEM = {
  0x00, 0x00,   // ID = 0
  0x84, 0x00,   // Flags = response + authoritative answer
  0x00, 0x00,   // Question count = 0
  0x00, 0x01,   // Answer count = 1
  0x00, 0x00,   // Name server records = 0
  0x00, 0x01    // Additional records = 1
};

// Generate positive response for IPV4 address
const uint8_t aRecord[] PROGMEM = {
  0x00, 0x01,                // Type = 1, A record/IPV4 address
  0x80, 0x01,                // Class = Internet, with cache flush bit
  0x00, 0x00, 0x00, 0x00,    // TTL in seconds, to be filled in later
  0x00, 0x04,                // Length of record
  0x00, 0x00, 0x00, 0x00     // IP address, to be filled in later
};

// Generate negative response for IPV6 address (CC3000 doesn't support IPV6)
const uint8_t nsecRecord[] PROGMEM = { 
  0xC0, 0x0C,                // Name offset
  0x00, 0x2F,                // Type = 47, NSEC (overloaded by MDNS)
  0x80, 0x01,                // Class = Internet, with cache flush bit
  0x00, 0x00, 0x00, 0x00,    // TTL in seconds, to be filled in later
  0x00, 0x08,                // Length of record
  0xC0, 0x0C,                // Next domain = offset to FQDN
  0x00,                      // Block number = 0
  0x04,                      // Length of bitmap = 4 bytes
  0x40, 0x00, 0x00, 0x00     // Bitmap value = Only first bit (A record/IPV4) is set
};

const uint8_t domain[] PROGMEM = { 'l', 'o', 'c', 'a', 'l' };

WiFiMDNSResponder::WiFiMDNSResponder() :
  minimumExpectedRequestLength(0)
{
}

WiFiMDNSResponder::~WiFiMDNSResponder()
{
}

bool WiFiMDNSResponder::begin(const char* _name, uint32_t _ttlSeconds)
{
  int nameLength = strlen(_name);

  if (nameLength > 255) {
    // Can only handle domains that are upto 255 chars in length.
    minimumExpectedRequestLength = 0;
    return false;
  }

  name = _name;
  ttlSeconds = _ttlSeconds;

  name.toLowerCase();
  minimumExpectedRequestLength = HEADER_SIZE + 1 + nameLength + 1 + sizeof(domain) + 5;

  // Open the MDNS UDP listening socket on port 5353 with multicast address
  // 224.0.0.251 (0xE00000FB)
  if (!udpSocket.beginMulticast(IPAddress(224, 0, 0, 251), 5353)) {
    return false;
  }

  return true;
}

void WiFiMDNSResponder::poll()
{
  if (parseRequest()) {
    replyToRequest();
  }
}

bool WiFiMDNSResponder::parseRequest()
{
  int packetLength = udpSocket.parsePacket();

  if (packetLength) {
    // check if parsed packet matches expected request length
    if (packetLength < minimumExpectedRequestLength) {
      // it does not, read the full packet in and drop data
      while(udpSocket.available()) {
        udpSocket.read();
      }

      return false;
    }

    // read packet
    uint8_t request[packetLength];
    udpSocket.read(request, packetLength);

    // parse request
    uint8_t requestNameLength   = request[HEADER_SIZE];
    uint8_t* requestName        = &request[HEADER_SIZE + 1];
    uint8_t requestDomainLength = request[HEADER_SIZE + 1 + requestNameLength];
    uint8_t* requestDomain      = &request[HEADER_SIZE + 1 + requestNameLength + 1];
    uint16_t requestQtype;
    uint16_t requestQclass;

    memcpy(&requestQtype, &request[minimumExpectedRequestLength - 4], sizeof(requestQtype));
    memcpy(&requestQclass, &request[minimumExpectedRequestLength - 2], sizeof(requestQclass));

    requestQtype = _ntohs(requestQtype);
    requestQclass = _ntohs(requestQclass);

    // compare request
    if (memcmp_P(request, expectedRequestHeader, 4) == 0 &&                      // request header start match
        memcmp_P(&request[6], &expectedRequestHeader[6], 6) == 0 &&              // request header end match
        requestNameLength == name.length() &&                                    // name length match
        strncasecmp(name.c_str(), (char*)requestName, requestNameLength) == 0 && // name match
        requestDomainLength == sizeof(domain) &&                                 // domain length match
        memcmp_P(requestDomain, domain, requestDomainLength) == 0 &&             // suffix match
        requestQtype == 0x0001 &&                                                // request QType match
        requestQclass == 0x0001) {                                               // request QClass match

      return true;
    }
  }

  return false;
}

void WiFiMDNSResponder::replyToRequest()
{
  int nameLength = name.length();
  int domainLength = sizeof(domain);
  uint32_t ipAddress = WiFi.localIP();
  uint32_t ttl = _htonl(ttlSeconds);

  int responseSize = sizeof(responseHeader) + 1 + nameLength + 1 + domainLength + 1 + sizeof(aRecord) + sizeof(nsecRecord);
  uint8_t response[responseSize];
  uint8_t* r = response;

  // copy header
  memcpy_P(r, responseHeader, sizeof(responseHeader));
  r += sizeof(responseHeader);
  
  // copy name
  *r = nameLength;
  memcpy(r + 1, name.c_str(), nameLength);
  r += (1 + nameLength);

  // copy domain
  *r = domainLength;
  memcpy_P(r + 1, domain, domainLength);
  r += (1 + domainLength);

  // terminator
  *r = 0x00;
  r++;

  // copy A record
  memcpy_P(r, aRecord, sizeof(aRecord));
  memcpy(r + TTL_OFFSET, &ttl, sizeof(ttl));            // replace TTL value
  memcpy(r + IP_OFFSET, &ipAddress, sizeof(ipAddress)); // replace IP address value
  r += sizeof(aRecord);

  // copy NSEC record
  memcpy_P(r, nsecRecord, sizeof(nsecRecord));
  r += sizeof(nsecRecord);

  udpSocket.beginPacket(IPAddress(224, 0, 0, 251), 5353);
  udpSocket.write(response, responseSize);
  udpSocket.endPacket();
}
