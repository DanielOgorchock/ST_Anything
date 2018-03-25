//******************************************************************************************
//  File: PS_Adafruit_Si7021_TempHumidity.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son) & Josh Hill
//
//  Summary:  PS_Adafruit_Si7021_TempHumidity is a class which implements both the SmartThings "Temperature Measurement" 
//			  and "Relative Humidity Measurement" device capabilities.
//			  It inherits from the st::PollingSensor class.  The current version uses I2C to measure the
//			  temperature and humidity from an Si7021 sensor.  This was tested with a generic Si7021 sensor from AliExpress.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Adafruit_Si7021_TempHumidity sensor2("temphumid1", 120, 7, "temperature1", "humidity1", false);
//
//			  st::PS_Adafruit_Si7021_TempHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- String strTemp - OPTIONAL - name of temperature sensor to send to ST Cloud (defaults to "temperature")
//				- String strHumid - OPTIONAL - name of humidity sensor to send to ST Cloud (defaults to "humidity")
//				- bool In_C - OPTIONAL - true = Report Celsius, false = Report Farenheit (Farentheit is the default)
//				- byte filterConstant - OPTIONAL - Value from 5% to 100% to determine how much filtering/averaging is performed 100 = none (default), 5 = maximum
//
//            Filtering/Averaging
//
//            Filtering the value sent to ST is performed per the following equation
//
//            filteredValue = (filterConstant/100 * currentValue) + ((1 - filterConstant/100) * filteredValue) 
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
//	  2015-01-17  Dan Ogorchock	 Added optional temperature and humidity device names in constructor to allow multiple Temp/Humidity sensors
//    2015-01-17  Dan Ogorchock	 Added optional temperature and humidity device names in constructor to allow multiple Temp/Humidity sensors
//    2015-03-29  Dan Ogorchock	 Optimized use of the DHT library (made it static) to reduce SRAM memory usage at runtime.
//    2017-06-27  Dan Ogorchock  Added optional Celsius reading argument
//    2017-08-17  Dan Ogorchock  Added optional filter constant argument and to transmit floating point values to SmartThings
//    2018-03-24  Josh Hill      Modified library to use Si7021 over I2C
//
//******************************************************************************************

#ifndef ST_PS_SI7021_H
#define ST_PS_SI7021_H

#include "PollingSensor.h"
#include "Adafruit_Si7021.h"

namespace st
{
	class PS_Adafruit_Si7021_TempHumidity: public PollingSensor
	{
		private:
			float m_fTemperatureSensorValue;//current Temperature value
			float m_fHumiditySensorValue;	//current Humidity Value
			static Adafruit_Si7021 Si7021Sensor;		//I2C

			String m_strTemperature;		//name of temparature sensor to use when transferring data to ST Cloud
			String m_strHumidity;			//name of temparature sensor to use when transferring data to ST Cloud	
			bool m_In_C;					//Return temp in C
			float m_fFilterConstant;        //Filter constant % as floating point from 0.00 to 1.00

		public:

			//constructor - called in your sketch's global variable declaration section
			PS_Adafruit_Si7021_TempHumidity(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp = "temperature1", String strHumid = "humidity1", bool In_C = false, byte filterConstant = 100);
			
			//destructor
			virtual ~PS_Adafruit_Si7021_TempHumidity();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline float getTemperatureSensorValue() const { return m_fTemperatureSensorValue; }
			inline float getHumiditySensorValue() const { return m_fHumiditySensorValue; }
				
	};
}
#endif
