//*******************************************************************************
//	SmartThings Waveshare ESP32-S3-ETH Ethernet Library 
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2025-09-11  Dan Ogorchock  Created
//*******************************************************************************


#include "SmartThingsESP32S3ETH.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsEthernet Constructor - STATIC 
	//*******************************************************************************
	SmartThingsESP32S3ETH::SmartThingsESP32S3ETH(byte mac[], IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, false),
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
	SmartThingsESP32S3ETH::SmartThingsESP32S3ETH(byte mac[], uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
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
	SmartThingsESP32S3ETH::~SmartThingsESP32S3ETH()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsEthernet Library 
	//*******************************************************************************
	void SmartThingsESP32S3ETH::init(void)
	{
		char buf[20];

		// give the ethernet module time to boot up:
		delay(1000);

		// Initialize SPI with specified pin configuration
		SPI.begin(W5500_SCK, W5500_MISO, W5500_MOSI, W5500_CS);

		Ethernet.init(W5500_CS);
		
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
		Serial.println(F(""));
		Serial.println(F("Enter the following IP address and Port into Hubitat HubDuino Parent Device Settings Page"));
		Serial.print(F("localIP = "));
		Serial.println(Ethernet.localIP());
		Serial.print(F("serverPort = "));
		Serial.println(st_serverPort);
		Serial.print(F("MAC Address = "));
		sprintf(buf, "%02X%02X%02X%02X%02X%02X", st_mac[0], st_mac[1], st_mac[2], st_mac[3], st_mac[4], st_mac[5]);
		Serial.println(buf);
		Serial.println(F(""));

		Serial.print(F("hubIP = "));
		Serial.println(st_hubIP);
		Serial.print(F("hubPort = "));
		Serial.println(st_hubPort);

		Serial.println(F(""));
		Serial.println(F("SmartThingsESP32S3ETH: Intialized"));
		Serial.println(F(""));
		//}
	}

	//*****************************************************************************
	// Run SmartThingsEthernet Library 
	//*****************************************************************************
	void SmartThingsESP32S3ETH::run(void)
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
							Serial.println(F(""));
							Serial.println(F("SmartThings.run() - Exceeded 200 character limit"));
							Serial.println(F(""));
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
								Serial.println(F("No Valid Data Received"));
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
					Serial.print(F("Handling request from ST. tempString = "));
					Serial.println(tempString);
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
	void SmartThingsESP32S3ETH::send(String message)
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
				Serial.println(F("***********************************************************"));
				Serial.println(F("***** SmartThings.send() - Ethernet Connection Failed *****"));
				Serial.println(F("***********************************************************"));
				Serial.print(F("hubIP = "));
				Serial.print(st_hubIP);
				Serial.print(F(" "));
				Serial.print(F("hubPort = "));
				Serial.println(st_hubPort);

				Serial.println(F("***********************************************************"));
				Serial.println(F("******        Attempting to restart network         *******"));
				Serial.println(F("***********************************************************"));
			}


			init();      //Re-Init connection to get things working again

			if (_isDebugEnabled)
			{
				Serial.println(F("***********************************************************"));
				Serial.println(F("******        Attempting to resend missed data      *******"));
				Serial.println(F("***********************************************************"));
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

		//if (_isDebugEnabled) { Serial.println(F("Ethernet.send(): Reading for reply data "));}
		// read any data returned from the POST
		//while (st_client.connected()) {
	        while (st_client.available()) {
			char c = st_client.read(); //gets byte from ethernet buffer
									   //if (_isDebugEnabled) { Serial.print(c); } //prints byte to serial monitor
									   //}
		}

		delay(1);
		st_client.stop();
	}

}
