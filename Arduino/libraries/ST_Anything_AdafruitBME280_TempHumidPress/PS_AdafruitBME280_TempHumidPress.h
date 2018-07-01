//******************************************************************************************
//  File: PS_AdafruitBME280_TempHumidPress.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitBME280_TempHumidPress is a class which implements the "Temperature Measurement",  
//			  "Relative Humidity Measurement", and "PressureMewasurement" device capabilities.
//			  It inherits from the st::PollingSensor class.  The current version uses I2C to measure the 
//			  temperature, humidity, and pressure from a BME280 series sensor using the Adafruit_BME280 library.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitBME280_TempHumidPress sensor2("bme280_1", 60, 0, "temperature1", "humidity1", "pressure1", false, 100, 0x77);
//
//			  st::PS_AdafruitBME280_TempHumidPress() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must be unique, but is not used for data transfer for this device
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- String strTemp - REQUIRED - name of temperature sensor to send to ST Cloud (e.g."temperature1")
//				- String strHumid - REQUIRED - name of humidity sensor to send to ST Cloud (e.g. "humidity1")
//				- String strPressure - REQUIRED - name of pressure sensor to send to ST Cloud (e.g. "pressure1")
//				- bool In_C - OPTIONAL - true = Report Celsius, false = Report Farenheit (Farentheit is the default)
//				- byte filterConstant - OPTIONAL - Value from 5% to 100% to determine how much filtering/averaging is performed 100 = none (default), 5 = maximum
//              - int address - OPTIONAL - I2C address of the sensor (defaults to 0x77 for the BME280)
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
//    2018-07-01  Dan & Daniel   Original Creation
//
//******************************************************************************************

#ifndef ST_PS_AdafruitBME280_TempHumidPress_H
#define ST_PS_AdafruitBME280_TempHumidPress_H

#include "PollingSensor.h"
#include <Adafruit_BME280.h>

namespace st
{
	class PS_AdafruitBME280_TempHumidPress: public PollingSensor
	{
		private:
			int m_nAddress;                 //I2C Address of the sensor
			float m_fTemperatureSensorValue;//current Temperature value
			float m_fHumiditySensorValue;	//current Humidity Value
			float m_fPressureSensorValue;	//current Pressure Value
			Adafruit_BME280 bme;		    //I2C BME280 library object
			String m_strTemperature;		//name of temparature sensor to use when transferring data to ST Cloud / Hubitat
			String m_strHumidity;			//name of humidity sensor to use when transferring data to ST Cloud / Hubitat		
			String m_strPressure;			//name of pressure sensor to use when transferring data to ST Cloud / Hubitat		
			bool m_In_C;					//Return temp in C (true or false)
			float m_fFilterConstant;        //Filter constant % as floating point from 0.00 to 1.00

		public:
			//constructor - called in your sketch's global variable declaration section 
			PS_AdafruitBME280_TempHumidPress(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp = "temperature1", String strHumid = "humidity1", String strPressure = "pressure1", bool In_C = false, byte filterConstant = 100, int address = 0x77);

			//destructor
			virtual ~PS_AdafruitBME280_TempHumidPress();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline float getTemperatureSensorValue() const { return m_fTemperatureSensorValue; }
			inline float getHumiditySensorValue() const { return m_fHumiditySensorValue; }
			inline float getPressureSensorValue() const { return m_fPressureSensorValue; }
	};
}
#endif
