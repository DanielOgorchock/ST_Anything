//******************************************************************************************
//  File: PS_TemperatureHumidity.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_TemperatureHumidity is a class which implements both the SmartThings "Temperature Measurement" 
//			  and "Relative Humidity Measurement" device capabilities.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  temperature and humidity from a DHT series sensor.  This was tested with both the DHT11 and DHT22.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_TemperatureHumidity sensor2("temphumid1", 120, 7, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22, "temperature1", "humidity1", false);
//
//			  st::PS_TemperatureHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- DHT_SENSOR DHTSensorType - REQUIRED - the type of DHT sensor (DHT11, DHT21, DHT22, DHT33, or DHT44)
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
//
//******************************************************************************************

#include "PS_TemperatureHumidity.h"

#include "Constants.h"
#include "Everything.h"
#include <dht.h>

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_TemperatureHumidity::PS_TemperatureHumidity(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalInputPin, DHT_SENSOR DHTSensorType, String strTemp, String strHumid, bool In_C, byte filterConstant) :
		PollingSensor(name, interval, offset),
		m_fTemperatureSensorValue(-1.0),
		m_fHumiditySensorValue(-1.0),
		m_bDHTSensorType(DHTSensorType),
		m_strTemperature(strTemp),
		m_strHumidity(strHumid),
		m_In_C(In_C)
	{
		setPin(digitalInputPin);

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
	PS_TemperatureHumidity::~PS_TemperatureHumidity()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_TemperatureHumidity::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_TemperatureHumidity::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug) 
			{
				Serial.print(F("PS_TemperatureHumidity::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//initialization routine - get first set of readings and send to ST cloud
	void PS_TemperatureHumidity::init()
	{
		delay(1500);		//Needed to prevent "Unknown Error" on first read of DHT Sensor
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_TemperatureHumidity::getData()
	{
		// READ DATA
		int8_t chk = 0;
		switch (m_bDHTSensorType) {
			case DHT11:
				//Serial.println(F("PS_TemperatureHumidity: DTH11 Read"));
				chk = DHT.read11(m_nDigitalInputPin);
				break;
			case DHT21:
				//Serial.println(F("PS_TemperatureHumidity: DTH21 Read"));
				chk = DHT.read21(m_nDigitalInputPin);
				break;
			case DHT22:
				//Serial.println(F("PS_TemperatureHumidity: DTH22 Read"));
				chk = DHT.read22(m_nDigitalInputPin);
				break;
			case DHT33:
				//Serial.println(F("PS_TemperatureHumidity: DTH33 Read"));
				chk = DHT.read33(m_nDigitalInputPin);
				break;
			case DHT44:
				//Serial.println(F("PS_TemperatureHumidity: DTH44 Read"));
				chk = DHT.read44(m_nDigitalInputPin);
				break;
			default:
				Serial.println(F("PS_TemperatureHumidity: Invalid DHT Sensor Type"));
			}


		switch (chk)
		{
		case DHTLIB_OK:

			//Humidity
			if (m_fHumiditySensorValue == -1.0)
			{
				Serial.println("First time through Humidity)");
				m_fHumiditySensorValue = DHT.humidity;  //first time through, no filtering
			}
			else
			{
				m_fHumiditySensorValue = (m_fFilterConstant * DHT.humidity) + (1 - m_fFilterConstant) * m_fHumiditySensorValue;
			}

			//Temperature
			if (m_fTemperatureSensorValue == -1.0)
			{
				Serial.println("First time through Termperature)");
				//first time through, no filtering
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (DHT.temperature * 1.8) + 32.0;		//Scale from Celsius to Farenheit
				}
				else
				{
					m_fTemperatureSensorValue = DHT.temperature;
				}
			}
			else
			{
				if (m_In_C == false)
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * ((DHT.temperature * 1.8) + 32.0)) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}
				else
				{
					m_fTemperatureSensorValue = (m_fFilterConstant * DHT.temperature) + (1 - m_fFilterConstant) * m_fTemperatureSensorValue;
				}
				
			}


			break;
		case DHTLIB_ERROR_CHECKSUM:
			if (st::PollingSensor::debug) {
				Serial.println(F("PS_TemperatureHumidity: DHT Checksum error"));
			}
			break;
		case DHTLIB_ERROR_TIMEOUT:
			if (st::PollingSensor::debug) {
				Serial.println(F("PS_TemperatureHumidity: DHT Time out error"));
			}
			break;
		//case DHTLIB_ERROR_CONNECT:
		//	if (st::PollingSensor::debug) {
		//		Serial.println(F("PS_TemperatureHumidity: DHT Connect error"));
		//	}
		//	break;
		//case DHTLIB_ERROR_ACK_L:
		//	if (st::PollingSensor::debug) {
		//		Serial.println(F("PS_TemperatureHumidity: DHT Ack Low error"));
		//	}
		//	break;
		//case DHTLIB_ERROR_ACK_H:
		//	if (st::PollingSensor::debug) {
		//		Serial.println(F("PS_TemperatureHumidity: DHT Ack High error"));
		//	}
		//	break;
		default:
			if (st::PollingSensor::debug) {
				Serial.println(F("PS_TemperatureHumidity: DHT Unknown error"));
			}
			break;


		}
		// DISPLAY DATA
		//Serial.print(m_nHumiditySensorValue, 1);
		//Serial.print(F(",\t\t"));
		//Serial.print(m_nTemperatureSensorValue, 1);
		//Serial.println();

		Everything::sendSmartString(m_strTemperature + " " + String(m_fTemperatureSensorValue));
		Everything::sendSmartString(m_strHumidity + " " + String(m_fHumiditySensorValue));
	}
	
	void PS_TemperatureHumidity::setPin(byte pin)
	{
		m_nDigitalInputPin=pin;
	}


	//initialize static members
	dht PS_TemperatureHumidity::DHT;					//DHT library object
}