#include "PS_Water.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor
	PS_Water::PS_Water(const String &name, unsigned int interval, int offset, byte analogInputPin):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0)
	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_Water::~PS_Water()
	{
		
	}

	const String& PS_Water::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		
		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Water::beSmart set polling interval to "));
				Serial.println(s.toInt() * 1000);
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Water::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
		
		return Constants::IGNORE_STRING;
	}
	
	const String& PS_Water::getData()
	{
		int m_nSensorValue = analogRead(m_nAnalogInputPin);
		
		Everything::Return_String.remove(0);
		Everything::Return_String+=getName() + (m_nSensorValue<100?F(" dry"):F(" wet"));
		return Everything::Return_String;
		
	}
	
	void PS_Water::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}