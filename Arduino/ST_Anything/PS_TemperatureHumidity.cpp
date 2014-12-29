#include "PS_TemperatureHumidity.h"

#include "Constants.h"
#include "Everything.h"
#include <dht.h>

namespace st
{
//private
	

//public
	//constructor
	PS_TemperatureHumidity::PS_TemperatureHumidity(const String &name, unsigned int interval, int offset, byte digitalInputPin, DHT_SENSOR DHTSensorType) :
		PollingSensor(name, interval, offset),
		m_nTemperatureSensorValue(0),
		m_nHumiditySensorValue(0),
		m_bDHTSensorType(DHTSensorType)
	{
		setPin(digitalInputPin);
	}
	
	//destructor
	PS_TemperatureHumidity::~PS_TemperatureHumidity()
	{
		
	}

	const String& PS_TemperatureHumidity::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_TemperatureHumidity::beSmart set polling interval to "));
				Serial.println(s.toInt() * 1000);
			}
		}
		else {
			Serial.print(F("PS_TemperatureHumidity::beSmart cannot convert "));
			Serial.print(s);
			Serial.println(F(" to an Integer."));
		}

		return Constants::IGNORE_STRING;
	}

	const String& PS_TemperatureHumidity::init()
	{
		delay(1000);		//Needed to prevent "Unknown Error" on first read of DHT Sensor
		return getData();
	}
	
	const String& PS_TemperatureHumidity::getData()
	{
		// READ DATA
		int chk;
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
			//Serial.print(F("OK,\t"));
			m_nHumiditySensorValue = DHT.humidity;
			m_nTemperatureSensorValue = (DHT.temperature * 1.8) + 32.0;		//Scale from Celsius to Farenheit
			break;
		case DHTLIB_ERROR_CHECKSUM:
			Serial.println(F("DHT Checksum error"));
			break;
		case DHTLIB_ERROR_TIMEOUT:
			Serial.println(F("DHT Time out error"));
			break;
		default:
			Serial.println(F("DHT Unknown error"));
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