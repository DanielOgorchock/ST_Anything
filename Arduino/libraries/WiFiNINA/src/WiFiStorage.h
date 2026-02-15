/*
  WiFiStorage.h - Library for Arduino boards based on NINA WiFi module.
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

#ifndef wifistorage_h
#define wifistorage_h

#include "utility/wifi_drv.h"

class WiFiStorageFile;

class WiFiStorageClass
{
public:
	static bool begin();

	static WiFiStorageFile open(const char *filename);
	static WiFiStorageFile open(String filename);

	static bool exists(const char *filename) {
		uint32_t len;
		return (WiFiDrv::existsFile(filename, strlen(filename), &len) > 0);
	}
	static bool exists(const char *filename, uint32_t* len) {
		return (WiFiDrv::existsFile(filename, strlen(filename), len) > 0);
	}
	static bool remove(const char *filename) {
		WiFiDrv::deleteFile(filename, strlen(filename));
		return true;
	}
	static bool rename(const char * old_file_name, const char * new_file_name) {
		return (WiFiDrv::renameFile(old_file_name, strlen(old_file_name), new_file_name, strlen(new_file_name)) == 0);
	}
	static bool read(const char *filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
		WiFiDrv::readFile(filename, strlen(filename), offset, buffer, buffer_len);
		return true;
	}
	static bool write(const char *filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
		WiFiDrv::writeFile(filename, strlen(filename), offset, buffer, buffer_len);
		return true;
	}
	static bool download(const char* url, const char *filename) {
		WiFiDrv::downloadFile(url, strlen(url), filename, strlen(filename));
		return true;
	}
    static bool downloadOTA(const char * url, uint8_t * res_ota_download = NULL) {
		/* The buffer within the NINA firmware allows a maximum
		 * url size of 128 bytes. It's better to prevent the
		 * transmission of over-sized URL as soon as possible.
		 */
		if (strlen(url) > 128)
			return false;

		uint8_t const res = WiFiDrv::downloadOTA(url, strlen(url));
		if (res_ota_download)
		  *res_ota_download = res;
		bool const success = (res == 0);
        return success;
    }


    static bool remove(String filename) {
    	return remove(filename.c_str());
	}
	static bool rename(String old_file_name, String new_file_name) {
		return rename(old_file_name.c_str(), new_file_name.c_str());
	}
	static bool read(String filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
		return read(filename.c_str(), offset, buffer, buffer_len);
	}
	static bool write(String filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
		return write(filename.c_str(), offset, buffer, buffer_len);
	}
    static bool download(String url, String filename) {
    	return download(url.c_str(), filename.c_str());
    }
    static bool download(String url, uint8_t * res_ota_download = NULL) {
        return downloadOTA(url.c_str(), res_ota_download);
    }
};

extern WiFiStorageClass WiFiStorage;


class WiFiStorageFile
{
public:
	constexpr WiFiStorageFile(const char* _filename) : filename(_filename) { }

	operator bool() {
		return WiFiStorage.exists(filename, &length);
	}
	uint32_t read(void *buf, uint32_t rdlen) {
		if (offset + rdlen > length) {
			if (offset >= length) return 0;
			rdlen = length - offset;
		}
		WiFiStorage.read(filename, offset, (uint8_t*)buf, rdlen);
		offset += rdlen;
		return rdlen;
	}
	uint32_t write(const void *buf, uint32_t wrlen) {
		WiFiStorage.write(filename, offset, (uint8_t*)buf, wrlen);
		offset += wrlen;
		return wrlen;
	}
	void seek(uint32_t n) {
		offset = n;
	}
	uint32_t position() {
		return offset;
	}
	uint32_t size() {
		WiFiStorage.exists(filename, &length);
		return length;
	}
	uint32_t available() {
		WiFiStorage.exists(filename, &length);
		return length - offset;
	}
	void erase() {
		offset = 0;
		WiFiStorage.remove(filename);
	}
	void flush();
	void close() {
		offset = 0;
	}
protected:
	friend class WiFiStorageClass;
	uint32_t offset = 0;
	uint32_t length = 0;
	const char* filename;
};

#endif