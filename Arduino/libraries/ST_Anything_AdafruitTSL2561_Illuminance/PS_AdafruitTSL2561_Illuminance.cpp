//******************************************************************************************
//  File: PS_AdafruitTSL2561_Illuminance.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitTSL2561_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the Illuminace from a TSL2561 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitTSL2561_Illuminance sensor1(F("illuminance1"), 60, 0);  (simple, uses defaults)
//			  For Example:  st::PS_AdafruitTSL2561_Illuminance sensor1(F("illuminance1"), 60, 0, TSL2561_ADDR_FLOAT, TSL2561_INTEGRATIONTIME_13MS, TSL2561_GAIN_1X); (full user control of settings)
//
//			  st::PS_AdafruitTSL2561_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//              - uint8_t addr -OPTIONAL - defaults to TSL2561_ADDR_FLOAT
//				- tsl2561IntegrationTime_t integrationTime - OPTIONAL - defaults to TSL2561_INTEGRATIONTIME_13MS
//				- tsl2561Gain_t gain - OPTIONAL - defaults to TSL2561_GAIN_1X
//
//
//              I2C address options
//                  TSL2561_ADDR_LOW          (0x29)    ///< Default address (pin pulled low)
//                  TSL2561_ADDR_FLOAT        (0x39)    ///< Default address (pin left floating)
//                  TSL2561_ADDR_HIGH         (0x49)    ///< Default address (pin pulled high)
//
//              Valid Integration Time Values:
//				    TSL2561_INTEGRATIONTIME_13MS  = 0x00,    // 13.7ms
//                  TSL2561_INTEGRATIONTIME_101MS = 0x01,    // 101ms
//                  TSL2561_INTEGRATIONTIME_402MS = 0x02     // 402ms
//
//				Valid Gain Values:
//				    TSL2561_GAIN_1X  = 0x00,    // No gain
//                  TSL2561_GAIN_16X = 0x10     // 16x gain
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
//    2018-07-02  Dan & Daniel   Original Creation
//
//
//******************************************************************************************


#include "PS_AdafruitTSL2561_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_AdafruitTSL2561_Illuminance::PS_AdafruitTSL2561_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, uint8_t addr, tsl2561IntegrationTime_t integrationTime, tsl2561Gain_t gain) :
		PollingSensor(name, interval, offset),
		tsl(addr)
	{
		tsl.setGain(gain);
		tsl.setIntegrationTime(integrationTime);
	}
	
	//destructor
	PS_AdafruitTSL2561_Illuminance::~PS_AdafruitTSL2561_Illuminance()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitTSL2561_Illuminance::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_AdafruitTSL2561_Illuminance::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_AdafruitTSL2561_Illuminance::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_AdafruitTSL2561_Illuminance::init() {
		Serial.println("Initializing the TSL2561 sensor...");
		if (tsl.begin()) {
			Serial.println("Found TSL2561 sensor.");
		}
		else {
			Serial.println("No TSL2561 found... check your wiring and address");
		}
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitTSL2561_Illuminance::getData()
	{
		/* Get a new sensor event */
		sensors_event_t event;
		tsl.getEvent(&event);
		//Serial.println(event.light);

		/* Display the results (light is measured in lux) */
		if ((event.light >= 0) && (event.light <= 65535))
		{
			m_nlux = event.light;
			//send data to SmartThings/Hubitat
			Everything::sendSmartString(getName() + " " + String(m_nlux));
		}
		else
		{
			/* If event.light = 0 lux the sensor is probably saturated
			and no reliable data could be generated! */
			Serial.println("TSL2561 Sensor overload");
		}

	}
	
}
