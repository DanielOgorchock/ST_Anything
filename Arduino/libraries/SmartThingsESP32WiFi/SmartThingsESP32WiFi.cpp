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
//  2019-07-01  Dan.t		 Added support for websocket Logging, st::debugPrint and st::debugPrintln
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
		st::debugPrint("[WiFi-event] event: ");
		st::debugPrintln(String(event));
		
		switch (event) {
		case SYSTEM_EVENT_STA_GOT_IP:
			st::debugPrintln("WiFi connected");
			st::debugPrintln("IP address: ");
			st::debugPrintln(WiFi.localIP());
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			st::debugPrintln("WiFi lost connection.  Attempting to reconnect...");
			WiFi.reconnect();
			disconnectCounter++;
			if (disconnectCounter > 10) {
				st::debugPrintln("We have recieved the STA_DISCONNECTED event over 10 times now.  Reboot...");
				ESP.restart();
			}
			break;
		case SYSTEM_EVENT_STA_START:
			st::debugPrintln("ESP32 station start");
			break;
		case SYSTEM_EVENT_STA_CONNECTED:
			st::debugPrintln("ESP32 station connected to AP");
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
		st::debugPrintln(F("Disabling ESP32 WiFi Access Point"));
		st::debugPrintln(F(""));
		WiFi.mode(WIFI_STA);
		//WiFi.setAutoReconnect(true);
		//WiFi.setAutoConnect(true);

		if (st_DHCP == false)
		{
			WiFi.config(st_localIP, st_localGateway, st_localSubnetMask, st_localDNSServer);
		}

		if (!st_preExistingConnection) {
			st::debugPrintln(F(""));
			st::debugPrintln(F("Initializing ESP32 WiFi network.  Please be patient..."));
			//wait for ESP32 to be ready on startup
			delay(1000); //may not be necessary, but seems to help my test board start up cleanly

			// attempt to connect to WiFi network
			WiFi.begin(st_ssid, st_password);
			st::debugPrint(F("Attempting to connect to WPA SSID: "));
			st::debugPrintln(st_ssid);
		}

		int count =0;
		while (WiFi.status() != WL_CONNECTED) {
			count++;
			st::debugPrint(F("."));
			delay(500);	// wait for connection:
			if (count > 10) {
				st::debugPrintln(F("what is taking so long?"));
				count = 0;
			}
		}

		st::debugPrintln(F(""));

		st_server.begin();

		st::debugPrintln(F(""));
		st::debugPrintln(F("Enter the following three lines of data into ST App on your phone!"));
		st::debugPrint(F("localIP = "));
		st::debugPrintln(WiFi.localIP());
		st::debugPrint(F("serverPort = "));
		st::debugPrintln(st_serverPort);
		st::debugPrint(F("MAC Address = "));
		String strMAC(WiFi.macAddress());
		strMAC.replace(":", "");
		st::debugPrintln(strMAC);
		st::debugPrintln(F(""));
		st::debugPrint(F("SSID = "));
		st::debugPrintln(st_ssid);
		st::debugPrint(F("PASSWORD = "));
		st::debugPrintln(st_password);
		st::debugPrint(F("hubIP = "));
		st::debugPrintln(st_hubIP);
		st::debugPrint(F("hubPort = "));
		st::debugPrintln(st_hubPort);
		st::debugPrint(F("RSSI = "));
		st::debugPrintln(String(WiFi.RSSI()));
		st::debugPrintln(F(""));
		st::debugPrintln(F("SmartThingsESP32WiFI: Intialized"));
		st::debugPrintln(F(""));

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
				st::debugPrintln(F("**********************************************************"));
				st::debugPrintln(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				st::debugPrintln(F("**********************************************************"));
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
	void SmartThingsESP32WiFi::send(String message)
	{
		if (WiFi.isConnected() == false)
		{
			if (_isDebugEnabled)
			{
				st::debugPrintln(F("**********************************************************"));
				st::debugPrintln(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				st::debugPrintln(F("**********************************************************"));
			}

			//WiFi.reconnect();
			//init();
		}

		//WiFiClient st_client;

		//Make sure the client is stopped, to free up socket for new connection
		st_client.stop();

		if (st_client.connect(st_hubIP, st_hubPort))
		{
			st_client.println(F("POST / HTTP/1.1"));
			st_client.print(F("HOST: "));
			st_client.print(st_hubIP);
			st_client.print(F(":"));
			st_client.println(st_hubPort);
			st_client.println(F("CONTENT-TYPE: text"));
            st_client.println(F("CONNECTION: CLOSE"));
			st_client.print(F("CONTENT-LENGTH: "));
			st_client.println(message.length());
			st_client.println();
			st_client.print(message);
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
				st::debugPrint(st_hubIP.toString());
				st::debugPrint(F(" "));
				st::debugPrint(F("hubPort = "));
				st::debugPrintln(String(st_hubPort));

				st::debugPrintln(F("***********************************************************"));
				st::debugPrintln(F("**** WiFi Disconnected.  ESP32 should auto-reconnect.  ***"));
				st::debugPrintln(F("***********************************************************"));
			}

			//WiFi.reconnect();
			//init();      //Re-Init connection to get things working again

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

        // Wait for a response
        unsigned long timeout = millis();
        while(!st_client.available())
        {
            if(millis() - timeout > 1000)
            {
                st::debugPrintln(F("Post request timed out\n"));
                st_client.stop();
                return;
            }
        }


		//if (_isDebugEnabled) { st::debugPrintln(F("WiFi.send(): Reading for reply data "));}
		// read any data returned from the POST
        //while (st_client.connected()) {
        while (st_client.available() && st_client.connected()) {
			char c = st_client.read(); //gets byte from ethernet buffer
            /*if((int) c == 255)
            {
                if(_isDebugEnabled)
                    st::debugPrintln(F("Breaking due to invalid value"));
                break;
            }*/
            if (_isDebugEnabled) { st::debugPrint(String(c)); } //prints byte to serial monitor
		}

		delay(1);
		st_client.stop();
	}
}
