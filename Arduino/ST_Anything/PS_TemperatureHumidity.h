#ifndef ST_PS_TEMPERATUREHUMIDITY_H
#define ST_PS_TEMPERATUREHUMIDITY_H

#include "PollingSensor.h"
#include <dht.h>

namespace st
{
	class PS_TemperatureHumidity: public PollingSensor
	{
		private:
			byte m_nDigitalInputPin;
			int m_nTemperatureSensorValue;
			int m_nHumiditySensorValue;
			dht DHT;
			byte m_bDHTSensorType;
			
		public:
			enum DHT_SENSOR { DHT11, DHT21, DHT22, DHT33, DHT44 };
			//constructor
			PS_TemperatureHumidity(const String &name, unsigned int interval, int offset, byte digitalInputPin, DHT_SENSOR DHTSensorType);
			
			//destructor
			virtual ~PS_TemperatureHumidity();

			virtual void beSmart(const String &str);
			
			virtual void init();
			virtual void getData();
			
			//gets
			inline byte getPin() const { return m_nDigitalInputPin; }
			inline byte getTemperatureSensorValue() const { return m_nTemperatureSensorValue; }
			inline byte getHumiditySensorValue() const { return m_nHumiditySensorValue; }
				
			//sets
			void setPin(byte pin);
	
	};
}



#endif