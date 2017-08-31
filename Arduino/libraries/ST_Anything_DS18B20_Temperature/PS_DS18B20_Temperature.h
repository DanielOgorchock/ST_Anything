//******************************************************************************************
//  File: PS_18B20_Temperature.h
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
#ifndef ST_PS_DS18B20_TEMPERATURE_H
#define ST_PS_DS18B20_TEMPERATURE_H


#include "PollingSensor.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

namespace st
{
	class PS_DS18B20_Temperature : public PollingSensor
	{
		private:
			float m_dblTemperatureSensorValue;		//current Temperature value
			OneWire m_OneWireBus;					//OneWire Bus
			DallasTemperature m_DS18B20;			//Dallas Temperature object
			byte m_Resolution;						//DS18B20 Resolution in bits - 9, 10, 11, or 12
			bool m_In_C;							//Return temp in C
			byte m_numSensors;						//number of DS18B20 sensors to report values for

		public:

			//constructor - called in your sketch's global variable declaration section
			PS_DS18B20_Temperature(const __FlashStringHelper *name, unsigned int interval, int offset, byte pin, bool In_C = false, byte resolution = 10, byte num_sensors = 1);

			//destructor
			virtual ~PS_DS18B20_Temperature();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud
			virtual void getData();

			//gets
			inline float getTemperatureSensorValue() const { return float(m_dblTemperatureSensorValue); }

			//sets

	};
}

#endif /* ST_PS_DS18B20_TEMPERATURE_H */
