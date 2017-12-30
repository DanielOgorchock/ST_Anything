//******************************************************************************************
//  File: PS_AdafruitTCS34725_Illum_Color.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitTCS34725_Illum_Color is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  value of a simple photo resistor.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitTCS34725_Illum_Color sensor1("illuminance1", 120, 0, PIN_ILLUMINANCE, 0, 1023, 0, 1000);
//
//			  st::PS_AdafruitTCS34725_Illum_Color() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
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
//    2015-01-03  Dan & Daniel   Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2017-09-07  Allan (vseven) Modified original PS_Illuminance library for use with the Adafruit TCS34725 sensor
//    2017-12-29  Allan (vseven) Fixed bug with improper init() definition per Dans guidance
//
//
//******************************************************************************************

#include "PS_AdafruitTCS34725_Illum_Color.h"

#include "Constants.h"
#include "Everything.h"

#include "Adafruit_TCS34725.h"

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_AdafruitTCS34725_Illum_Color::PS_AdafruitTCS34725_Illum_Color(const __FlashStringHelper *name, unsigned int interval, int offset):
		PollingSensor(name, interval, offset),m_nSensorValue(0)
	{

	}
	
	//destructor
	PS_AdafruitTCS34725_Illum_Color::~PS_AdafruitTCS34725_Illum_Color()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitTCS34725_Illum_Color::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_AdafruitTCS34725_Illum_Color::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_AdafruitTCS34725_Illum_Color::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_AdafruitTCS34725_Illum_Color::init() {
	  	Serial.println("Initiating the TCS34725 sensor...");
  		if (tcs.begin()) {
			Serial.println("Found sensor.   tcs.begin = true");
		} else {
			Serial.println("No TCS34725 found ... check your connections");
		}
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitTCS34725_Illum_Color::getData()
	{
		//Serial.println("getData routine called");
		uint16_t lux, colorTemp, red, green, blue, clear;

  		tcs.getRawData(&red, &green, &blue, &clear);
  		colorTemp = tcs.calculateColorTemperature(red, green, blue);
 		lux = tcs.calculateLux(red, green, blue);

		//Serial.println(lux,DEC);
		//Serial.println(colorTemp,DEC);
		//Serial.println(red,DEC);
		//Serial.println(green,DEC);
		//Serial.println(blue,DEC);
		//Serial.println(clear,DEC);

		String m_nSensorValue = String(lux, DEC) + ':' + String(colorTemp, DEC) + ':' + String(red, DEC) + ':' + String(green, DEC) + ':' + String(blue, DEC) + ':' + String(clear, DEC);

		//Serial.println(getName());
		//Serial.println(m_nSensorValue);		

		Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
	}
	
}
