//*******************************************************************************
//	SmartThings Arduino WiFiNINA Library - Use an Arduino MKR 1010, NANO33IoT 
//                                         or similar boards 
//                                        
//
//	License
//	(C) Copyright 2019 Dan Ogorchock
//
//	History
//	2019-06-23  Dan Ogorchock  Created
//  2019-08-17  Dan Ogorchock  NANO33IoT 
//
//*******************************************************************************

#ifndef __SMARTTHINGSWIFININA_H__ 
#define __SMARTTHINGSWIFININA_H__


#include "SmartThingsEthernet.h"

//*******************************************************************************
// Using WiFiNINA library for the Arduino MKR 1010 or similar 
//*******************************************************************************
#include <SPI.h>
#include <WiFiNINA.h>


namespace st
{
	class SmartThingsWiFiNINA: public SmartThingsEthernet
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
		/// @brief  SmartThings Arduino + WiFi Constructor - STATIC IP
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
		SmartThingsWiFiNINA(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "WiFiNINA", bool enableDebug = true, int transmitInterval = 500);

		//*******************************************************************************
		/// @brief  SmartThings Arduino + WiFi Constructor - DHCP
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsWiFiNINA(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "WiFiNINA", bool enableDebug = false, int transmitInterval = 500);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsWiFiNINA();

		//*******************************************************************************
		/// Initialize SmartThingsWiFiNINA Library 
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsWiFiNINA Library 
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif
