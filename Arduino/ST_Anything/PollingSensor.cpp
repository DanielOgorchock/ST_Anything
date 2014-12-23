#include "PollingSensor.h"


namespace st
{
//private
	bool PollingSensor::checkInterval()
	{
		//check for time overflow
		if(millis()<m_nPreviousTime)
		{
			Serial.println("Overflow");
			m_nPreviousTime=0;
		}
	
		//calculate new delta time
		m_nDeltaTime+=(millis()-m_nPreviousTime);
		m_nPreviousTime=millis();
		
		//determine interval has passed
		if(m_nDeltaTime>=m_nInterval)
		{
			m_nDeltaTime=0;
			if(DEBUG)
			{
				Serial.println("PollingSensor named \"" + getName() + "\" has triggered (" + m_nInterval + " millisecond interval)");
			}
			return true;
		}
		else
		{
			return false;
		}
	}

//public
	//constructor
	PollingSensor::PollingSensor(const String &name, unsigned long interval, long offset):
		Sensor(name),
		m_nPreviousTime(millis()),
		m_nDeltaTime(-offset),
		m_nInterval(interval)
	{
	
	}
	
	//destructor
	PollingSensor::~PollingSensor()
	{
		
	}

	void PollingSensor::init()
	{
	
	}
	
	void PollingSensor::update()
	{
		if(checkInterval())
		{
			if(!getData())
			{
				Serial.println("Error getting data for \""+getName()+"\"");
			}
		}
	}
	
	bool PollingSensor::getData()
	{
		return true;
	}
	
	bool PollingSensor::DEBUG=false;
}