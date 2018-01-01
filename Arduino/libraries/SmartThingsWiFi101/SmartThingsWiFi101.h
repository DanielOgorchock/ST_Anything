//*******************************************************************************
//	SmartThings Arduino Wifi101 Library - Use an Arduino with an Arduino WiFi 101 
//                                        shield or Adafruit ATWINC1500
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-05-06  Dan Ogorchock  Created
//  2018-01-01  Dan Ogorchock  Added WiFi.RSSI() data collection
//*******************************************************************************

#ifndef __SMARTTHINGSWIFI101_H__ 
#define __SMARTTHINGSWIFI101_H__


#include "SmartThingsEthernet.h"

//*******************************************************************************
// Using WiFi101 library for the Arduino WiFi 101 shield or Adafruit ATWINC1500 
//*******************************************************************************
#include <SPI.h>
#include <WiFi101.h>


namespace st
{
	class SmartThingsWiFi101: public SmartThingsEthernet
	{
	private:
		//WiFi Specific
		char st_ssid[50];
		char st_password[50];
		WiFiServer st_server; //server
		WiFiClient st_client; //client
		long previousMillis;
		long RSSIsendInterval;

	public:

		//*******************************************************************************
		/// @brief  SmartThings Arduino + WiFi 101 Constructor - STATIC IP
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
		SmartThingsWiFi101(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "Wifi101", bool enableDebug = true, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings Arduino + WiFi Constructor 101 - DHCP
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsWiFi101(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "WiFi101", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsWiFi101();

		//*******************************************************************************
		/// Initialize SmartThingsWiFi101 Library 
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsWiFi101 Library 
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif
