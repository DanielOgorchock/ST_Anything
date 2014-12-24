#include "Everything.h"


namespace st
{
	SmartThingsCallout_t receiveSmartString; //function prototype
	
//private


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
	}
	
	void Everything::sendSmartString(const String &str)
	{
		if(str==Constants::IGNORE_STRING)
		{
			return;
		}
		
		if(debug)
		{
			Serial.println("Sending: "+str);
		}
		
		SmartThing.send(str);
	}
	
	Device* Everything::getDeviceByName(const String &str)
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			if(String(m_Sensors[index]->getId())==str)
				return (Device*)m_Sensors[index];
		}
		
		for(unsigned int index=0; index<m_nExecutorCount; ++index)
		{
			if(String(m_Executors[index]->getId())==str)
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
			Serial.println("Received: "+message);
		}
	}
	
	
	//initialize static members
	String Everything::Return_String;
	SmartThings Everything::SmartThing((Constants::THING_SHIELD_PINS==Constants::PINS_0_1?0:2), (Constants::THING_SHIELD_PINS==Constants::PINS_0_1?1:3), receiveSmartString);
	Sensor* Everything::m_Sensors[Constants::MAX_SENSOR_COUNT];
	Executor* Everything::m_Executors[Constants::MAX_EXECUTOR_COUNT];
	unsigned int Everything::m_nSensorCount=0;
	unsigned int Everything::m_nExecutorCount=0;
	bool Everything::debug=false;
	
}