//******************************************************************************************
//  File: PS_AdafruitBME280_TempHumidPress.cpp
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
//    2019-07-01  Dan.t		 	 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//******************************************************************************************

#include "PS_AdafruitBME280_TempHumidPress.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
    PS_AdafruitBME280_TempHumidPress::PS_AdafruitBME280_TempHumidPress(const __FlashStringHelper *name, unsigned int interval, int offset, String strTemp, String strHumid, String strPressure, bool In_C, byte filterConstant, int address) :
	    PollingSensor(name, interval, offset),
		bme(),
		m_fTemperatureSensorValue(-1.0),
		m_fHumiditySensorValue(-1.0),
		m_fPressureSensorValue(-1.0),
		m_strTemperature(strTemp),
		m_strHumidity(strHumid),
		m_strPressure(strPressure),
		m_In_C(In_C),
		m_nAddress(address)
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
	PS_AdafruitBME280_TempHumidPress::~PS_AdafruitBME280_TempHumidPress()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitBME280_TempHumidPress::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				st::debugPrint(F("PS_AdafruitBME280_TempHumidPress::beSmart set polling interval to "));
				st::debugPrintln(String(s.toInt()));
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				st::debugPrint(F("PS_AdafruitBME280_TempHumidPress::beSmart cannot convert "));
				st::debugPrint(s);
				st::debugPrintln(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_AdafruitBME280_TempHumidPress::init()
	{
		char buf[5];
		sprintf(buf, "%02X", m_nAddress);

		bool status = bme.begin(m_nAddress);
		if (st::PollingSensor::debug)
		{
			if (!status) {
				st::debugPrintln(F(""));
				st::debugPrint("Error: Could not find a valid BME280 sensor at address 0x");
				st::debugPrint(buf);
				st::debugPrintln(", check wiring and address setting in sketch!");
				st::debugPrintln(F(""));
				delay(3000);
			}
		}

		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitBME280_TempHumidPress::getData()
	{

			//Humidity
			if (m_fHumiditySensorValue == -1.0)
			{
				st::debugPrintln("First time through Humidity");
				m_fHumiditySensorValue = bme.readHumidity();  //first time through, no filtering
			}
			else
			{
				m_fHumiditySensorValue = (m_fFilterConstant * bme.readHumidity()) + (1 - m_fFilterConstant) * m_fHumiditySensorValue;
			}

			//Temperature
			if (m_fTemperatureSensorValue == -1.0)
			{
				st::debugPrintln("First time through Temperature");
				//first time through, no filtering
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (bme.readTemperature() * 1.8) + 32.0;		//Scale from Celsius to Farenheit
				}
				else
				{
					m_fTemperatureSensorValue = bme.readTemperature();
				}
			}
			else
			{
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * ((bme.readTemperature() * 1.8) + 32.0)) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}
				else
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * bme.readTemperature()) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}

			}
			
			//Pressure
			if (m_fPressureSensorValue == -1.0)
			{
				st::debugPrintln("First time through Pressure");
				m_fPressureSensorValue = (bme.readPressure() / 100.0F);  //first time through, no filtering
			}
			else
			{
				m_fPressureSensorValue = (m_fFilterConstant * (bme.readPressure() / 100.0F)) + (1 - m_fFilterConstant) * m_fPressureSensorValue;
			}

		Everything::sendSmartString(m_strTemperature + " " + String(m_fTemperatureSensorValue));
		Everything::sendSmartString(m_strHumidity + " " + String(m_fHumiditySensorValue));
		Everything::sendSmartString(m_strPressure + " " + String(m_fPressureSensorValue));

	}
	
}