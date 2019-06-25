//******************************************************************************************
//  File: PS_MKR_THERM.cpp
//  Authors: Dan G Ogorchock 
//
//  Summary:  PS_MKR_THERM is a class which implements the SmartThings "Temperature Measurement" 
//			  device capability.
//			  It inherits from the st::PollingSensor class.  This class implements support for 
//            Arduino MKR THERM Thermocouple Shield.    
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_MKR_THERM sensor1(F("temperature1"), 120, 3);
//
//			  st::PS_TemperatureHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
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
//    2019-06-23  Dan Ogorchock  Original Creation
//    2019-06-24  Dan Ogorchock  Improved false reading handling
//
//
//******************************************************************************************

#include "PS_MKR_THERM.h"

#include "Constants.h"
#include "Everything.h"

#include <Arduino_MKRTHERM.h>

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_MKR_THERM::PS_MKR_THERM(const __FlashStringHelper *name, unsigned int interval, int offset):
		PollingSensor(name, interval, offset),
		m_dblTemperatureSensorValue(-99.0)
	{

	}
	
	//destructor
	PS_MKR_THERM::~PS_MKR_THERM()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_MKR_THERM::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_MKR_THERM::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				Serial.print(F("PS_MKR_THERM::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_MKR_THERM::init()
	{	
		if (!THERM.begin()) {
			Serial.println("Failed to initialize MKR THERM shield!");
		}
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_MKR_THERM::getData()
	{
		double tempTemperature;
		double totalTemperature = 0.0;
		int numGoodValues = 0;

		for (int i = 1; i <= 3; i++) {
			tempTemperature = THERM.readTemperature();
			if (isnan(tempTemperature))
			{
				if (st::PollingSensor::debug) {
					Serial.println(F("PS_MKR_THERM:: Error Reading Thermocouple."));
				}
			}
			else
			{
				totalTemperature = totalTemperature + tempTemperature;
				numGoodValues++;
			}
		}

		if (numGoodValues > 0) {
			m_dblTemperatureSensorValue = totalTemperature/numGoodValues;
			Everything::sendSmartString(getName() + " " + String(int(m_dblTemperatureSensorValue)));
		}
		else
		{
			Serial.println(F("PS_MKR_THERM:: Error Reading Thermocouple multiple times.  No good value measured this cycle."));
		}
	}
	
}