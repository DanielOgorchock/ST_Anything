//*******************************************************************************
//	SmartThings Arduino WiFiNINA Library - Use an Arduino MKR 1010 or similar  
//                                        
//
//	License
//	(C) Copyright 2019 Dan Ogorchock
//
//	History
//	2019-06-23  Dan Ogorchock  Created
//  2019-07-01  Dan.t		 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//*******************************************************************************

#include "SmartThingsWiFiNINA.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsWiFiNINA Constructor - Arduino + WiFi - STATIC IP
	//*******************************************************************************
	SmartThingsWiFiNINA::SmartThingsWiFiNINA(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, false),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*****************************************************************************
	// SmartThingsWiFiNINA Constructor - Arduino + WiFi - DHCP
	//*****************************************************************************
	SmartThingsWiFiNINA::SmartThingsWiFiNINA(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}


	//*****************************************************************************
	//SmartThingsWiFiNINA::~SmartThingsWiFiNINA()
	//*****************************************************************************
	SmartThingsWiFiNINA::~SmartThingsWiFiNINA()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsWiFiNINA Library 
	//*******************************************************************************
	void SmartThingsWiFiNINA::init(void)
	{
		int status = WL_IDLE_STATUS;     // the Wifi radio's status

		st::debugPrintln(F(""));
		st::debugPrintln(F("Initializing WiFi NINA network.  Please be patient..."));

		// check for the presence of the shield
		if (WiFi.status() == WL_NO_MODULE) {
			st::debugPrintln("Communication with WiFi module failed!");
			// don't continue
			while (true);
		}

		String fv = WiFi.firmwareVersion();
		if (fv < "1.0.0") {
			st::debugPrintln("Please upgrade the WiFi module's firmware");
		}

		// attempt to connect to WiFi network
		while (status != WL_CONNECTED) {
			st::debugPrint("Attempting to connect to SSID: ");
			st::debugPrintln(st_ssid);
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

		//WiFi.lowPowerMode(); //Reduce power usage when IDLE

		st_server.begin();

		uint8_t mac[6];
		char buf[20];
		IPAddress ip = WiFi.localIP();
		st::debugPrintln(F(""));
		st::debugPrintln(F("Enter the following three lines of data into ST App on your phone!"));
		st::debugPrint(F("localIP = "));
		st::debugPrintln(String(ip));
		st::debugPrint(F("serverPort = "));
		st::debugPrintln(String(st_serverPort));
		WiFi.macAddress(mac);
		st::debugPrint(F("MAC Address = "));
		sprintf(buf, "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
		st::debugPrintln(String(buf));
		st::debugPrintln(F(""));

		st::debugPrint(F("SSID = "));
		st::debugPrintln(st_ssid);
		st::debugPrint(F("PASSWORD = "));
		st::debugPrintln(st_password);
		st::debugPrint(F("hubIP = "));
		st::debugPrintln(String(st_hubIP));
		st::debugPrint(F("hubPort = "));
		st::debugPrintln(String(st_hubPort));
		st::debugPrint(F("RSSI = "));
		st::debugPrintln(String(WiFi.RSSI()));
		st::debugPrintln(F(""));
		st::debugPrintln(F("SmartThingsWiFiNINA: Intialized"));
		st::debugPrintln(F(""));

		RSSIsendInterval = 5000;
		previousMillis = millis() - RSSIsendInterval;
	}

	//*****************************************************************************
	// Run SmartThingsWiFiNINA Library 
	//*****************************************************************************
	void SmartThingsWiFiNINA::run(void)
	{
		String readString;
		String tempString;
		String strRSSI;

		if (WiFi.status() != WL_CONNECTED)
		{
			if (_isDebugEnabled)
			{
				st::debugPrintln(F("**********************************************************"));
				st::debugPrintln(F("**** WiFi Disconnected.  Attempting restart!        ******"));
				st::debugPrintln(F("**********************************************************"));
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
					st::debugPrintln(strRSSI);
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
	void SmartThingsWiFiNINA::send(String message)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			st::debugPrintln(F("**********************************************************"));
			st::debugPrintln(F("**** WiFi Module Disconnected.  Attempting restart! ******"));
			st::debugPrintln(F("**********************************************************"));
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
				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("***** SmartThings.send() - Ethernet Connection Failed *****"));
				st::debugPrintln(F("***********************************************************"));
				st::debugPrint(F("hubIP = "));
				st::debugPrint(String(st_hubIP));
				st::debugPrint(F(" "));
				st::debugPrint(F("hubPort = "));
				st::debugPrintln(String(st_hubPort));

				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("******        Attempting to restart network         *******"));
				st::debugPrintln(F("***********************************************************"));
			}
				
			WiFi.end();  //End current broken WiFi Connection
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

		//if (_isDebugEnabled) { st::debugPrintln(F("WiFi.send(): Reading for reply data "));}
		// read any data returned from the POST
		//while (st_client.connected()) {
			while (st_client.available()) {
				char c = st_client.read(); //gets byte from ethernet buffer
				//if (_isDebugEnabled) { st::debugPrint(String(c)); } //prints byte to serial monitor
			}
		//}

		delay(1);
		st_client.stop();
	}

}