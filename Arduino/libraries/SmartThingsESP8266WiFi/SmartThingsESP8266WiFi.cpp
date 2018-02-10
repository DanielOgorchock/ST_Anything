//*******************************************************************************
//	SmartThings NodeMCU ESP8266 Wifi Library
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-10  Dan Ogorchock  Created
//  2017-12-29  Dan Ogorchock  Added WiFi.RSSI() data collection
//  2018-01-06  Dan Ogorchock  Simplified the MAC address printout to prevent confusion
//  2018-01-06  Dan Ogorchock  Added OTA update capability
//  2018-02-03  Dan Ogorchock  Support for Hubitat
//*******************************************************************************

#include "SmartThingsESP8266WiFi.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsESP8266WiFI Constructor - Static IP
	//*******************************************************************************
	SmartThingsESP8266WiFi::SmartThingsESP8266WiFi(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, false),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*******************************************************************************
	// SmartThingsESP8266WiFI Constructor - DHCP
	//*******************************************************************************
	SmartThingsESP8266WiFi::SmartThingsESP8266WiFi(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*******************************************************************************
	// SmartThingsESP8266WiFI Constructor - DHCP
	//*******************************************************************************
	SmartThingsESP8266WiFi::SmartThingsESP8266WiFi(uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		st_preExistingConnection = true;
	}

	//*****************************************************************************
	//SmartThingsESP8266WiFI::~SmartThingsESP8266WiFI()
	//*****************************************************************************
	SmartThingsESP8266WiFi::~SmartThingsESP8266WiFi()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsESP8266WiFI Library
	//*******************************************************************************
	void SmartThingsESP8266WiFi::init(void)
	{
		if (!st_preExistingConnection) {
			Serial.println(F(""));
			Serial.println(F("Initializing ESP8266 WiFi network.  Please be patient..."));

			if (st_DHCP == false)
			{
				WiFi.config(st_localIP, st_localGateway, st_localSubnetMask, st_localDNSServer);
			}
			// attempt to connect to WiFi network
			WiFi.begin(st_ssid, st_password);
			Serial.print(F("Attempting to connect to WPA SSID: "));
			Serial.println(st_ssid);
		}

		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(F("."));
			delay(500);	// wait for connection:
		}

		Serial.println();

		st_server.begin();

		Serial.println(F(""));
		Serial.println(F("Enter the following three lines of data into ST App on your phone!"));
		Serial.print(F("localIP = "));
		Serial.println(WiFi.localIP());
		Serial.print(F("serverPort = "));
		Serial.println(st_serverPort);
		Serial.print(F("MAC Address = "));
		String strMAC(WiFi.macAddress());
		strMAC.replace(":", "");
		Serial.println(strMAC);
		Serial.println(F(""));
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

		String("ESP8266_" + strMAC).toCharArray(st_devicename, sizeof(st_devicename));
		WiFi.hostname(st_devicename);

		Serial.println(F(""));
		Serial.println(F("SmartThingsESP8266WiFI: Intialized"));
		Serial.println(F(""));

		//Turn off Wirelss Access Point
		Serial.println(F("Disabling ESP8266 WiFi Access Point"));
		Serial.println(F(""));
		WiFi.mode(WIFI_STA);

		RSSIsendInterval = 5000;
		previousMillis = millis() - RSSIsendInterval;

		// Setup OTA Updates

		// Port defaults to 8266
		// ArduinoOTA.setPort(8266);

		// Hostname defaults to esp8266-[ChipID]
		ArduinoOTA.setHostname(st_devicename);

		// No authentication by default
		//ArduinoOTA.setPassword((const char*)"123");

		ArduinoOTA.onStart([]() {
			Serial.println("Start");
		});
		ArduinoOTA.onEnd([]() {
			Serial.println("\nEnd");
		});
		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
			Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
		});
		ArduinoOTA.onError([](ota_error_t error) {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR) Serial.println("End Failed");
		});
		ArduinoOTA.begin();
		Serial.println("ArduinoOTA Ready");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		Serial.print("ArduionOTA Host Name: ");
		Serial.println(ArduinoOTA.getHostname());
		Serial.println();
	}

	//*****************************************************************************
	// Run SmartThingsESP8266WiFI Library
	//*****************************************************************************
	void SmartThingsESP8266WiFi::run(void)
	{

		ArduinoOTA.handle();

		String readString;
		String tempString;
		String strRSSI;

		if (WiFi.isConnected() == false)
		{
			if (_isDebugEnabled)
			{
				Serial.println(F("**********************************************************"));
				Serial.println(F("**** WiFi Disconnected.  ESP8266 should auto-reconnect ***"));
				Serial.println(F("**********************************************************"));
			}

			//init();
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
	void SmartThingsESP8266WiFi::send(String message)
	{
		if (WiFi.isConnected() == false)
		{
			if (_isDebugEnabled)
			{
				Serial.println(F("**********************************************************"));
				Serial.println(F("**** WiFi Disconnected.  ESP8266 should auto-reconnect ***"));
				Serial.println(F("**********************************************************"));
			}

			//init();
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
				Serial.println(F("**** WiFi Disconnected.  ESP8266 should auto-reconnect ****"));
				Serial.println(F("***********************************************************"));
			}

			//init();      //Re-Init connection to get things working again

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
