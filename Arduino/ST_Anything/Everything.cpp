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
			m_Sensors[index]->update();
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
		while(Return_String.length()>=1 && Return_String[0]!='|')
		{
			unsigned int index=Return_String.indexOf("|");
			if(debug)
			{
				Serial.print(F("Sending: "));
				Serial.println(Return_String.substring(0, index));
			}
			#ifndef DISABLE_SMARTTHINGS
				SmartThing.send(Return_String.substring(0, index));
			#endif
			#if defined(ENABLE_SERIAL)
				Serial.println(Return_String.substring(0, index));
			#endif
			
			Return_String=Return_String.substring(index+1);
		}
		Return_String.remove(0);
	}
	
	void Everything::refreshExecutors()
	{
		for(unsigned int i=0; i<m_nExecutorCount; ++i)
		{
			m_Executors[i]->refresh();
		}
	}
	
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
			m_Sensors[index]->init();
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			m_Executors[index]->init();
		}
		
		if(debug)
		{
			Serial.println(F("init devices ended"));
			Serial.print(F("Free RAM = "));
			Serial.println(freeRam());
		}
		
		exLastMillis=millis(); //avoid immediately refreshing after initialization
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
		
		sendStrings();
		
		if(millis()-exLastMillis >= Constants::EX_REFRESH_INTERVAL)
		{
			exLastMillis=millis();
			refreshExecutors();
		}
		
		if(debug && millis()%30000==0 && millis()!=lastmillis)
		{
			lastmillis = millis();
			Serial.print(F("Loop: Free Ram = "));  
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
			Serial.print(F("ERROR: \""));
			Serial.print(str);
			Serial.println(F("\" would overflow the Return_String 'buffer'"));
			return false;
		}
		else
		{
			Return_String+=str+"|";
			return true;
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
			p->beSmart(message);
		}
	}
	
	//initialize static members
	String Everything::Return_String;
	Sensor* Everything::m_Sensors[Constants::MAX_SENSOR_COUNT];
	Executor* Everything::m_Executors[Constants::MAX_EXECUTOR_COUNT];
	unsigned int Everything::m_nSensorCount=0;
	unsigned int Everything::m_nExecutorCount=0;
	unsigned long Everything::lastmillis=0;
	unsigned long Everything::exLastMillis=0;
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
	
	