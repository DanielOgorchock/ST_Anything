//******************************************************************************************
//  File: PS_AdafruitVEML7700_Illuminance.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitVEML7700_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the Illuminace from a VEML7700 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitVEML7700_Illuminance sensor1(F("illuminance1"), 60, 0);  (simple, uses defaults)
//			  For Example:  st::PS_AdafruitVEML7700_Illuminance sensor1(F("illuminance1"), 60, 0, VEML7700_IT_50MS, VEML7700_GAIN_1_8); (full user control of settings)
//
//			  st::PS_AdafruitVEML7700_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- uint8_t integrationTime - OPTIONAL - defaults to VEML7700_IT_50MS
//				- uint8_t gain - OPTIONAL - defaults to VEML7700_GAIN_1_8
//
//
//              I2C address options
//                  VEML7700_I2CADDR_DEFAULT 0x10		//< Default address
//
//              Valid Integration Time Values:
//					VEML7700_IT_100MS           0x00
//					VEML7700_IT_200MS           0x01
//					VEML7700_IT_400MS           0x02
//					VEML7700_IT_800MS           0x03
//					VEML7700_IT_50MS            0x08	//< Default integration time
//					VEML7700_IT_25MS            0x0C
//
//				Valid Gain Values:
//					VEML7700_GAIN_1             0x00
//					VEML7700_GAIN_2             0x01
//					VEML7700_GAIN_1_8           0x02	//< Default gain
//					VEML7700_GAIN_1_4           0x03
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
//    2019-09-28  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************


#include "PS_AdafruitVEML7700_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_AdafruitVEML7700_Illuminance::PS_AdafruitVEML7700_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, uint8_t integrationTime, uint8_t gain) :
		PollingSensor(name, interval, offset),
		veml(),
		m_nIntegrationTime(integrationTime),
		m_nGain(gain)
	{
	}
	
	//destructor
	PS_AdafruitVEML7700_Illuminance::~PS_AdafruitVEML7700_Illuminance()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitVEML7700_Illuminance::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_AdafruitVEML7700_Illuminance::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_AdafruitVEML7700_Illuminance::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_AdafruitVEML7700_Illuminance::init() {
		Serial.println("Initializing the VEML7700 sensor...");
		if (veml.begin()) {
			Serial.println("Found VEML7700 sensor.");
		}
		else {
			Serial.println("No VEML7700 found... check your wiring and address");
		}

		veml.setGain(m_nGain);
		veml.setIntegrationTime(m_nIntegrationTime);
		delay(100); //slight delay to allow sensor to initialize for accurate first reading
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitVEML7700_Illuminance::getData()
	{
		/* Get a new sensor data */

		m_nLux = (long) veml.readLux();
		//Serial.println(m_nLux);

		/* Display the results (light is measured in lux) */
		if ((m_nLux >= 0) && (m_nLux <= 120000))
		{
			//send data to SmartThings/Hubitat
			Everything::sendSmartString(getName() + " " + String(m_nLux));
		}
		else
		{
			/* No reliable data could be generated! */
			Serial.println("VEML7700 Sensor failure");
		}

	}
	
}
