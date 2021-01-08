//******************************************************************************************
//  File: PS_Water.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Water is a class which implements both the SmartThings "Water Sensor" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  presence of water using an inexpensive water sensor.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Water sensor3(F("water1"), 60, 6, PIN_WATER, 200, false);
//
//			  st::PS_Water() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- int limit - OPTIONAL - the alarm limit to compare analog pin's reading to, above which the sensor reports "wet" instead of "dry", default = 100 
//				- bool invertLogic - OPTIONAL - if set to true, will invert the comparison against target from < to >, default = false 
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
//    2015-08-23  Dan			 Added optional alarm limit to constructor
//    2018-10-17  Dan            Added invertLogic parameter to constructor
//    2020-01-08  Andy A.        Ability to calculate wet/dry from sibling Ultrasonic sensor
//
//******************************************************************************************


#include "PS_Water.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Water::PS_Water(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, int limit, bool invertLogic):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0),
		m_nSensorLimit(limit),
		m_binvertLogic(invertLogic)
	{
		setPin(analogInputPin);
        m_bSkipInputPin = false;
	}
	
    // alternative constructor, for deriving wet/dry from another ultrasonic sensor
    PS_Water::PS_Water(const __FlashStringHelper *name, unsigned int interval, int offset, int limit, bool invertLogic):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0),
		m_nSensorLimit(limit),
		m_binvertLogic(invertLogic)
    {
        m_bSkipInputPin = true;
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
				Serial.println(s.toInt());
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
        // determine where to read data from
        if (m_bSkipInputPin) 
        {
            // use the existing sensor value (provided by another sensor)
        } 
        else 
        {
		    m_nSensorValue = analogRead(m_nAnalogInputPin);
        }

		if (st::PollingSensor::debug)
		{
            Serial.print(F("PS_Water::"));
            if (!m_bSkipInputPin) 
            {
			    Serial.print(F("Analog Pin value is "));
            } 
            else 
            {
                Serial.print(F("Explicit sensor value is "));
            }
			Serial.print(m_nSensorValue);
			Serial.print(F(" vs limit of "));
			Serial.print(m_nSensorLimit);
            Serial.print(F(" ("));
            if (!m_binvertLogic)
            {
                Serial.print(F("not "));
            }
            Serial.println(F("inverted)"));
		}

		//compare the sensor's value is against the limit to determine whether to send "dry" versus "wet".  
		if (m_binvertLogic)
		{
			Everything::sendSmartString(getName() + (m_nSensorValue > m_nSensorLimit ? F(" dry") : F(" wet")));
		}
		else
		{
			Everything::sendSmartString(getName() + (m_nSensorValue < m_nSensorLimit ? F(" dry") : F(" wet")));
		}
	}
	
	void PS_Water::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}

    void PS_Water::setSensorValue(int newValue) 
    {
        m_nSensorValue = newValue;
    }
}