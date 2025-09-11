//*******************************************************************************
//	SmartThings Waveshare ESP32-S3-ETH Ethernet Library 
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2025-09-11  Dan Ogorchock  Created
//*******************************************************************************

#ifndef __SMARTTHINGSESP32S3ETH_H__ 
#define __SMARTTHINGSESP32S3ETH_H__

#include <SPI.h>
#include <Ethernet.h>
#include "SmartThingsEthernet.h"

// Define W5500 pin assignments
#define W5500_CS    14  // Chip Select pin
#define W5500_RST    9  // Reset pin
#define W5500_INT   10  // Interrupt pin
#define W5500_MISO  12  // MISO pin
#define W5500_MOSI  11  // MOSI pin
#define W5500_SCK   13  // Clock pin

//*******************************************************************************
// Using Ethernet Shield
//*******************************************************************************

namespace st
{
	class SmartThingsESP32S3ETH: public SmartThingsEthernet
	{
	private:
		//Ethernet W5500 Specific 
		byte st_mac[6];
		EthernetServer st_server; //server
		EthernetClient st_client; //client

	public:

		//*******************************************************************************
		/// @brief  SmartThings Ethernet Constructor - STATIC
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
		SmartThingsESP32S3ETH(byte mac[], IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 500);

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
		SmartThingsESP32S3ETH(byte mac[], uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType = "EthernetShield", bool enableDebug = false, int transmitInterval = 500);

		//*******************************************************************************
		/// Destructor 
		//*******************************************************************************
		~SmartThingsESP32S3ETH();

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
