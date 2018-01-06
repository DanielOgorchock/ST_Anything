//*******************************************************************************
//	SmartThings Arduino Ethernet Library 
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-04  Dan Ogorchock  Created
//  2017-05-02  Dan Ogorchock  Add support for W5500 Ethernet2 Shield
//  2018-01-06  Dan Ogorchock  Added RSSI Interval as user-definable interval
//*******************************************************************************

#ifndef __SMARTTHINGSETHERNET_H__ 
#define __SMARTTHINGSETHERNET_H__

#include "SmartThings.h"

//Adjust the RSSI Transmit Interval below as you see fit (in milliseconds)
//  Note:  When the board first boots, it transmits frequently, then slows over 
//         time to the interval below.
#define RSSI_TX_INTERVAL 60000

//*******************************************************************************
// Using Ethernet Shield
//*******************************************************************************
#include <IPAddress.h>
/*
//#if defined ARDUINO_ARCH_AVR
//#include <SPI.h>
//#include <Ethernet.h>
//#elif defined ARDUINO_ARCH_ESP8266
//#include <ESP8266WiFi.h>
//#endif
*/
namespace st
{
	class SmartThingsEthernet: public SmartThings
	{
	private:

	protected:
		IPAddress st_localIP;
		IPAddress st_localGateway;
		IPAddress st_localSubnetMask;
		IPAddress st_localDNSServer;
		IPAddress st_hubIP;
		uint16_t st_serverPort;
		uint16_t st_hubPort;
		bool st_DHCP;

	public:

		//*******************************************************************************
		/// @brief  SmartThings Ethernet Constructor - STATIC
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
		SmartThingsEthernet(IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 100, bool DHCP = false);

		//Abrreviated STATIC version for Arduino + ESP-01 WiFiEsp library (no Gateway, Subnet Mask, or DNS can be user provided)
		SmartThingsEthernet(IPAddress localIP, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 100, bool DHCP = false);

		//*******************************************************************************
		/// @brief  SmartThings Ethernet Constructor - DHCP
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsEthernet(uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 100, bool DHCP = true);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsEthernet();

		//*******************************************************************************
		/// Initialize SmartThings Library 
		//*******************************************************************************
		virtual void init(void) = 0; //all derived classes must implement this pure virtual function

		//*******************************************************************************
		/// Run SmartThings Library 
		//*******************************************************************************
		virtual void run(void) = 0; //all derived classes must implement this pure virtual function

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message) = 0; //all derived classes must implement this pure virtual function


	};
}
#endif
