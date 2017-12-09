//******************************************************************************************
//  File: Everything.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::Everything is a static class which essentially acts as the main() routine for 
//			  a ST_Anything application.
//			  -All st::Device type objects (Sensors and Executors) are managed by st::Everything.  
//			  -It calls the correct functions within each object it 
//			   is responsible for at the proper time.  
//			  -It handles all initialization of and use of the SmarThings Shield library.
//			  
//			  User-definable settings which will impact the st::Everything class are stored in 
//			  Constants.h.  Please edit Constants.h to adjust these settings.
//
//			  In general, this file should not need to be modified.   
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//	  2015-01-10  Dan Ogorchock	 Minor improvements to support Door Control Capability
//	  2015-03-14  Dan Ogorchock	 Added public setLED() function to control ThingShield LED
//    2015-03-28  Dan Ogorchock  Added throttling capability to sendStrings to improve success rate of ST Cloud getting the data ("SENDSTRINGS_INTERVAL" is in CONSTANTS.H)
//    2017-02-07  Dan Ogorchock  Added support for new SmartThings v2.0 library (ThingShield, W5100, ESP8266)
//
//******************************************************************************************

#ifndef ST_EVERYTHING_H
#define ST_EVERYTHING_H

//#include "Arduino.h"
#include "Constants.h"
#include "Sensor.h"
#include "Executor.h"

#include "SmartThings.h"

namespace st
{
	SmartThingsCallout_t receiveSmartString;	//function prototype for ST Library callback function

	class Everything
	{
		private:
			static Sensor* m_Sensors[Constants::MAX_SENSOR_COUNT];		//array of Sensor objects that st::Everything will keep track of
			static byte m_nSensorCount;	//number of st::Sensor objects added to st::Everything in your sketch Setup() routine
			
			static Executor* m_Executors[Constants::MAX_EXECUTOR_COUNT]; //array of Executor objects that st::Everything will keep track of
			static byte m_nExecutorCount;//number of st::Executor objects added to st::Everything in your sketch Setup() routine
			
			
			//static SmartThingsNetworkState_t stNetworkState;
		
			//static void updateNetworkState();	//keeps track of the current ST Shield to Hub network status
			static void updateSensors();		//simply calls update on all the sensors
			static void sendStrings();			//sends all updates from the devices in Return_String
			static unsigned long sendstringsLastMillis;	//keep track of how long since last time we sent data to ST Cloud, to enable throttling

			static unsigned long lastmillis;	//used to keep track of last time run() has output freeRam() info
			
			//stuff for refreshing Devices
			static unsigned long refLastMillis;	//used to keep track of last time run() has called refreshDevices()
			static void refreshDevices();		//simply calls refresh on all the Devices

			#ifdef ENABLE_SERIAL
				static void readSerial();		//reads data from Arduino IDE Serial Monitor, if enabled in Constants.h
			#endif
		
			static String Return_String;		//static buffer for string data queued for transfer to SmartThings Shield - prevents dynamic memory allocation heap fragmentation
		
		public:
			static void init();					//st::Everything initialization routine called in your sketch setup() routine 
			static void initDevices();			//calls the init() routine of every object added to st::Everything in your sketch setup() routine 
			static void run();					//st::Everything initialization routine called in your sketch loop() routine 
			
			static bool sendSmartString(String &str); //sendSmartString() may edit the string reference passed to it - queues messages - preferable
			static bool sendSmartStringNow(String &str); //sendSmartStringNow() may edit the string reference passed to it - sends messages immediate - only for special circumstances

			static Device* getDeviceByName(const String &str);	//returns pointer to Device object by name
			
			static bool addSensor(Sensor *sensor);		//adds a Sensor object to st::Everything's m_Sensors[] array - called in your sketch setup() routine
			static bool addExecutor(Executor *executor);//adds a Executor object to st::Everything's m_Executors[] array - called in your sketch setup() routine
		
			static byte bTimersPending;	//number of time critical events in progress - if > 0, do NOT perform refreshDevices() routine 

			static bool debug;	//debug flag to determine if debug print statements are executed - set value in your sketch's setup() routine
			
			static void (*callOnMsgSend)(const String &msg); //If this function pointer is assigned, the function it points to will be called upon every time a string is sent to the cloud.		
			static void (*callOnMsgRcvd)(const String &msg); //If this function pointer is assigned, the function it points to will be called upon every time a string is received from the cloud.

			//SmartThings Object
			#ifndef DISABLE_SMARTTHINGS
				static st::SmartThings* SmartThing;	//SmartThings Shield Library object
			#endif

			friend SmartThingsCallout_t receiveSmartString; //callback function to act on data received from SmartThings Shield - called from SmartThings Shield Library
			
			//SmartThings Object
			//#ifndef DISABLE_SMARTTHINGS
			//  static void setLED(uint8_t red, uint8_t green, uint8_t blue) {SmartThing.shieldSetLED(red, green, blue);}
			//#endif
	};
}
#endif
