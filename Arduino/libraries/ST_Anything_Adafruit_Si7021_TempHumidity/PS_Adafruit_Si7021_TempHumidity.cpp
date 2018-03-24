//******************************************************************************************
//  File: PS_Adafruit_BME280_TemperatureHumidityBarometric.cpp
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

#include "PS_Adafruit_Si7021_TempHumidity.h"

#include "Constants.h"
#include "Everything.h"
#include "Adafruit_Si7021.h"


namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Adafruit_Si7021_TempHumidity::PS_Adafruit_Si7021_TempHumidity(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp, String strHumid, bool In_C, byte filterConstant) :
		PollingSensor(name, interval, offset),
		m_fTemperatureSensorValue(-1.0),
		m_fHumiditySensorValue(-1.0),
		m_strTemperature(strTemp),
		m_strHumidity(strHumid),
		m_In_C(In_C)
	{

		//check for upper and lower limit and adjust accordingly
		if ((filterConstant <= 0) || (filterConstant >= 100))
		{
			m_fFilterConstant = 1.0;
		}
		else if (filterConstant <= 5)
		{
			m_fFilterConstant = 0.05;
		}
		else
		{
			m_fFilterConstant = float(filterConstant) / 100;
		}

	}
	
	//destructor
	PS_Adafruit_Si7021_TempHumidity::~PS_Adafruit_Si7021_TempHumidity()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Adafruit_Si7021_TempHumidity::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Adafruit_Si7021_TempHumidity::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				Serial.print(F("PS_Adafruit_Si7021_TempHumidity::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_Adafruit_Si7021_TempHumidity::init()
	{
		delay(1500);		//Needed to prevent "Unknown Error" on first read of DHT Sensori (possibly not necessary with Si7021)
		
		bool status;
		status = Si7021Sensor.begin();
		if (!status) {
			Serial.println("Could not find a valid Si7021 sensor, check wiring!");
			while (1);
		}

		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_Adafruit_Si7021_TempHumidity::getData()
	{
		// READ DATA
		//Humidity
		if (m_fHumiditySensorValue == -1.0)
		{
			Serial.println("First time through Humidity)");
			m_fHumiditySensorValue = Si7021Sensor.readHumidity();  //first time through, no filtering
		}
		else
		{
			m_fHumiditySensorValue = (m_fFilterConstant * Si7021Sensor.readHumidity()) + (1 - m_fFilterConstant) * m_fHumiditySensorValue;
		}

		//Temperature
		if (m_fTemperatureSensorValue == -1.0)
		{
			Serial.println("First time through Termperature)");
			//first time through, no filtering
			if (m_In_C == false)
			{
				m_fTemperatureSensorValue = (Si7021Sensor.readTemperature() * 1.8) + 32.0;		//Scale from Celsius to Farenheit
			}
			else
			{
				m_fTemperatureSensorValue = Si7021Sensor.readTemperature();
			}
		}
		else
		{
			if (m_In_C == false)
			{
				m_fTemperatureSensorValue = (m_fFilterConstant * ((Si7021Sensor.readTemperature() * 1.8) + 32.0)) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
			}
			else
			{
				m_fTemperatureSensorValue = (m_fFilterConstant * Si7021Sensor.readTemperature()) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
			}
			
		}


		// DISPLAY DATA
		//Serial.print(m_nHumiditySensorValue, 1);
		//Serial.print(F(",\t\t"));
		//Serial.print(m_nTemperatureSensorValue, 1);
		//Serial.println();

		Everything::sendSmartString(m_strTemperature + " " + String(m_fTemperatureSensorValue));
		Everything::sendSmartString(m_strHumidity + " " + String(m_fHumiditySensorValue));
	}
	

	//initialize static members
	Adafruit_Si7021 PS_Adafruit_Si7021_TempHumidity::Si7021Sensor = Adafruit_Si7021();

}
