//******************************************************************************************
//  File: PS_Illuminance.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  value of a simple photo resistor.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Illuminance sensor1("illuminance1", 120, 0, PIN_ILLUMINANCE, 0, 1023, 0, 1000);
//
//			  st::PS_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- int s_l - OPTIONAL - first argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- int s_h - OPTIONAL - second argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- int m_l - OPTIONAL - third argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- int m_h - OPTIONAL - fourth argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to 
//			  the ST Shield.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "PS_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Illuminance::PS_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, int s_l, int s_h, int m_l, int m_h):
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

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Illuminance::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Illuminance::beSmart set polling interval to "));
				Serial.println(s.toInt());
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
	}

	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_Illuminance::getData()
	{
		int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		
		Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
	}
	
	void PS_Illuminance::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}