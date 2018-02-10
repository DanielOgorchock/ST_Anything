//*******************************************************************************
//	SmartThings NodeMCU ESP32 Wifi Library
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-08-15  Dan Ogorchock  Created with the help of Joshua Spain
//  2017-09-05  Dan Ogorchock  Added automatic WiFi reconnect logic as ESP32 
//                             doesn't do this automatically currently
//  2018-01-01  Dan Ogorchock  Added WiFi.RSSI() data collection
//  2018-01-06  Dan Ogorchock  Simplified the MAC address printout to prevent confusion
//  2018-02-03  Dan Ogorchock  Support for Hubitat
//*******************************************************************************

#include "SmartThingsESP32WiFi.h"

namespace st
{
	int SmartThingsESP32WiFi::disconnectCounter = 0;

	//*******************************************************************************
	// SmartThingsESP32WiFi Constructor - Static IP
	//*******************************************************************************
	SmartThingsESP32WiFi::SmartThingsESP32WiFi(String ssid, String password, IPAddress localIP, IPAddress localGateway, IPAddress localSubnetMask, IPAddress localDNSServer, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(localIP, localGateway, localSubnetMask, localDNSServer, serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, false),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*******************************************************************************
	// SmartThingsESP32WiFI Constructor - DHCP
	//*******************************************************************************
	SmartThingsESP32WiFi::SmartThingsESP32WiFi(String ssid, String password, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}

	//*******************************************************************************
	// SmartThingsESP32WiFI Constructor - DHCP
	//*******************************************************************************
	SmartThingsESP32WiFi::SmartThingsESP32WiFi(uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThingsEthernet(serverPort, hubIP, hubPort, callout, shieldType, enableDebug, transmitInterval, true),
		st_server(serverPort)
	{
		st_preExistingConnection = true;
	}

	//*****************************************************************************
	//SmartThingsESP32WiFI::~SmartThingsESP32WiFI()
	//*****************************************************************************
	SmartThingsESP32WiFi::~SmartThingsESP32WiFi()
	{

	}

	//**************************************************************************************
	/// Event Handler for ESP32 WiFi Events (needed to implement reconnect logic for now...)
	//**************************************************************************************
	void SmartThingsESP32WiFi::WiFiEvent(WiFiEvent_t event)
	{
		Serial.printf("[WiFi-event] event: %d\n", event);

		switch (event) {
		case SYSTEM_EVENT_STA_GOT_IP:
			Serial.println("WiFi connected");
			Serial.println("IP address: ");
			Serial.println(WiFi.localIP());
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			Serial.println("WiFi lost connection.  Attempting to reconnect...");
			WiFi.reconnect();
			disconnectCounter++;
			if (disconnectCounter > 10) {
				Serial.println("We have recieved the STA_DISCONNECTED event over 10 times now.  Reboot...");
				ESP.restart();
			}
			break;
		case SYSTEM_EVENT_STA_START:
			Serial.println("ESP32 station start");
			break;
		case SYSTEM_EVENT_STA_CONNECTED:
			Serial.println("ESP32 station connected to AP");
			disconnectCounter = 0;
			break;
		}

	}

	//*******************************************************************************
	/// Initialize SmartThingsESP32WiFI Library
	//*******************************************************************************
	void SmartThingsESP32WiFi::init(void)
	{
		// delete old config
		WiFi.disconnect(true);
		delay(1000);
		WiFi.onEvent(SmartThingsESP32WiFi::WiFiEvent);

		//Turn off Wirelss Access Point
		Serial.println(F("Disabling ESP32 WiFi Access Point"));
		Serial.println(F(""));
		WiFi.mode(WIFI_STA);
		//WiFi.setAutoReconnect(true);
		//WiFi.setAutoConnect(true);

		if (st_DHCP == false)
		{
			WiFi.config(st_localIP, st_localGateway, st_localSubnetMask, st_localDNSServer);
		}

		if (!st_preExistingConnection) {
			Serial.println(F(""));
			Serial.println(F("Initializing ESP32 WiFi network.  Please be patient..."));
			//wait for ESP32 to be ready on startup
			delay(1000); //may not be necessary, but seems to help my test board start up cleanly

			// attempt to connect to WiFi network
			WiFi.begin(st_ssid, st_password);
			Serial.print(F("Attempting to connect to WPA SSID: "));
			Serial.println(st_ssid);
		}

		int count =0;
		while (WiFi.status() != WL_CONNECTED) {
			count++;
			Serial.print(F("."));
			delay(500);	// wait for connection:
			if (count > 10) {
				Serial.println(F("what is taking so long?"));
				count = 0;
			}
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
		Serial.println(F(""));
		Serial.println(F("SmartThingsESP32WiFI: Intialized"));
		Serial.println(F(""));

		RSSIsendInterval = 5000;
		previousMillis = millis() - RSSIsendInterval;

	}

	//*****************************************************************************
	// Run SmartThingsESP32WiFI Library
	//*****************************************************************************
	void SmartThingsESP32WiFi::run(void)
	{
		String readString;
		String tempString;
		String strRSSI;

		if (WiFi.isConnected() == false)
		{
			if (_isDebugEnabled)
			{
				Serial.println(F("**********************************************************"));
				Serial.println(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				Serial.println(F("**********************************************************"));
			}
			//WiFi.reconnect();
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
	void SmartThingsESP32WiFi::send(String message)
	{
		if (WiFi.isConnected() == false)
		{
			if (_isDebugEnabled)
			{
				Serial.println(F("**********************************************************"));
				Serial.println(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				Serial.println(F("**********************************************************"));
			}

			//WiFi.reconnect();
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
				Serial.println(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				Serial.println(F("***********************************************************"));
			}

			//WiFi.reconnect();
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
