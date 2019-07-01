//******************************************************************************************
//  File: EX_WebSocketLogger.cpp
//  Authors: Daniel Terryn
//
//  Summary:  EX_WebSocketLogger is a class which implements a websocket to provide logging via ethernet.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_WebSocketLogger executor1(F("EX_WebSocketLogger"), 81);
//
//			  st::EX_WebSocketLogger() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- int port - REQUIRED - Port the websocket listens on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-07-01  Dan.t   		 Original Creation
//
//
//******************************************************************************************

#include "Constants.h"
#include "EX_WebSocketLogger.h"
#include "Everything.h"

namespace st
{

	//public
	//constructor
	EX_WebSocketLogger::EX_WebSocketLogger(const __FlashStringHelper *name, int websocketServerPort) :
		Executor(name)
	{
		m_printMessage = "";
#ifdef COMPLIE_WEBSOCKET_LIB
		m_webSocket = new WebSocketsServer(websocketServerPort);
#endif
	}

	//destructor
	EX_WebSocketLogger::~EX_WebSocketLogger()
	{
#ifdef COMPLIE_WEBSOCKET_LIB
		if (m_webSocket)
			delete m_webSocket;
		m_webSocket = NULL;
#endif
	}

	void EX_WebSocketLogger::update()
	{
		refresh();
	}

	//initialization routine
	void EX_WebSocketLogger::init()
	{
#ifdef COMPLIE_WEBSOCKET_LIB
		m_webSocket->begin();
#endif
		st::Everything::m_pnPrintFn=wsDebugPrint; 
		st::Everything::m_pnPrintLnFn=wsDebugPrintLn; 
	}

	//called periodically to ensure state of the Alarm Siren is up to date in the SmartThings Cloud (in case an event is missed)
	void EX_WebSocketLogger::refresh()
	{
#ifdef COMPLIE_WEBSOCKET_LIB
		m_webSocket->loop();
#endif
	}

	//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
	void EX_WebSocketLogger::beSmart(const String &str)
	{
		refresh();
	}

	void wsDebugPrint(String message)
	{
#ifdef COMPLIE_WEBSOCKET_LIB
  		EX_WebSocketLogger::m_printMessage += message;
#endif
  		Serial.print(message);
	}

	void wsDebugPrintLn(String message) 
	{
  		Serial.println(message);
#ifdef COMPLIE_WEBSOCKET_LIB
  		EX_WebSocketLogger::m_printMessage += message;
  		EX_WebSocketLogger::m_webSocket->broadcastTXT(EX_WebSocketLogger::m_printMessage);
  		EX_WebSocketLogger::m_printMessage="";
#endif
	}

#ifdef COMPLIE_WEBSOCKET_LIB
	WebSocketsServer *EX_WebSocketLogger::m_webSocket = NULL;
#endif
	String EX_WebSocketLogger::m_printMessage = "";
}