//******************************************************************************************
//  File: PS_SoundPressureLevel.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_SoundPressureLevel is a class which implements the "Sound Pressure Level" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an anlog input pin and then scale it to engineering units.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (e.g. 0 to 1024) to Engineering Units before sending to SmartThings. 
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example: static st::PS_SoundPressureLevel sensor1(F("soundPressureLevel1"), 60, 0, PIN_SPL, 0, 1024, 0.0, 165.0, 50);
//
//			  st::PS_SoundPressureLevel() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- double s_l - OPTIONAL - first argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - minimum raw AI value
//				- double s_h - OPTIONAL - second argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - maximum raw AI value
//				- double m_l - OPTIONAL - third argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - Engineering Unit Min (or Max if inverting)
//				- double m_h - OPTIONAL - fourth argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - Engineering Unit Max (or Min if inverting)
//              - long m_nHighSpeedPollingInterval - OPTIONAL - number of milliseconds between high speed analog reads - defaults to 50ms
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
//    2019-07-08  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************
#include "PS_SoundPressureLevel.h"

#include "Constants.h"
#include "Everything.h"
#include <math.h>

namespace st
{
//private
	float PS_SoundPressureLevel::map_double(double x, double in_min, double in_max, double out_min, double out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

//public
	//constructor - called in your sketch's global variable declaration section
	PS_SoundPressureLevel::PS_SoundPressureLevel(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double s_l, double s_h, double m_l, double m_h, long HighSpeedPollingInterval):
		PollingSensor(name, interval, offset),
		m_fSensorValue(-1.0),
		SENSOR_LOW(s_l),
		SENSOR_HIGH(s_h),
		MAPPED_LOW(m_l),
		MAPPED_HIGH(m_h),
		m_nHighSpeedPollingInterval(HighSpeedPollingInterval)
	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_SoundPressureLevel::~PS_SoundPressureLevel()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_SoundPressureLevel::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_SoundPressureLevel::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_SoundPressureLevel::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_SoundPressureLevel::update()
	{

		//read the SPL value if enough time has elapsed
		if ((millis() - m_nPrevMillis) > m_nHighSpeedPollingInterval) {
			m_nPrevMillis = millis();

			//read analog input
			long tempAnalogInput = analogRead(m_nAnalogInputPin);

			//scale raw AI value into Engineering Units
			float tempValue = map_double(tempAnalogInput, SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);

			//if new value is greater than previous value, store the new value (i.e. keep the MAX SPL)
			if (tempValue > m_fSensorValue) {
				m_fSensorValue = tempValue;
			}
		}

		//make sure to call the parent class' update function, since we've overriden update()
		PollingSensor::update();

	}

	//function to get data from sensor and queue results for transfer to Hub 
	void PS_SoundPressureLevel::getData()
	{	
		//make sure we have at least one reading before transferring data
		update();

		//transfer the data to the hub
		Everything::sendSmartString(getName() + " " + String(m_fSensorValue));
		
		//reset the max value
		m_fSensorValue = -1.0;
	}
	
	void PS_SoundPressureLevel::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}
