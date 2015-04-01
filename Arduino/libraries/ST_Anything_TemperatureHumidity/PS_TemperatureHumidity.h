//******************************************************************************************
//  File: PS_TemperatureHumidity.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_TemperatureHumidity is a class which implements both the SmartThings "Temperature Measurement" 
//			  and "Relative Humidity Measurement" device capabilities.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  temperature and humidity from a DHT series sensor.  This was tested with both the DHT11 and DHT22.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_TemperatureHumidity sensor2("temphumid", 120000, 3000, PIN_TEMPERATUREHUMIDITY, st::PS_TemperatureHumidity::DHT22);
//
//			  st::PS_TemperatureHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- DHT_SENSOR DHTSensorType - REQUIRED - the type of DHT sensor (DHT11, DHT21, DHT22, DHT33, or DHT44)
//				- String strTemp - OPTIONAL - name of temperature sensor to send to ST Cloud (defaults to "temperature")
//				- String strHumid - OPTIONAL - name of humidity sensor to send to ST Cloud (defaults to "humidity")
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
//
//
//******************************************************************************************

#ifndef ST_PS_TEMPERATUREHUMIDITY_H
#define ST_PS_TEMPERATUREHUMIDITY_H

#include "PollingSensor.h"
#include <dht.h>

namespace st
{
	class PS_TemperatureHumidity: public PollingSensor
	{
		private:
			byte m_nDigitalInputPin;		//digital pin connected to the DHT sensor
			int m_nTemperatureSensorValue;	//current Temperature value
			int m_nHumiditySensorValue;		//current Humidity Value
			static dht DHT;					//DHT library object
			byte m_bDHTSensorType;			//DHT Sensor Type
			String m_strTemperature;			//name of temparature sensor to use when transferring data to ST Cloud
			String m_strHumidity;			//name of temparature sensor to use when transferring data to ST Cloud		

		public:
			//types of DHT sensors supported by the dht library
			enum DHT_SENSOR { DHT11, DHT21, DHT22, DHT33, DHT44 };

			//constructor - called in your sketch's global variable declaration section
			PS_TemperatureHumidity(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalInputPin, DHT_SENSOR DHTSensorType, String strTemp="temperature", String strHumid="humidity");
			
			//destructor
			virtual ~PS_TemperatureHumidity();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
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
