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
		
		if(m_nPreviousTime==0) //eliminates problem of there being a delay before first update() call
		{
			m_nPreviousTime=millis();
		}
		
		//calculate new delta time
		m_nDeltaTime+=(millis()-m_nPreviousTime)-m_nOffset;
		m_nOffset=0;
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
	PollingSensor::PollingSensor(const String &name, long interval, long offset):
		Sensor(name),
		m_nPreviousTime(0),
		m_nDeltaTime(0),
		m_nInterval(interval),
		m_nOffset(offset)
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