//*******************************************************************************
//	SmartThings Arduino ESP8266 Wifi Library
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-05  Dan Ogorchock  Created
//  2017-12-29  Dan Ogorchock  Added WiFi.RSSI() data collection
//  2018-01-06  Dan Ogorchock  Added OTA update capability
//*******************************************************************************

#ifndef __SMARTTHINGSESP8266WIFI_H__
#define __SMARTTHINGSESP8266WIFI_H__

#include "SmartThingsEthernet.h"

//*******************************************************************************
// Using ESP8266 WiFi
//*******************************************************************************
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

namespace st
{
	class SmartThingsESP8266WiFi: public SmartThingsEthernet
	{
	private:
		//ESP8266 WiFi Specific
		char st_ssid[50];
		char st_password[50];
		boolean st_preExistingConnection = false;
		WiFiServer st_server; //server
		WiFiClient st_client; //client
		long previousMillis;
		long RSSIsendInterval;
		char st_devicename[50];

	public:

		//*******************************************************************************
		/// @brief  SmartThings ESP8266 WiFi Constructor - Static IP
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] localIP - TCP/IP Address of the Arduino
		///   @param[in] localGateway - TCP/IP Gateway Address of local LAN (your Router's LAN Address)
		///   @param[in] localSubnetMask - Subnet Mask of the Arduino
		///   @param[in] localDNSServer - DNS Server
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsESP8266WiFi(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP8266Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings ESP8266 WiFi Constructor - DHCP
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsESP8266WiFi(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP8266Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings ESP8266 WiFi Constructor - Pre-existing connection
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsESP8266WiFi(uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP8266Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// Destructor
		//*******************************************************************************
		~SmartThingsESP8266WiFi();

		//*******************************************************************************
		/// Initialize SmartThingsESP8266WiFI Library
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsESP8266WiFI Library
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif
