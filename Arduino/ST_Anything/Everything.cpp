#include "Everything.h"


namespace st
{
//private


	void Everything::updateSensors()
	{
		for(unsigned int index=0; index<m_nSensorCount; ++index)
		{
			sendSmartString(m_Sensors[index]->update());
		}
	}
	
	void Everything::receiveSmartString()
	{
		//More SmartThings code required
	}
	
//public
	//constructor
	Everything::Everything():
		m_nSensorCount(0),
		m_nExecutorCount(0)
	{
		
	}
	
	//destructor
	Everything::~Everything()
	{
	
	}
	
	void Everything::init()
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
		receiveSmartString();
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
		
		//SmartThings code will go here.
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
	
	bool Everything::debug=false;
	
}