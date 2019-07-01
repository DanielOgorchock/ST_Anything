//*******************************************************************************
//	SmartThings Arduino Ethernet Library 
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-04  Dan Ogorchock  Created
//  2018-01-06  Dan Ogorchock  Simplified the MAC address printout to prevent confusion
//  2018-02-03  Dan Ogorchock  Support for Hubitat
//  2019-07-01  Dan.t		 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//*******************************************************************************

#include "SmartThingsEthernetW5100.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsEthernet Constructor  
	//*******************************************************************************
	SmartThingsEthernetW5100::SmartThingsEthernetW5100(byte mac[], IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval),
		st_server(serverPort)
	{
		//make a local copy of the MAC address
		for (byte x = 0; x <= 5; x++)
		{
			st_mac[x] = mac[x];
    	}
	}

	//*******************************************************************************
	// SmartThingsEthernet Constructor - DHCP 
	//*******************************************************************************
	SmartThingsEthernetW5100::SmartThingsEthernetW5100(byte mac[], uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		//make a local copy of the MAC address
		for (byte x = 0; x <= 5; x++)
		{
			st_mac[x] = mac[x];
		}
	}

	//*****************************************************************************
	//SmartThingsEthernet::~SmartThingsEthernet()
	//*****************************************************************************
	SmartThingsEthernetW5100::~SmartThingsEthernetW5100()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsEthernet Library 
	//*******************************************************************************
	void SmartThingsEthernetW5100::init(void)
	{
		char buf[20];

		// give the ethernet module time to boot up:
		delay(1000);

		if (st_DHCP == false)
		{
			Ethernet.begin(st_mac, st_localIP, st_localDNSServer, st_localGateway, st_localSubnetMask);
		}
		else
		{
			Ethernet.begin(st_mac);
		}

		st_server.begin();


		//if (_isDebugEnabled)
		//{
		st::debugPrintln(F(""));
		st::debugPrintln(F("Enter the following three lines of data into ST App on your phone!"));
		st::debugPrint(F("localIP = "));
		st::debugPrintln(String(Ethernet.localIP()));
		st::debugPrint(F("serverPort = "));
		st::debugPrintln(String(st_serverPort));
		st::debugPrint(F("MAC Address = "));
		sprintf(buf, "%02X%02X%02X%02X%02X%02X", st_mac[0], st_mac[1], st_mac[2], st_mac[3], st_mac[4], st_mac[5]);
		st::debugPrintln(buf);
		st::debugPrintln(F(""));

		st::debugPrint(F("hubIP = "));
		st::debugPrintln(String(st_hubIP));
		st::debugPrint(F("hubPort = "));
		st::debugPrintln(String(st_hubPort));

		st::debugPrintln(F(""));
		st::debugPrintln(F("SmartThingsEthernet: Intialized"));
		st::debugPrintln(F(""));
		//}
	}

	//*****************************************************************************
	// Run SmartThingsEthernet Library 
	//*****************************************************************************
	void SmartThingsEthernetW5100::run(void)
	{
		String readString;
		String tempString;

		if (st_DHCP) { Ethernet.maintain(); }  //Renew DHCP lease if necessary

		EthernetClient client = st_server.available();
		if (client) {
			boolean currentLineIsBlank = true;
			while (client.connected()) {
				if (client.available()) {
					char c = client.read();
					//read char by char HTTP request
					if (readString.length() < 200) {
						//store characters to string
						readString += c;
					}
					else
					{
						if (_isDebugEnabled)
						{
							st::debugPrintln(F(""));
							st::debugPrintln(F("SmartThings.run() - Exceeded 200 character limit"));
							st::debugPrintln(F(""));
						}
					}
					// if you've gotten to the end of the line (received a newline
					// character) and the line is blank, the http request has ended,
					// so you can send a reply
					if (c == '\n' && currentLineIsBlank) {
						//now output HTML data header
						tempString = readString.substring(readString.indexOf('/') + 1, readString.indexOf('?'));

						if (tempString.length() > 0) {
							client.println(F("HTTP/1.1 200 OK")); //send new page
							client.println();
						}
						else {
							client.println(F("HTTP/1.1 204 No Content"));
							client.println();
							client.println();
							if (_isDebugEnabled)
							{
								st::debugPrintln(F("No Valid Data Received"));
							}
						}
						break;
					}
					if (c == '\n') {
						// you're starting a new line
						currentLineIsBlank = true;
					}
					else if (c != '\r') {
						// you've gotten a character on the current line
						currentLineIsBlank = false;
					}
				}
			}

			delay(1);
			//stopping client
			client.stop();

			//Handle the received data after cleaning up the network connection
			if (tempString.length() > 0) {
				if (_isDebugEnabled)
				{
					st::debugPrint(F("Handling request from ST. tempString = "));
					st::debugPrintln(tempString);
				}
				//Pass the message to user's SmartThings callout function
				tempString.replace("%20", " ");  //Clean up for Hubitat
				_calloutFunction(tempString);
			}

			readString = "";
			tempString = "";
		}
	}

	//*******************************************************************************
	/// Send Message out over Ethernet to the Hub 
	//*******************************************************************************
	void SmartThingsEthernetW5100::send(String message)
	{
		//Make sure the client is stopped, to free up socket for new conenction
		st_client.stop();

		if (st_client.connect(st_hubIP, st_hubPort))
		{
			st_client.println(F("POST / HTTP/1.1"));
			st_client.print(F("HOST: "));
			st_client.print(st_hubIP);
			st_client.print(F(":"));
			st_client.println(st_hubPort);
			st_client.println(F("CONTENT-TYPE: text"));
			st_client.print(F("CONTENT-LENGTH: "));
			st_client.println(message.length());
			st_client.println();
			st_client.println(message);
		}
		else
		{
			//connection failed;
			if (_isDebugEnabled)
			{
				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("***** SmartThings.send() - Ethernet Connection Failed *****"));
				st::debugPrintln(F("***********************************************************"));
				st::debugPrint(F("hubIP = "));
				st::debugPrint(String(st_hubIP));
				Serial.print(F(" "));
				st::debugPrint(F("hubPort = "));
				st::debugPrintln(String(st_hubPort));

				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("******        Attempting to restart network         *******"));
				st::debugPrintln(F("***********************************************************"));
			}


			init();      //Re-Init connection to get things working again

			if (_isDebugEnabled)
			{
				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("******        Attempting to resend missed data      *******"));
				st::debugPrintln(F("***********************************************************"));
			}


			st_client.flush();
			st_client.stop();
			if (st_client.connect(st_hubIP, st_hubPort))
			{
				st_client.println(F("POST / HTTP/1.1"));
				st_client.print(F("HOST: "));
				st_client.print(st_hubIP);
				st_client.print(F(":"));
				st_client.println(st_hubPort);
				st_client.println(F("CONTENT-TYPE: text"));
				st_client.print(F("CONTENT-LENGTH: "));
				st_client.println(message.length());
				st_client.println();
				st_client.println(message);
			}

		}

		//if (_isDebugEnabled) { st::debugPrintln(F("Ethernet.send(): Reading for reply data "));}
		// read any data returned from the POST
		while (st_client.connected()) {
			//while (st_client.available()) {
			char c = st_client.read(); //gets byte from ethernet buffer
									   //if (_isDebugEnabled) { st::debugPrint(String(c)); } //prints byte to serial monitor
									   //}
		}

		delay(1);
		st_client.stop();
	}

}