#include "PS_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor
	PS_Illuminance::PS_Illuminance(byte id, unsigned int interval, int offset, byte analogInputPin, int s_l, int s_h, int m_l, int m_h):
		PollingSensor(id, interval, offset),
		m_nSensorValue(0),
		SENSOR_LOW(s_l),
		SENSOR_HIGH(s_h),
		MAPPED_LOW(m_l),
		MAPPED_HIGH(m_h)
	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_Illuminance::~PS_Illuminance()
	{
		
	}

	const String& PS_Illuminance::init()
	{
		return Constants::IGNORE_STRING;
	}
	
	const String& PS_Illuminance::getData()
	{
		int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		
		if(PollingSensor::debug)
		{
			Everything::Return_String="DEBUG: \""+String(getId())+"\" "+String(m_nSensorValue);
			return Everything::Return_String;
		}
		else
		{
			return Constants::IGNORE_STRING;
		}
	}
	
	void PS_Illuminance::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}