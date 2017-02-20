//*******************************************************************************
//	SmartThings Arduino WifiEsp Library - Use an Arduino with an ESP-01 for WiFi
//
//	License
//	(C) Copyright 2017 Dan Ogorchock
//
//	History
//	2017-02-20  Dan Ogorchock  Created
//*******************************************************************************


#include "SmartThingsWiFiEsp.h"

namespace st
{
	//*******************************************************************************
	// SmartThingsWiFiEsp Constructor - Arduino + ESP-01 board  
	//*******************************************************************************
	SmartThingsWiFiEsp::SmartThingsWiFiEsp(Stream *espSerial, String ssid, String password, IPAddress localIP, uint16_t serverPort, IPAddress hubIP, uint16_t hubPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug) :
		SmartThingsEthernet(localIP, serverPort, hubIP, hubPort, callout, shieldType, enableDebug),
		st_server(serverPort),
		st_espSerial(espSerial)
	{
		ssid.toCharArray(st_ssid, sizeof(st_ssid));
		password.toCharArray(st_password, sizeof(st_password));
	}



	//*****************************************************************************
	//SmartThingsWiFiEsp::~SmartThingsESP8266WiFI()
	//*****************************************************************************
	SmartThingsWiFiEsp::~SmartThingsWiFiEsp()
	{

	}

	//*******************************************************************************
	/// Initialize SmartThingsWiFiEsp Library 
	//*******************************************************************************
	void SmartThingsWiFiEsp::init(void)
	{

		Serial.println(F(""));
		Serial.println(F("Initializing WiFiEsp network.  Please be patient..."));

		// Initialize the WiFiEsp library
		WiFi.init(st_espSerial);

		// Connect to WiFi network
		WiFi.begin(st_ssid, st_password);

		// Set the local IP address
		WiFi.config(st_localIP);

		while (WiFi.status() != WL_CONNECTED) 
		{
			delay(2000);
			if (_isDebugEnabled)
			{
				Serial.println(F(""));
				Serial.println(F("Waiting for WiFi to connect"));
			}
		}

		st_server.begin();

		Serial.print(F("MAC Address = "));
		byte mac[6];
		WiFi.macAddress(mac);
		char buf[20];
		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
		Serial.println(buf);


		if (_isDebugEnabled)
		{
			Serial.println(F(""));
			Serial.print(F("SSID = "));
			Serial.println(st_ssid);
			Serial.print(F("PASSWORD = "));
			Serial.println(st_password);
			Serial.print(F("hubIP = "));
			Serial.println(st_hubIP);
			Serial.print(F("hubPort = "));
			Serial.println(st_hubPort);
		}
		Serial.println(F("SmartThingsWiFiEsp: Intialized"));
	}

	//*****************************************************************************
	// Run SmartThingsWiFiEsp Library 
	//*****************************************************************************
	void SmartThingsWiFiEsp::run(void)
	{
		String readString;
		String tempString;

		WiFiEspClient client = st_server.available();
		if (client) {
			boolean currentLineIsBlank = true;
			while (client.connected()) {
				if (client.available()) {
					char c = client.read();
					//read char by char HTTP request
					if (readString.length() < 100) {
						//store characters to string
						readString += c;
					}
					if (c == '\n' && currentLineIsBlank) {
						//now output HTML data header
						tempString = readString.substring(readString.indexOf('/') + 1, readString.indexOf('?'));

						if (tempString.length() > 0) {
							client.println(F("HTTP/1.1 200 OK")); //send new page
							if (_isDebugEnabled)
							{
								Serial.print(F("Handling request from ST. tempString = "));
								Serial.println(tempString);
							}
							_calloutFunction(tempString);
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
			readString = "";
			tempString = "";

			delay(1);
			//stopping client
			client.stop();
		}
	}

	//*******************************************************************************
	/// Send Message out over Ethernet to the Hub 
	//*******************************************************************************
	void SmartThingsWiFiEsp::send(String message)
	{
		if (st_client.connect(st_hubIP, st_hubPort))
		{
			st_client.println(F("POST / HTTP/1.1"));
			st_client.print(F("HOST: "));
			st_client.print(st_hubIP);
			st_client.print(F(":"));
			st_client.println(st_hubPort);
			//st_client.println(message);

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
				Serial.print(F(""));
				Serial.println(F("SmartThingsESP8266WiFi::send() - Ethernet Connection Failed"));
				Serial.print(F("hubIP = "));
				Serial.print(st_hubIP);
				Serial.print(F(" "));
				Serial.print(F("hubPort = "));
				Serial.println(st_hubPort);
			}
		}

		// read any data returned from the POST
		while (st_client.connected() && !st_client.available()) delay(1);  //waits for data
		while (st_client.connected() || st_client.available())             //connected or data available
		{
			char c = st_client.read();
		}

		delay(1);
		st_client.stop();
	}

}
