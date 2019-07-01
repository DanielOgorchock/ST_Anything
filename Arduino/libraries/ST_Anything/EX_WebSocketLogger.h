//******************************************************************************************
//  File: EX_WebSocketLogger.h
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

#ifndef ST_EX_WebSocketLogger_H
#define ST_EX_WebSocketLogger_H

#include "Executor.h"

#ifdef COMPLIE_WEBSOCKET_LIB
	#include <WebSocketsServer.h>
#endif
#include "ST_Debug.h"

namespace st
{
	
	debugPrint_t wsDebugPrint;
	debugPrintln_t wsDebugPrintLn;

	class EX_WebSocketLogger : public Executor
	{
	private:
		int m_websocketServerPort;
#ifdef COMPLIE_WEBSOCKET_LIB
		static WebSocketsServer *m_webSocket;
#endif
		static String m_printMessage;


	public:
		//constructor
		EX_WebSocketLogger(const __FlashStringHelper *name, int websocketServerPort);

		//destructor
		virtual ~EX_WebSocketLogger();

		//initialization function
		virtual void init();

		//called periodically to ensure state of the Alarm Siren is up to date in the SmartThings Cloud (in case an event is missed)
		virtual void refresh();
		
		virtual void update();

		//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
		virtual void beSmart(const String &str);
		friend 	debugPrint_t wsDebugPrint;
		friend 	debugPrintln_t wsDebugPrintLn;

	};
}



#endif