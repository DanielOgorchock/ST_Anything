#include "PollingSensor.h"

#include "Constants.h"

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

	String PollingSensor::init()
	{
		return Constants::IGNORE_STRING;
	}
	
	String PollingSensor::update()
	{
		if(checkInterval())
		{
			return getData();
		}
		
		return Constants::IGNORE_STRING;
	}
	
	String PollingSensor::getData()
	{
		if(debug)
		{
			return "DEBUG: \""+getName() + "\" has triggered (" + m_nInterval + " millisecond interval)";
		}
		else
		{
			return Constants::IGNORE_STRING;
		}
	}
	
	bool PollingSensor::debug=false;
}