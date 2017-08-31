//******************************************************************************************
//  File: PS_AdafruitThermocouple.h
//  Authors: Dan G Ogorchock 
//
//  Summary:  PS_AdafruitThermocouple is a class which implements the SmartThings "Temperature Measurement" 
//			  device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  temperature Adafruit MAX31855 series thermocouple sensor.    
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitThermocouple sensor1("temperature1", 120, 3, PIN_SCLK, PIN_CS, PIN_MISO);
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
//
//
//******************************************************************************************

#ifndef ST_PS_ADAFRUITTHERMOCOUPLE_H
#define ST_PS_ADAFRUITTHERMOCOUPLE_H

#include "PollingSensor.h"
#include <SPI.h>
#include <Adafruit_MAX31855.h>

namespace st
{
	class PS_AdafruitThermocouple: public PollingSensor
	{
		private:
			double m_dblTemperatureSensorValue;		//current Temperature value
			Adafruit_MAX31855 m_Adafruit_MAX31855;	//Adafruit MAX31855 object

		public:

			//constructor - called in your sketch's global variable declaration section
			PS_AdafruitThermocouple(const __FlashStringHelper *name, unsigned int interval, int offset, int8_t pinSCLK, int8_t pinCS, int8_t pinMISO);
			
			//destructor
			virtual ~PS_AdafruitThermocouple();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline int getTemperatureSensorValue() const { return int(m_dblTemperatureSensorValue); }
				
			//sets
	
	};
}



#endif