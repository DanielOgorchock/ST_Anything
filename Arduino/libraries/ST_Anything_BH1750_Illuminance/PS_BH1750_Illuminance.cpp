//******************************************************************************************
//  File: PS_BH1750_Illuminance.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_BH1750_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the Illuminace from a MAX44009 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_BH1750_Illuminance sensor1(F("illuminance1"), 60, 0);  (simple, uses defaults)
//			  For Example:  st::PS_BH1750_Illuminance sensor1(F("illuminance1"), 60, 0, BH1750_ADDR_LOW); (full user control of settings)
//
//			  st::PS_BH1750_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//              - uint8_t addr -OPTIONAL - defaults to BH1750_ADDR_LOW
//
//
//              I2C address options
//                BH1750_ADDR_LOW               0x23      //< Pin A0 pulled Low
//                BH1750_ADDR_HIGH              0x5C      //< Pin A0 pulled Hi
//
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
//    2018-07-03  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#include "PS_BH1750_Illuminance.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_BH1750_Illuminance::PS_BH1750_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, uint8_t addr) :
		PollingSensor(name, interval, offset),
		myLux(addr)
	{

	}
	
	//destructor
	PS_BH1750_Illuminance::~PS_BH1750_Illuminance()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_BH1750_Illuminance::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_BH1750_Illuminance::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_BH1750_Illuminance::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_BH1750_Illuminance::init() {

		//initialize the BH1750
		myLux.begin();

		//give sensor time to get its first data sample ready to be read
		delay(1000); 

		//read and transmit initial data from sensor
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_BH1750_Illuminance::getData()
	{
		/* Get a new sensor data */
		m_nLux = myLux.readLightLevel();

		//Send data to SmartThings/Hubitat
		Everything::sendSmartString(getName() + " " + String(m_nLux));

	}
	
}
