//******************************************************************************************
//  File: PS_Voltage.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Voltage is a class which implements the SmartThings "Voltage Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an anlog input pin and then scale it to engineering units.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Voltage sensor1("voltage1", 120, 0, PIN_VOLTAGE, 0, 1023, 0, 5);
//
//			  st::PS_Voltage() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- double s_l - OPTIONAL - first argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double s_h - OPTIONAL - second argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double m_l - OPTIONAL - third argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double m_h - OPTIONAL - fourth argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
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
//    2015-04-19  Dan & Daniel   Original Creation
//    2017-08-18  Dan Ogorchock  Modified to return floating point values instead of integer
//
//
//******************************************************************************************


#include "PS_Voltage.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	float map_double(double x, double in_min, double in_max, double out_min, double out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Voltage::PS_Voltage(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double s_l, double s_h, double m_l, double m_h):
		PollingSensor(name, interval, offset),
		m_fSensorValue(0),
		SENSOR_LOW(s_l),
		SENSOR_HIGH(s_h),
		MAPPED_LOW(m_l),
		MAPPED_HIGH(m_h)
	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_Voltage::~PS_Voltage()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Voltage::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Voltage::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Voltage::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_Voltage::getData()
	{
		m_fSensorValue= map_double(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		
		Everything::sendSmartString(getName() + " " + String(m_fSensorValue));
	}
	
	void PS_Voltage::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}
