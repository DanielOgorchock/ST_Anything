//******************************************************************************************
//  File: PS_AdafruitThermocouple.cpp
//  Authors: Dan G Ogorchock 
//
//  Summary:  PS_AdafruitThermocouple is a class which implements the SmartThings "Temperature Measurement" 
//			  device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  temperature Adafruit MAX31855 series thermocouple sensor.    
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitThermocouple sensor1(F("temperature1"), 120, 3, PIN_SCLK, PIN_CS, PIN_MISO);
//
//			  st::PS_TemperatureHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- int8_t pinSCLK - REQUIRED - the Arduino Pin to be used as the MAX31855 SCLK
//				- int8_t pinCS - REQUIRED - the Arduino Pin to be used as the MAX31855 CS
//				- int8_t pinMISO - REQUIRED - the Arduino Pin to be used as the MAX31855 MISO
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
//    2015-03-24  Dan Ogorchock  Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2019-06-24  Dan Ogorchock  Improved false reading handling
//    2019-07-01  Dan.t		 	 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//
//******************************************************************************************

#include "PS_AdafruitThermocouple.h"

#include "Constants.h"
#include "Everything.h"
#include <SPI.h>
#include <Adafruit_MAX31855.h>

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_AdafruitThermocouple::PS_AdafruitThermocouple(const __FlashStringHelper *name, unsigned int interval, int offset, int8_t pinSCLK, int8_t pinCS, int8_t pinMISO):
		PollingSensor(name, interval, offset),
		m_dblTemperatureSensorValue(0.0),
		m_Adafruit_MAX31855(Adafruit_MAX31855(pinSCLK, pinCS, pinMISO))
	{

	}
	
	//destructor
	PS_AdafruitThermocouple::~PS_AdafruitThermocouple()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitThermocouple::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				st::debugPrint(F("PS_AdafruitThermocouple::beSmart set polling interval to "));
				st::debugPrintln(String(s.toInt()));
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				st::debugPrint(F("PS_AdafruitThermocouple::beSmart cannot convert "));
				st::debugPrint(s);
				st::debugPrintln(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_AdafruitThermocouple::init()
	{		
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitThermocouple::getData()
	{
		double tempTemperature;
		double totalTemperature = 0.0;
		int numGoodValues = 0;

		for (int i = 1; i <= 3; i++) {
			tempTemperature = m_Adafruit_MAX31855.readFarenheit();
			if (isnan(tempTemperature))
			{
				if (st::PollingSensor::debug) {
				st::debugPrintln(F("PS_AdafruitThermocouple:: Error Reading Thermocouple"));
				}
			}
			else
			{
				totalTemperature = totalTemperature + tempTemperature;
				numGoodValues++;
			}
		}

		if (numGoodValues > 0) {
			m_dblTemperatureSensorValue = totalTemperature / numGoodValues;
			Everything::sendSmartString(getName() + " " + String(int(m_dblTemperatureSensorValue)));
		}
		else
		{
			st::debugPrintln(F("PS_AdafruitThermocouple:: Error Reading Thermocouple multiple times.  No good value measured this cycle."));
		}

	}
	
}