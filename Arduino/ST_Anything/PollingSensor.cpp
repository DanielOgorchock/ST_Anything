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
			return true;
		}
		else
		{
			return false;
		}
	}

//public
	//constructor
	PollingSensor::PollingSensor(const String &name, unsigned int interval, int offset):
		Sensor(name),
		m_nPreviousTime(millis()),
		m_nDeltaTime(0-offset),
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
			if(DEBUG)
			{
				Serial.println("PollingSensor named \"" + getName() + "\" has triggered (" + m_nInterval + " millisecond interval)");
			}
		}
	}
	
	bool PollingSensor::DEBUG=false;
}