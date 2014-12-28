#include "Everything.h"


namespace st
{
	SmartThingsCallout_t receiveSmartString; //function prototype
	
//private
	void Everything::updateNetworkState()
	{
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
	}

	void Everything::updateSensors()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->update());
		}
	}
	
	
	
//public
	void Everything::initSmartThings()
	{
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
	}
	
	void Everything::initDevices()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->init());
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			sendSmartString(m_Executors[index]->init());
		}
	}
	
	void Everything::run()
	{
		updateSensors();
		SmartThing.run();
		updateNetworkState();
	}
	
	void Everything::sendSmartString(const String &str)
	{
		if(&str==&Constants::IGNORE_STRING)
		{
			return;
		}
		
		if (str.indexOf('|') == -1) 
		{
			if (debug)
			{
				Serial.print(F("Sending: "));
				Serial.println(str);
			}
			SmartThing.send(str);
		}
		else
		{
			if (debug)
			{
				Serial.print(F("Sending: "));
				Serial.println(str.substring(0, str.indexOf('|')));
			}
			SmartThing.send(str.substring(0, str.indexOf('|')));	//Send first half to ST Cloud

			if (debug)
			{
				Serial.print(F("Sending: "));
				Serial.println(str.substring(str.indexOf('|') + 1));
			}
			SmartThing.send(str.substring(str.indexOf('|') + 1));	//Send second half to ST Cloud

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
	}
	
	//friends!
	void receiveSmartString(String message)
	{
		if(Everything::debug)
		{
			Serial.print(F("Received: "));
			Serial.println(message);
		}
	}
	
	
	//initialize static members
	String Everything::Return_String;
	SmartThings Everything::SmartThing((Constants::THING_SHIELD_PINS==Constants::PINS_0_1?1:3), (Constants::THING_SHIELD_PINS==Constants::PINS_0_1?0:2), receiveSmartString);
	Sensor* Everything::m_Sensors[Constants::MAX_SENSOR_COUNT];
	Executor* Everything::m_Executors[Constants::MAX_EXECUTOR_COUNT];
	unsigned int Everything::m_nSensorCount=0;
	unsigned int Everything::m_nExecutorCount=0;
	SmartThingsNetworkState_t Everything::stNetworkState=(SmartThingsNetworkState_t)99; //bogus value
	bool Everything::debug=false;
	
}