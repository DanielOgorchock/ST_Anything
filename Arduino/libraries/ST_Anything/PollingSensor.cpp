//******************************************************************************************
//  File: PollingSensor.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::PollingSensor is a generic class which inherits from st::Sensor.  This is the
//			  parent class for the st::PS_Illuminace, st::PS_Water, and PS_TemperatureHumidity classes.
//
//			  In general, this file should not need to be modified.   
//
//			  st::PollingSensor() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
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
			if (debug)
			{
				Serial.println(F("PollingSensor: millis() Overflow handled"));
			}
			
			m_nPreviousTime = 0;
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
	PollingSensor::PollingSensor(const __FlashStringHelper *name, long interval, long offset):
		Sensor(name),
		m_nPreviousTime(0),
		m_nDeltaTime(0),
		m_nInterval(interval*1000),
		m_nOffset(offset*1000)
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

	void PollingSensor::refresh()
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