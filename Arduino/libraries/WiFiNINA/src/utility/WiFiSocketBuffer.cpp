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

#include <stdlib.h>
#include <string.h>

#include "utility/server_drv.h"

#include "WiFiSocketBuffer.h"

#define WIFI_SOCKET_NUM_BUFFERS (sizeof(_buffers) / sizeof(_buffers[0]))

#ifdef __AVR__
#define WIFI_SOCKET_BUFFER_SIZE 64
#else
#define WIFI_SOCKET_BUFFER_SIZE 1500
#endif

WiFiSocketBufferClass::WiFiSocketBufferClass()
{
  memset(&_buffers, 0x00, sizeof(_buffers));
}

WiFiSocketBufferClass::~WiFiSocketBufferClass()
{
  for (unsigned int i = 0; i < WIFI_SOCKET_NUM_BUFFERS; i++) {
    close(i);
  }
}

void WiFiSocketBufferClass::close(int socket)
{
  if (_buffers[socket].data) {
    free(_buffers[socket].data);
    _buffers[socket].data = _buffers[socket].head = NULL;
    _buffers[socket].length = 0;
  }
}

int WiFiSocketBufferClass::available(int socket)
{
  if (_buffers[socket].length == 0) {
    if (_buffers[socket].data == NULL) {
      _buffers[socket].data = _buffers[socket].head = (uint8_t*)malloc(WIFI_SOCKET_BUFFER_SIZE);
      _buffers[socket].length = 0;
    }

    // sizeof(size_t) is architecture dependent
    // but we need a 16 bit data type here
    uint16_t size = WIFI_SOCKET_BUFFER_SIZE;
    if (ServerDrv::getDataBuf(socket, _buffers[socket].data, &size)) {
      _buffers[socket].head = _buffers[socket].data;
      _buffers[socket].length = size;
    }
  }

  return _buffers[socket].length;
}

int WiFiSocketBufferClass::peek(int socket)
{
  if (!available(socket)) {
    return -1;
  }

  return *_buffers[socket].head;
}

int WiFiSocketBufferClass::read(int socket, uint8_t* data, size_t length)
{
  int avail = available(socket);

  if (!avail) {
    return 0;
  }

  if (avail < (int)length) {
    length = avail;
  }

  memcpy(data, _buffers[socket].head, length);
  _buffers[socket].head += length;
  _buffers[socket].length -= length;

  return length;
}

WiFiSocketBufferClass WiFiSocketBuffer;
