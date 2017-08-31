//******************************************************************************************
//  File: PS_18B20_Temperature.cpp
//  Author: Dan G Ogorchock
//
//  Summary:  PS_DS18B20_Temperature is a class which implements both the SmartThings "Temperature Measurement" capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital pin to measure the 
//			  temperature from a Dallas Semiconductor One Wire DS18B20 series sensor. 
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_DS18B20_Temperature sensor1("temperature1", 120, 0, PIN_TEMPERATURE, false);
//
//			  st::PS_DS18B20_Temperature() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used for the One-Wire DS18B20 sensor conenction
//				- bool In_C - OPTIONAL - true = Report Celsius, false = Report Farenheit (Farentheit is the default)
//				- byte resolution - OPTIONAL - DS18B20 sensor resolution in bits.  9, 10, 11, or 12.  Defaults to 10 for decent accuracy and performance
//				- byte num_sensors - OPTIONAL - number of OneWire DS18B20 sensors attached to OneWire bus - Defaults to 1
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
//    2015-10-08  Matt Boykin    Original Creation
//	  2016-02-19  Dan Ogorchock	 Cleaned Up for inclusing in the ST_Anything Project
//    2016-02-27  Dan Ogorchock  Added support for multiple DS18B20 sensors
//    2017-08-18  Dan Ogorchock  Modified to send floating point values to SmartThings
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************
#include "PS_DS18B20_Temperature.h"

#include "Constants.h"
#include "Everything.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

namespace st
{
//private


//public
	//constructor - called in your sketch's global variable declaration section
	PS_DS18B20_Temperature::PS_DS18B20_Temperature(const __FlashStringHelper *name, unsigned int interval, int offset, byte pin, bool In_C, byte resolution, byte num_sensors) :
		PollingSensor(name, interval, offset),
		m_dblTemperatureSensorValue(0.0),
		m_OneWireBus(pin),
		m_DS18B20(&m_OneWireBus),
		m_In_C(In_C),
		m_Resolution(resolution),
		m_numSensors(num_sensors)
	{
		
	}

	//destructor
	PS_DS18B20_Temperature::~PS_DS18B20_Temperature()
	{

	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_DS18B20_Temperature::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_DS18B20_Temperature::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_DS18B20_Temperature::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_DS18B20_Temperature::init()
	{
		m_DS18B20.begin();					   //Initialize the DallasTemperature library
		m_DS18B20.setResolution(m_Resolution); //Set the temperature sensor resolution
		m_DS18B20.requestTemperatures();	   //Send the command to get temperatures once, to ensure clean data before sending to ST
		delay(500);						       //Wait 500ms to let sensors stabilize
		getData();							   //Get temperature data and send to ST cloud
	}

	//function to get data from sensor and queue results for transfer to ST Cloud
	void PS_DS18B20_Temperature::getData()
	{
		if (st::PollingSensor::debug) {
			Serial.print(F("PS_DS18B20_Temperature::Requesting temperatures..."));
		}
		
		m_DS18B20.requestTemperatures(); // Send the command to get temperatures

		if (st::PollingSensor::debug) {
			Serial.println(F("DONE"));
		}

		for (int index = 1; index <= m_numSensors; index++)
		{
			if (m_In_C)
			{
				m_dblTemperatureSensorValue = m_DS18B20.getTempCByIndex(index-1);
			}
			else
			{
				m_dblTemperatureSensorValue = m_DS18B20.getTempFByIndex(index-1);
			}

			if (st::PollingSensor::debug) {
				Serial.print(F("PS_DS18B20_Temperature:: Temperature for the device # "));
				Serial.print(index);
				Serial.print(F(" is: "));
				Serial.println(m_dblTemperatureSensorValue);
			}


			if (isnan(m_dblTemperatureSensorValue))
			{
				if (st::PollingSensor::debug) {
					Serial.print(F("PS_DS18B20_Temperature:: Error Reading Sensor # "));
					Serial.println(index);
				}
				m_dblTemperatureSensorValue = -99.0;
			}

			if (m_numSensors == 1)
			{
				Everything::sendSmartString(getName() + " " + String(m_dblTemperatureSensorValue));
			}
			else
			{
				Everything::sendSmartString(getName() + index + " " + String(m_dblTemperatureSensorValue));
			}
		}
	}

}



