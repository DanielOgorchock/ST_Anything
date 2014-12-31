#include "PS_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

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

	const String& PS_Illuminance::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Illuminance::beSmart set polling interval to "));
				Serial.println(s.toInt() * 1000);
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Illuminance::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}

		return Constants::IGNORE_STRING;
	}

	const String& PS_Illuminance::getData()
	{
		int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		
		Everything::Return_String.remove(0);
		Everything::Return_String+=getName() + " " + String(m_nSensorValue);
		return Everything::Return_String;
	}
	
	void PS_Illuminance::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}