#include "PS_Illuminance.h"


namespace st
{
//private
	

//public
	//constructor
	PS_Illuminance::PS_Illuminance(const String &name, unsigned int interval, int offset, byte analogInputPin, int s_l, int s_h, int m_l, int m_h):
		PollingSensor(name, interval, offset),
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

	void PS_Illuminance::init()
	{
		
	}
	
	bool PS_Illuminance::getData()
	{
		int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		if(PollingSensor::debug)
		{
			Serial.print("Illuminance sensor value (");
			Serial.print(MAPPED_LOW);
			Serial.print("-");
			Serial.print(MAPPED_HIGH);
			Serial.print(") = ");
			Serial.println(m_nSensorValue);
		}
		return true;
	}
	
	void PS_Illuminance::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}