#include "PS_Water.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
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

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Water::beSmart(const String &str)
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
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud
	void PS_Water::getData()
	{
		int m_nSensorValue = analogRead(m_nAnalogInputPin);
		
		//check to see if the sensor's value is < 100.  If so send "dry", otherwise send "wet".  Adjust the 100 as needed for your sensor.
		Everything::sendSmartString(getName() + (m_nSensorValue<100?F(" dry"):F(" wet")));
	}
	
	void PS_Water::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}