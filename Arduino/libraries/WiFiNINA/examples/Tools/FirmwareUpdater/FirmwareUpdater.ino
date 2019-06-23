/*
  FirmwareUpdater - Firmware Updater for the 
  Arduino MKR WiFi 1010, Arduino MKR Vidor 4000, and Arduino UNO WiFi Rev.2.
  
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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ESP32BootROM.h"

typedef struct __attribute__((__packed__)) {
  uint8_t command;
  uint32_t address;
  uint32_t arg1;
  uint16_t payloadLength;

  // payloadLenght bytes of data follows...
} UartPacket;

static const int MAX_PAYLOAD_SIZE = 1024;

#define CMD_READ_FLASH        0x01
#define CMD_WRITE_FLASH       0x02
#define CMD_ERASE_FLASH       0x03
#define CMD_MD5_FLASH         0x04
#define CMD_MAX_PAYLOAD_SIZE  0x50
#define CMD_HELLO             0x99

void setup() {
  Serial.begin(1000000);

  if (!ESP32BootROM.begin(921600)) {
    Serial.println("Unable to communicate with ESP32 boot ROM!");
    while (1);
  }
}

void receivePacket(UartPacket *pkt, uint8_t *payload) {
  // Read command
  uint8_t *p = reinterpret_cast<uint8_t *>(pkt);
  uint16_t l = sizeof(UartPacket);
  while (l > 0) {
    int c = Serial.read();
    if (c == -1)
      continue;
    *p++ = c;
    l--;
  }

  // Convert parameters from network byte order to cpu byte order
  pkt->address = fromNetwork32(pkt->address);
  pkt->arg1 = fromNetwork32(pkt->arg1);
  pkt->payloadLength = fromNetwork16(pkt->payloadLength);

  // Read payload
  l = pkt->payloadLength;
  while (l > 0) {
    int c = Serial.read();
    if (c == -1)
      continue;
    *payload++ = c;
    l--;
  }
}

// Allocated statically so the compiler can tell us
// about the amount of used RAM
static UartPacket pkt;
static uint8_t payload[MAX_PAYLOAD_SIZE];

void loop() {
  receivePacket(&pkt, payload);

  if (pkt.command == CMD_HELLO) {
    if (pkt.address == 0x11223344 && pkt.arg1 == 0x55667788)
      Serial.print("v10000");
  }

  if (pkt.command == CMD_MAX_PAYLOAD_SIZE) {
    uint16_t res = toNetwork16(MAX_PAYLOAD_SIZE);
    Serial.write(reinterpret_cast<uint8_t *>(&res), sizeof(res));
  }

  if (pkt.command == CMD_READ_FLASH) {
    // not supported!
    Serial.println("ER");
  }

  if (pkt.command == CMD_WRITE_FLASH) {
    uint32_t len = pkt.payloadLength;
    if (!ESP32BootROM.dataFlash(payload, len)) {
      Serial.print("ER");
    } else {
      Serial.print("OK");
    }
  }

  if (pkt.command == CMD_ERASE_FLASH) {
    uint32_t address = pkt.address;
    uint32_t len = pkt.arg1;
    if (!ESP32BootROM.beginFlash(address, len, MAX_PAYLOAD_SIZE)) {
      Serial.print("ER");
    } else {
      Serial.print("OK");
    }
  }

  if (pkt.command == CMD_MD5_FLASH) {
    uint32_t address = pkt.address;
    uint32_t len = pkt.arg1;

    if (!ESP32BootROM.endFlash(1)) {
      Serial.print("ER");
    } else {
      ESP32BootROM.end();

      uint8_t md5[16];

      if (!ESP32BootROM.begin(921600)) {
        Serial.print("ER");
      } else if (!ESP32BootROM.md5Flash(address, len, md5)) {
        Serial.print("ER");
      } else {
        Serial.print("OK");
        Serial.write(md5, sizeof(md5));
      }
    }
  }
}
