#include <arduino.h>
#include "Everything.h"

int freeRam(); //function prototype

namespace st
{
	SmartThingsCallout_t receiveSmartString; //function prototype
	
//private
	void Everything::updateNetworkState()
	{
		#ifndef DISABLE_SMARTTHINGS
			SmartThingsNetworkState_t tempState = SmartThing.shieldGetLastNetworkState();
			if (tempState != stNetworkState)
			{
				switch (tempState)
				{
				  case STATE_NO_NETWORK:
					if (debug) Serial.println(F("NO_NETWORK"));
					SmartThing.shieldSetLED(2, 0, 0); // red
					break;
				  case STATE_JOINING:
					if (debug) Serial.println(F("JOINING"));
					SmartThing.shieldSetLED(2, 0, 0); // red
					break;
				  case STATE_JOINED:
					if (debug) Serial.println(F("JOINED"));
					SmartThing.shieldSetLED(0, 0, 0); // off
					break;
				  case STATE_JOINED_NOPARENT:
					if (debug) Serial.println(F("JOINED_NOPARENT"));
					SmartThing.shieldSetLED(2, 0, 2); // purple
					break;
				  case STATE_LEAVING:
					if (debug) Serial.println(F("LEAVING"));
					SmartThing.shieldSetLED(2, 0, 0); // red
					break;
				  default:
				  case STATE_UNKNOWN:
					if (debug) Serial.println(F("UNKNOWN"));
					SmartThing.shieldSetLED(0, 2, 0); // green
					break;
				}
				stNetworkState = tempState; 
			}
		#endif
	}

	void Everything::updateSensors()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->update());
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
	
	
	
//public
	void Everything::init()
	{
		Serial.begin(Constants::SERIAL_BAUDRATE);
		Return_String.reserve(st::Constants::RETURN_STRING_RESERVE);
		
		if(debug)
		{
			Serial.println(F("init started"));
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
		
		#ifndef DISABLE_SMARTTHINGS
			if(Constants::WAIT_FOR_JOIN_AT_START)
			{
				while(stNetworkState!=STATE_JOINED)
				{
					updateNetworkState();
					SmartThing.run();
				}
			}
			else
			{
				updateNetworkState();
				SmartThing.run();
			}
		#endif
		
		
		if(debug)
		{
			Serial.println(F("init ended"));
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
	}
	
	void Everything::initDevices()
	{
		if(debug)
		{
			Serial.println(F("init devices started"));
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
		
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->init());
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			sendSmartString(m_Executors[index]->init());
		}
		
		if(debug)
		{
			Serial.println(F("init devices ended"));
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
	}
	
	void Everything::run()
	{
		updateSensors();
		
		#ifndef DISABLE_SMARTTHINGS
			SmartThing.run();
			updateNetworkState();
		#endif
		
		#if defined(ENABLE_SERIAL)
			readSerial();
		#endif
		
		if(debug && millis()%30000==0 && millis()!=lastmillis)
		{
			lastmillis = millis();
			Serial.print(F("Loop: Free Ram = "));  
			Serial.println(freeRam());
		}
	}
	
	void Everything::sendSmartString(const String &str)
	{
		if(&str==&Constants::IGNORE_STRING)
		{
			return;
		}
		
		if(debug)
		{
			Serial.print(F("Sending: "));
			Serial.println(str);
		}
		
		if(str.indexOf('|') == -1)
		{
			#ifndef DISABLE_SMARTTHINGS
				SmartThing.send(str);
			#endif
			
			#if defined(ENABLE_SERIAL)
				Serial.println(str);
			#endif
		}
		
		else
		{
			byte index=str.indexOf('|');
			#ifndef DISABLE_SMARTTHINGS
				SmartThing.send(str.substring(0, index));
			#endif
			
			#if defined(ENABLE_SERIAL)
				Serial.println(str.substring(0, index));
			#endif
			
			sendSmartString(str.substring(index+1));
		}
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
			return false;
		}
		else
		{
			m_Sensors[m_nSensorCount]=sensor;
			++m_nSensorCount;
		}
		
		if(debug)
		{
			Serial.print(F("adding sensor named "));
			Serial.println(sensor->getName());
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
		return true;
	}
	
	bool Everything::addExecutor(Executor *executor)
	{
		if(m_nExecutorCount>=Constants::MAX_EXECUTOR_COUNT)
		{
			return false;
		}
		else
		{
			m_Executors[m_nExecutorCount]=executor;
			++m_nExecutorCount;
		}
		
		if(debug)
		{
			Serial.print(F("adding executor named "));
			Serial.println(executor->getName());
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
		return true;
	}
	
	//friends!
	void receiveSmartString(String message)
	{
		message.trim();
		if(Everything::debug)
		{
			Serial.print(F("Received: "));
			Serial.println(message);
		}
		
		Device *p=Everything::getDeviceByName(message.substring(0, message.indexOf(' ')));
		if(p!=0)
		{
			Everything::sendSmartString(p->beSmart(message));
		}
	}
	
	//initialize static members
	String Everything::Return_String;
	Sensor* Everything::m_Sensors[Constants::MAX_SENSOR_COUNT];
	Executor* Everything::m_Executors[Constants::MAX_EXECUTOR_COUNT];
	unsigned int Everything::m_nSensorCount=0;
	unsigned int Everything::m_nExecutorCount=0;
	unsigned long Everything::lastmillis=0;
	bool Everything::debug=false;

	//SmartThings static members
	#ifndef DISABLE_SMARTTHINGS
		// Please refer to Constants.h for settings that affect whether a board uses Software or Hardware Serial calls
		#if defined(ST_SOFTWARE_SERIAL)  //use Software Serial
			SmartThings Everything::SmartThing(Constants::pinRX, Constants::pinTX, receiveSmartString);
		#elif defined(ST_HARDWARE_SERIAL) //use Hardware Serial
			SmartThings Everything::SmartThing(Constants::SERIAL_TYPE, receiveSmartString);
		#endif

		SmartThingsNetworkState_t Everything::stNetworkState=(SmartThingsNetworkState_t)99; //bogus value

	#endif
}

int freeRam()
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
	
	