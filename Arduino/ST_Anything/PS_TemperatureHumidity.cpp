#include "PS_TemperatureHumidity.h"

#include "Constants.h"
#include "Everything.h"
#include <dht.h>

namespace st
{
//private
	

//public
	//constructor
	PS_TemperatureHumidity::PS_TemperatureHumidity(const String &name, unsigned int interval, int offset, byte digitalInputPin) :
		PollingSensor(name, interval, offset),
		m_nTemperatureSensorValue(0),
		m_nHumiditySensorValue(0)	
	{
		setPin(digitalInputPin);
	}
	
	//destructor
	PS_TemperatureHumidity::~PS_TemperatureHumidity()
	{
		
	}

	const String& PS_TemperatureHumidity::init()
	{
		return Constants::IGNORE_STRING;
	}
	
	const String& PS_TemperatureHumidity::getData()
	{
		// READ DATA

		//Serial.print(F("DHT22, \t"));
		int chk = DHT.read22(m_nDigitalInputPin);		//DHT22 Sensor
		//int chk = DHT.read11(m_nDigitalInputPin);		//DHT11 Sensor

		switch (chk)
		{
		case DHTLIB_OK:
			//Serial.print(F("OK,\t"));
			m_nHumiditySensorValue = DHT.humidity;
			m_nTemperatureSensorValue = (DHT.temperature * 1.8) + 32.0;		//Scale from Celsius to Farenheit
			break;
		case DHTLIB_ERROR_CHECKSUM:
			Serial.print(F("Checksum error,\t"));
			break;
		case DHTLIB_ERROR_TIMEOUT:
			Serial.print(F("Time out error,\t"));
			break;
		default:
			Serial.print(F("Unknown error,\t"));
			break;
		}
		// DISPLAY DATA
		//Serial.print(m_nHumiditySensorValue, 1);
		//Serial.print(F(",\t\t"));
		//Serial.print(m_nTemperatureSensorValue, 1);
		//Serial.println();
		
		Everything::Return_String.remove(0);
		Everything::Return_String += "temperature " + String(m_nTemperatureSensorValue) + "|humidity " + String(m_nHumiditySensorValue);
		return Everything::Return_String;
	}
	
	void PS_TemperatureHumidity::setPin(byte pin)
	{
		m_nDigitalInputPin=pin;
	}
}