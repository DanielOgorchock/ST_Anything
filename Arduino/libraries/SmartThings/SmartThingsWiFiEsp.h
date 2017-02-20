//*******************************************************************************
//	SmartThings Arduino WifiEsp Library - Use an Arduino with an ESP-01 for WiFi
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-20  Dan Ogorchock  Created
//*******************************************************************************


#ifndef __SMARTTHINGSWIFIESP_H__ 
#define __SMARTTHINGSWIFIESP_H__


#include "SmartThingsEthernet.h"

//*******************************************************************************
// Using WiFiEsp library for the ESP-01 board
//*******************************************************************************
#include <WiFiEsp.h>


namespace st
{
	class SmartThingsWiFiEsp: public SmartThingsEthernet
	{
	private:
		//WiFi Specific
		char st_ssid[50];
		char st_password[50];
		WiFiEspServer st_server; //server
		WiFiEspClient st_client; //client
		Stream* st_espSerial;    //Serial UART used to commincate with the ESP-01 board

	public:

		//*******************************************************************************
		/// @brief  SmartThings Arduino + ESP-01 WiFi Constructor
		///   @param[in] espSerial - Pointer to an initialized Hardware Serial device (e.g. Serial1, Serial2, or Serial3)
		///   @param[in] ssid - Wifi Network SSID
		///   @param[in] password - Wifi Network Password
		///   @param[in] localIP - TCP/IP Address of the Arduino
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsWiFiEsp(Stream *espSerial, String ssid, String password, IPAddress localIP, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "WiFiESP", bool enableDebug = false);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsWiFiEsp();

		//*******************************************************************************
		/// Initialize SmartThingsWiFiEsp Library 
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsWiFiEsp Library 
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif

