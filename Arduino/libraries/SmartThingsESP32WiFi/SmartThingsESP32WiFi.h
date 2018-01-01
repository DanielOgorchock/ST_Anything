//*******************************************************************************
//	SmartThings Arduino ESP32 Wifi Library
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-08-15  Dan Ogorchock  Created with the help of Joshua Spain
//  2017-09-05  Dan Ogorchock  Added automatic WiFi reconnect logic as ESP32 
//                             doesn't do this automatically currently
//  2018-01-01  Dan Ogorchock  Added WiFi.RSSI() data collection
//*******************************************************************************

#ifndef __SMARTTHINGSESP32WIFI_H__
#define __SMARTTHINGSESP32WIFI_H__

#include "SmartThingsEthernet.h"

//*******************************************************************************
// Using ESP32 WiFi
//*******************************************************************************
#include <WiFi.h>

namespace st
{
	class SmartThingsESP32WiFi: public SmartThingsEthernet
	{
	private:
		//ESP32 WiFi Specific
		char st_ssid[50];
		char st_password[50];
        static int disconnectCounter;	
		boolean st_preExistingConnection = false;
		WiFiServer st_server; //server
		WiFiClient st_client; //client
		long previousMillis;
		long RSSIsendInterval;

		//**************************************************************************************
		/// Event Handler for ESP32 WiFi Events (needed to implement reconnect logic for now...)
		//**************************************************************************************
		static void WiFiEvent(WiFiEvent_t event);

	public:
		//*******************************************************************************
		/// @brief  SmartThings ESP32 WiFi Constructor - Static IP
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
		SmartThingsESP32WiFi(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP32Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings ESP32 WiFi Constructor - DHCP
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsESP32WiFi(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP32Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings ESP32 WiFi Constructor - Pre-existing connection
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsESP32WiFi(uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "ESP32Wifi", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// Destructor
		//*******************************************************************************
		~SmartThingsESP32WiFi();

		//*******************************************************************************
		/// Initialize SmartThingsESP32WiFI Library
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsESP32WiFI Library
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif
