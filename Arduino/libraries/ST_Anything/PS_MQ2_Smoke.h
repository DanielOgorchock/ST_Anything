//******************************************************************************************
//  File: PS_MQ2_Smoke.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_MQ2_Smoke is a class which implements the SmartThings "Smoke Detector" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  value from the MQ2 sensor.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_MQ2_Smoke sensor1("smoke1", 10, 0, PIN_MQ2_SMOKE, 300);
//
//			  st::PS_MQ2_Smoke() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name e.g. "smoke1", "smoke2", etc...
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- int sensorLimit - REQUIRED - if sensor reads value above this limit, report Smoke Detected (typical range is 0 to 1023 on Arduino Analog Input)
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to 
//			  the Arduino.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2017-07-04  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_PS_MQ2_SMOKE_H
#define ST_PS_MQ2_SMOKE_H

#include "PollingSensor.h"

namespace st
{
	class PS_MQ2_Smoke: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;
			int m_nSensorValue;
			int m_nSensorLimit;
			
		public:
			//constructor - called in your sketch's global variable declaration section
			PS_MQ2_Smoke(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, int sensorLimit);
			
			//destructor
			virtual ~PS_MQ2_Smoke();
			
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