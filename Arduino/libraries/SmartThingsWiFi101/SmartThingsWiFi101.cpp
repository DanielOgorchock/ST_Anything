//*******************************************************************************
//	SmartThings Arduino Wifi101 Library - Use an Arduino with an Arduino WiFi 101 
//                                        shield or Adafruit ATWINC1500
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-05-06  Dan Ogorchock  Created
//  2017-05-29  Dan Ogorchock  Implemented Low Power Mode to reduce power usage/chip heat
//  2018-01-01  Dan Ogorchock  Added WiFi.RSSI() data collection
//  2018-01-06  Dan Ogorchock  Simplified the MAC address printout to prevent confusion
//  2018-02-03  Dan Ogorchock  Support for Hubitat
//*******************************************************************************

#include "SmartThingsWiFi101.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsWiFi101 Constructor - Arduino + WiFi 101 - STATIC IP
	//*******************************************************************************
	SmartThingsWiFi101::SmartThingsWiFi101(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, false),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*****************************************************************************
	// SmartThingsWiFi101 Constructor - Arduino + WiFi 101 - DHCP
	//*****************************************************************************
	SmartThingsWiFi101::SmartThingsWiFi101(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}


	//*****************************************************************************
	//SmartThingsWiFi101::~SmartThingsWiFi101()
	//*****************************************************************************
	SmartThingsWiFi101::~SmartThingsWiFi101()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsWiFi101 Library 
	//*******************************************************************************
	void SmartThingsWiFi101::init(void)
	{
		int status = WL_IDLE_STATUS;     // the Wifi radio's status

		Serial.println(F(""));
		Serial.println(F("Initializing WiFi 101 network.  Please be patient..."));

		// check for the presence of the shield
		if (WiFi.status() == WL_NO_SHIELD) {
			Serial.println("WiFi 101 shield not present");
			// don't continue
			while (true);
		}

		// attempt to connect to WiFi network
		while (status != WL_CONNECTED) {
			Serial.print("Attempting to connect to WPA SSID: ");
			Serial.println(st_ssid);
			// Connect to WPA/WPA2 network
			status = WiFi.begin(st_ssid, st_password);
			// wait 5 seconds for connection:
			delay(5000);
		}

		if (st_DHCP == false)
		{
			// Set the local IP address
			WiFi.config(st_localIP, st_localDNSServer, st_localGateway, st_localSubnetMask);
		}

		WiFi.lowPowerMode(); //Reduce power usage when IDLE

		st_server.begin();

		uint8_t mac[6];
		char buf[20];
		IPAddress ip = WiFi.localIP();
		Serial.println();
		Serial.println(F("Enter the following three lines of data into ST App on your phone!"));
		Serial.print(F("localIP = "));
		Serial.println(ip);
		Serial.print(F("serverPort = "));
		Serial.println(st_serverPort);
		WiFi.macAddress(mac);
		Serial.print(F("MAC Address = "));
		sprintf(buf, "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
		Serial.println(buf);
		Serial.println();

		Serial.print(F("SSID = "));
		Serial.println(st_ssid);
		Serial.print(F("PASSWORD = "));
		Serial.println(st_password);
		Serial.print(F("hubIP = "));
		Serial.println(st_hubIP);
		Serial.print(F("hubPort = "));
		Serial.println(st_hubPort);
		Serial.print(F("RSSI = "));
		Serial.println(WiFi.RSSI());
		Serial.println();
		Serial.println(F("SmartThingsWiFi101: Intialized"));
		Serial.println();

		RSSIsendInterval = 5000;
		previousMillis = millis() - RSSIsendInterval;
	}

	//*****************************************************************************
	// Run SmartThingsWiFi101 Library 
	//*****************************************************************************
	void SmartThingsWiFi101::run(void)
	{
		String readString;
		String tempString;
		String strRSSI;

		if (WiFi.status() != WL_CONNECTED)
		{
			if (_isDebugEnabled)
			{
				Serial.println(F("**********************************************************"));
				Serial.println(F("**** WiFi Disconnected.  Attempting restart!        ******"));
				Serial.println(F("**********************************************************"));
				WiFi.end();
				init();
			}
		}
		else
		{
			if (millis() - previousMillis > RSSIsendInterval)
			{

				previousMillis = millis();

				if (RSSIsendInterval < RSSI_TX_INTERVAL)
				{
					RSSIsendInterval = RSSIsendInterval + 1000;
				}

				strRSSI = String("rssi ") + String(WiFi.RSSI());
				send(strRSSI);

				if (_isDebugEnabled)
				{
					Serial.println(strRSSI);
				}
			}
		}

		WiFiClient client = st_server.available();
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
	void SmartThingsWiFi101::send(String message)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			Serial.println(F("**********************************************************"));
			Serial.println(F("**** WiFi Module Disconnected.  Attempting restart! ******"));
			Serial.println(F("**********************************************************"));
			WiFi.end();
			init();
		}

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
				
			WiFi.end();  //End current broken WiFi Connection
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

		//if (_isDebugEnabled) { Serial.println(F("WiFi.send(): Reading for reply data "));}
		// read any data returned from the POST
		while (st_client.connected()) {
			//while (st_client.available()) {
				char c = st_client.read(); //gets byte from ethernet buffer
				//if (_isDebugEnabled) { Serial.print(c); } //prints byte to serial monitor
			//}
		}

		delay(1);
		st_client.stop();
	}

}