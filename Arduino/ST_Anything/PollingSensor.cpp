//******************************************************************************************
//  File: PollingSensor.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::PollingSensor is a generic class which inherits from st::Sensor.  This is the
//			  parent class for the st::PS_Illuminace, st::PS_Water, and PS_TemperatureHumidity classes.
//			  In general, this file should not need to be modified.   
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#include "PollingSensor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	bool PollingSensor::checkInterval()
	{
		//check for time overflow
		if(millis()<m_nPreviousTime)
		{
			Serial.println(F("Overflow"));
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

	void PollingSensor::init()
	{
		getData();
	}
	
	void PollingSensor::update()
	{
		if(checkInterval())
		{
			getData();
		}
	}
	
	void PollingSensor::getData()
	{
		if(debug)
		{
			Everything::sendSmartString(getName() + F(" triggered"));
		}
	}
	
	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool PollingSensor::debug=false;
}