//*******************************************************************************
//	SmartThings Arduino Ethernet Library 
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-04  Dan Ogorchock  Created
//  2017-05-02  Dan Ogorchock  Minor tweak to coexist peacefully with newer W5500 shield
//*******************************************************************************

#ifndef __SMARTTHINGSETHERNETW5100_H__ 
#define __SMARTTHINGSETHERNETW5100_H__


#include "SmartThingsEthernet.h"
#include <SPI.h>
#include <Ethernet.h>

//*******************************************************************************
// Using Ethernet Shield
//*******************************************************************************

namespace st
{
	class SmartThingsEthernetW5100: public SmartThingsEthernet
	{
	private:
		//Ethernet W5100 Specific 
		byte st_mac[6];
		EthernetServer st_server; //server
		EthernetClient st_client; //client

	public:

		//*******************************************************************************
		/// @brief  SmartThings Ethernet Constructor 
		///   @param[in] mac[] - MAC Address of the Ethernet Shield, 6 bytes 
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
		SmartThingsEthernetW5100(byte mac[], IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// @brief  SmartThings Ethernet Constructor - DHCP
		///   @param[in] mac[] - MAC Address of the Ethernet Shield, 6 bytes 
		///   @param[in] serverPort - TCP/IP Port of the Arduino
		///   @param[in] hubIP - TCP/IP Address of the ST Hub
		///   @param[in] hubPort - TCP/IP Port of the ST Hub
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsEthernetW5100(byte mac[], uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 100);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsEthernetW5100();

		//*******************************************************************************
		/// Initialize SmartThingsEthernet Library 
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// Run SmartThingsEthernet Library 
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// Send Message to the Hub 
		//*******************************************************************************
		virtual void send(String message);

	};
}
#endif
