/*
  WiFi.h - Library for Arduino Wifi shield.
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

#ifndef WIFI_H
#define WIFI_H

#define WIFI_FIRMWARE_LATEST_MODEL_A "19.4.4"
#define WIFI_FIRMWARE_LATEST_MODEL_B "19.5.4"

// for backwards compatibility
#define WIFI_FIRMWARE_REQUIRED WIFI_FIRMWARE_LATEST_MODEL_B

#include <Arduino.h>

extern "C" {
	#include "driver/include/m2m_wifi.h"
}

#include "WiFiClient.h"
#include "WiFiSSLClient.h"
#include "WiFiServer.h"

typedef enum {
	WL_NO_SHIELD = 255,
	WL_IDLE_STATUS = 0,
	WL_NO_SSID_AVAIL,
	WL_SCAN_COMPLETED,
	WL_CONNECTED,
	WL_CONNECT_FAILED,
	WL_CONNECTION_LOST,
	WL_DISCONNECTED,
	WL_AP_LISTENING,
	WL_AP_CONNECTED,
	WL_AP_FAILED,
	WL_PROVISIONING,
	WL_PROVISIONING_FAILED
} wl_status_t;

/* Encryption modes */
enum wl_enc_type {  /* Values map to 802.11 encryption suites... */
	ENC_TYPE_WEP  = M2M_WIFI_SEC_WEP,
	ENC_TYPE_TKIP = M2M_WIFI_SEC_WPA_PSK,
	ENC_TYPE_CCMP = M2M_WIFI_SEC_802_1X,
	/* ... except these two, 7 and 8 are reserved in 802.11-2007 */
	ENC_TYPE_NONE = M2M_WIFI_SEC_OPEN,
	ENC_TYPE_AUTO = M2M_WIFI_SEC_INVALID
};

typedef enum {
	WL_RESET_MODE = 0,
	WL_STA_MODE,
	WL_PROV_MODE,
	WL_AP_MODE
} wl_mode_t;

typedef enum {
	WL_PING_DEST_UNREACHABLE = -1,
	WL_PING_TIMEOUT = -2,
	WL_PING_UNKNOWN_HOST = -3,
	WL_PING_ERROR = -4
} wl_ping_result_t;

class WiFiClass
{
public:
	WiFiClass();

	void setPins(int8_t cs, int8_t irq, int8_t rst, int8_t en = -1);

	int init();
	
	char* firmwareVersion();

	/* Start Wifi connection with WPA/WPA2 encryption.
	 *
	 * param ssid: Pointer to the SSID string.
	 * param key: Key input buffer.
	 */
	uint8_t begin();
	uint8_t begin(const char *ssid);
	uint8_t begin(const char *ssid, uint8_t key_idx, const char* key);
	uint8_t begin(const char *ssid, const char *key);
	uint8_t begin(const String &ssid) { return begin(ssid.c_str()); }
	uint8_t begin(const String &ssid, uint8_t key_idx, const String &key) { return begin(ssid.c_str(), key_idx, key.c_str()); }
	uint8_t begin(const String &ssid, const String &key) { return begin(ssid.c_str(), key.c_str()); }

	/* Start Wifi in Access Point, with open security.
	 * Only one client can connect to the AP at a time.
	 *
	 * param ssid: Pointer to the SSID string.
	 * param channel: Wifi channel to use. Valid values are 1-12.
	 */
	uint8_t beginAP(const char *ssid);
	uint8_t beginAP(const char *ssid, uint8_t channel);
	uint8_t beginAP(const char *ssid, uint8_t key_idx, const char* key);
	uint8_t beginAP(const char *ssid, uint8_t key_idx, const char* key, uint8_t channel);
	uint8_t beginAP(const char *ssid, const char* key);
	uint8_t beginAP(const char *ssid, const char* key, uint8_t channel);

	uint8_t beginProvision();
	uint8_t beginProvision(uint8_t channel);

	uint32_t provisioned();

	void config(IPAddress local_ip);
	void config(IPAddress local_ip, IPAddress dns_server);
	void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
	void config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);

	void hostname(const char* name);

	void disconnect();
	void end();

	uint8_t *macAddress(uint8_t *mac);

	uint32_t localIP();
	uint32_t subnetMask();
	uint32_t gatewayIP();
	char* SSID();
	int32_t RSSI();
	uint8_t encryptionType();
	uint8_t* BSSID(uint8_t* bssid);
	uint8_t* APClientMacAddress(uint8_t* mac);
	int8_t scanNetworks();
	char* SSID(uint8_t pos);
	int32_t RSSI(uint8_t pos);
	uint8_t encryptionType(uint8_t pos);
	uint8_t* BSSID(uint8_t pos, uint8_t* bssid);
	uint8_t channel(uint8_t pos);

	uint8_t status();

	int hostByName(const char* hostname, IPAddress& result);
	int hostByName(const String &hostname, IPAddress& result) { return hostByName(hostname.c_str(), result); }

	int ping(const char* hostname, uint8_t ttl = 128);
	int ping(const String &hostname, uint8_t ttl = 128);
	int ping(IPAddress host, uint8_t ttl = 128);

	unsigned long getTime();

	void refresh(void);

	void lowPowerMode(void);
	void maxLowPowerMode(void);
	void noLowPowerMode(void);

	void handleEvent(uint8_t u8MsgType, void *pvMsg);
	void handleResolve(uint8_t * hostName, uint32_t hostIp);
	void handlePingResponse(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode);

private:
	int _init;
	char _version[9];

	uint32_t _localip;
	uint32_t _submask;
	uint32_t _gateway;
	int _dhcp;
	uint32_t _resolve;
	byte *_remoteMacAddress;
	wl_mode_t _mode;
	wl_status_t _status;
	char _scan_ssid[M2M_MAX_SSID_LEN];
	uint8_t _scan_auth;
	uint8_t _scan_channel;
	char _ssid[M2M_MAX_SSID_LEN];

	uint8_t startConnect(const char *ssid, uint8_t u8SecType, const void *pvAuthInfo);
	uint8_t startAP(const char *ssid, uint8_t u8SecType, const void *pvAuthInfo, uint8_t channel);
	uint8_t* remoteMacAddress(uint8_t* remoteMacAddress);

	uint8_t startProvision(const char *ssid, const char *url, uint8_t channel);
};

extern WiFiClass WiFi;

#endif /* WIFI_H */
