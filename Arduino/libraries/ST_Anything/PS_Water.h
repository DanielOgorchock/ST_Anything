//******************************************************************************************
//  File: PS_Water.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Water is a class which implements both the SmartThings "Water Sensor" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  presence of water using an inexpensive water sensor.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Water sensor3("water1", 60, 6, PIN_WATER, 200);
//
//			  st::PS_Water() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- int limit - OPTIONAL - the alarm limit to compare analog pin's reading to, above which the sensor reports "wet" instead of "dry", default = 100 
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
//    2015-08-23  Dan			 Added optional alarm limit to constructor
//
//
//******************************************************************************************

#ifndef ST_PS_WATER_H
#define ST_PS_WATER_H

#include "PollingSensor.h"

namespace st
{
	class PS_Water: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;		//analog pin connected to the water sensor
			int m_nSensorValue;			//current sensor value
			int m_nSensorLimit;			//alarm limit
			
		public:
			//constructor - called in your sketch's global variable declaration section
			PS_Water(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, int limit = 100);
			
			//destructor
			virtual ~PS_Water();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			void setPin(byte pin);
	
	};
}



#endif