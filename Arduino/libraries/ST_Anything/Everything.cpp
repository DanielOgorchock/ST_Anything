//******************************************************************************************
//  File: Everything.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::Everything is a generic class which essentially acts as the main() routine.
//			  All st::Device type objects are managed by st::Everything.  It is responsible for
//			  for calling the correct functions within each object it is responsible for at the
//			  proper time.  It handles all initialization of and use of the SmarThings Shield library.
//			  
//			  THere are user-definable settings which will impact the st::Everything class stored in 
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
//    2017-02-19  Dan Ogorchock  Fixed bug in throttling capability
//    2017-04-26  Dan Ogorchock  Allow each communication method to specify unique ST transmission throttling delay
//
//******************************************************************************************

//#include <Arduino.h>
//#include <avr/pgmspace.h>
#include "Everything.h"

long freeRam();	//freeRam() function prototype - useful in determining how much SRAM is available on Arduino
#if defined(ARDUINO_ARCH_SAMD)
extern "C" char* sbrk(int incr);
#endif
namespace st
{
	
//private
	void Everything::updateSensors()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			m_Sensors[index]->update();
			sendStrings();
		}
	}
	
#if defined(ENABLE_SERIAL)
	void Everything::readSerial()
	{
		String message;
		while(Serial.available()>0)
		{
			char c=Serial.read();
			message+=c;
			delay(10);
		}
		if(message.length()>0)
		{
			receiveSmartString(message);
		}
	}
#endif
	
	void Everything::sendStrings()
	{
		unsigned int index;
		//Loop through the Return_String buffer and send each "|" delimited string to ST Shield
		while(Return_String.length()>=1 && Return_String[0]!='|')
		{
			index=Return_String.indexOf("|");
			if(debug)
			{
				Serial.print(F("Everything: Sending: "));
				Serial.println(Return_String.substring(0, index));
				//Serial.print(F("Everything: getTransmitInterval() = "));
				//Serial.println(SmartThing->getTransmitInterval());
			}
			#ifndef DISABLE_SMARTTHINGS
//			if (millis() - sendstringsLastMillis < Constants::SENDSTRINGS_INTERVAL)
			if (millis() - sendstringsLastMillis < SmartThing->getTransmitInterval())
				{
//					delay(Constants::SENDSTRINGS_INTERVAL - (millis() - sendstringsLastMillis)); //Added due to slow ST Hub/Cloud Processing.  Events were being missed.  DGO 2015-03-28
					delay(SmartThing->getTransmitInterval() - (millis() - sendstringsLastMillis)); //modified to allow different values for each method of communicating to ST cloud.  DGO 2017-04-26
			}
				SmartThing->send(Return_String.substring(0, index));
				sendstringsLastMillis = millis();
			#endif
			#if defined(ENABLE_SERIAL) && defined(DISABLE_SMARTTHINGS)
				Serial.println(Return_String.substring(0, index));
			#endif
			
			if(callOnMsgSend!=0)
			{
				callOnMsgSend(Return_String.substring(0, index));
			}

			Return_String=Return_String.substring(index+1);
		}
		Return_String.remove(0);	//clear the Return_String buffer
	}
	
	void Everything::refreshDevices()
	{
		for(unsigned int i=0; i<m_nExecutorCount; ++i)
		{
			m_Executors[i]->refresh();
			sendStrings();
		}

		for (unsigned int i = 0; i<m_nSensorCount; ++i)
		{
			m_Sensors[i]->refresh();
			sendStrings();
		}
	}
	
//public
	void Everything::init()
	{
		Serial.begin(Constants::SERIAL_BAUDRATE);
		Return_String.reserve(st::Constants::RETURN_STRING_RESERVE);	//allocate Return_String buffer one time to prevent Heap Fragmentation.  RETURN_STRING_RESERVE is set in Constants.h
		
		if(debug)
		{
			Serial.println(F("Everything: init started"));
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
		
		#ifndef DISABLE_SMARTTHINGS
			SmartThing->init();
		#endif
		
		
		if(debug)
		{
			Serial.println(F("Everything: init ended"));
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
	}
	
	void Everything::initDevices()
	{
		if(debug)
		{
			Serial.println(F("Everything: initDevices started"));
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
		
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			m_Sensors[index]->init();
			sendStrings();
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			m_Executors[index]->init();
			sendStrings();
		}
		
		if(debug)
		{
			Serial.println(F("Everything: initDevices ended"));
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
		
		refLastMillis = millis(); //avoid immediately refreshing after initialization
	}
	
	void Everything::run()
	{
		updateSensors();			//call each st::Sensor object to refresh data

		#ifndef DISABLE_SMARTTHINGS
			SmartThing->run();		//call the ST Shield Library to receive any data from the ST Hub
		#endif
		
		#if defined(ENABLE_SERIAL)
			readSerial();			//read data from the Arduino IDE Serial Monitor window (useful for debugging sometimes)
		#endif
		
		sendStrings();				//send any pending updates to ST Cloud
		
		#ifndef DISABLE_REFRESH		//Added new check to allow user to disable REFRESH feature - setting is in Constants.h)
		if ((bTimersPending == 0) && ((millis() - refLastMillis) >= long(Constants::DEV_REFRESH_INTERVAL) * 1000))  //DEV_REFRESH_INTERVAL is set in Constants.h
		{
			refLastMillis = millis();
			refreshDevices();	//call each st::Device object to refresh data (this is just a safeguard to ensure the state of the Arduino and the ST Cloud stay in synch should an event be missed)
		}
		#endif
		
		if((debug) && (millis()%60000==0) && (millis()!=lastmillis))
		{
			lastmillis = millis();
			Serial.print(F("Everything: Free Ram = "));  
			Serial.println(freeRam());
		}
	}
	
	bool Everything::sendSmartString(String &str)
	{
		while(str.length()>1 && str[0]=='|') //get rid of leading pipes (messes up sendStrings()'s parsing technique)
		{
			str=str.substring(1);
		}
		if((str.length()==1 && str[0]=='|') || str.length()==0)
		{
			return false;
		}
		
		if(Return_String.length()+str.length()>=Constants::RETURN_STRING_RESERVE)
		{
			if (debug)
			{
				Serial.print(F("Everything: ERROR: \""));
				Serial.print(str);
				Serial.println(F("\" would overflow the Return_String 'buffer'"));
			}
			return false;
		}
		else
		{
			Return_String+=str+"|";		//add the new message to the queue to be sent to ST Shield with a "|" delimiter
			return true;
		}
	}

	bool Everything::sendSmartStringNow(String &str)
	{
		if (sendSmartString(str)) sendStrings(); //send any pending updates to ST Cloud immediately
	}

	Device* Everything::getDeviceByName(const String &str)
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			if(m_Sensors[index]->getName()==str)
				return (Device*)m_Sensors[index];
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			if(m_Executors[index]->getName()==str)
				return (Device*)m_Executors[index];
		}
		
		return 0; //null if no such device present
	}
	
	bool Everything::addSensor(Sensor *sensor)
	{
		if(m_nSensorCount>=Constants::MAX_SENSOR_COUNT)
		{
			if(debug)
			{
				Serial.print(F("Did not add sensor named "));
				Serial.print(sensor->getName());
				Serial.println(F("(You've exceeded maximum number of sensors; edit Constants.h)"));
			}
			return false;
		}
		else
		{
			m_Sensors[m_nSensorCount]=sensor;
			++m_nSensorCount;
		}
		
		if(debug)
		{
			Serial.print(F("Everything: adding sensor named "));
			Serial.println(sensor->getName());
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
		return true;
	}
	
	bool Everything::addExecutor(Executor *executor)
	{
		if(m_nExecutorCount>=Constants::MAX_EXECUTOR_COUNT)
		{
			if(debug)
			{
				Serial.print(F("Did not add executor named "));
				Serial.print(executor->getName());
				Serial.println(F("(You've exceeded maximum number of executors; edit Constants.h)"));
			}
			return false;
		}
		else
		{
			m_Executors[m_nExecutorCount]=executor;
			++m_nExecutorCount;
		}
		
		if(debug)
		{
			Serial.print(F("Everything: adding executor named "));
			Serial.println(executor->getName());
			Serial.print(F("Everything: Free RAM = "));
			Serial.println(freeRam());
		}
		return true;
	}
	
	//friends!
	void receiveSmartString(String message)
	{
		message.trim();
		if(Everything::debug && message.length()>1)
		{
			Serial.print(F("Everything: Received: "));
			Serial.println(message);
		}
		
		if (message == "refresh")
		{
			Everything::refreshDevices();
		}
		else if (message.length() > 1)		//ignore empty string messages from the ST Hub
		{
			Device *p = Everything::getDeviceByName(message.substring(0, message.indexOf(' ')));
			if (p != 0)
			{
				p->beSmart(message);	//pass the incoming SmartThings Shield message to the correct Device's beSmart() routine
			}
		}
		
		if(Everything::callOnMsgRcvd!=0)
		{
			Everything::callOnMsgRcvd(message);
		}

	}
	
	//initialize static members
	st::SmartThings* Everything::SmartThing=0; //initialize pointer to null
	String Everything::Return_String;
	Sensor* Everything::m_Sensors[Constants::MAX_SENSOR_COUNT];
	Executor* Everything::m_Executors[Constants::MAX_EXECUTOR_COUNT];
	byte Everything::m_nSensorCount=0;
	byte Everything::m_nExecutorCount=0;
	unsigned long Everything::lastmillis=0;
	unsigned long Everything::refLastMillis=0;
	unsigned long Everything::sendstringsLastMillis=0;
	bool Everything::debug=false;
	byte Everything::bTimersPending=0;	//initialize variable
	void (*Everything::callOnMsgSend)(const String &msg)=0; //initialize this callback function to null
	void (*Everything::callOnMsgRcvd)(const String &msg)=0; //initialize this callback function to null
	
	//SmartThings static members
	//#ifndef DISABLE_SMARTTHINGS
	//	// Please refer to Constants.h for settings that affect whether a board uses SoftwareSerial or Hardware Serial calls
	//	#if defined(ST_SOFTWARE_SERIAL)  //use Software Serial
	//		SmartThingsThingShield Everything::SmartThing(Constants::pinRX, Constants::pinTX, receiveSmartString);
	//	#elif defined(ST_HARDWARE_SERIAL) //use Hardware Serial
	//		SmartThingsThingShield Everything::SmartThing(Constants::SERIAL_TYPE, receiveSmartString);
	//	#endif

	//	SmartThingsNetworkState_t Everything::stNetworkState=(SmartThingsNetworkState_t)99; //bogus value for first pass through Everything::updateNetworkState()
	//#endif
}

//freeRam() function - useful in determining how much SRAM is available on Arduino
long freeRam()
{
#if defined(ARDUINO_ARCH_AVR) 
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
#elif defined(ARDUINO_ARCH_ESP8266)
	return ESP.getFreeHeap();
#elif defined(ARDUINO_ARCH_SAMD)
	char top;
	return &top - reinterpret_cast<char*>(sbrk(0));
#else
	return -1;
#endif // !


}
	
