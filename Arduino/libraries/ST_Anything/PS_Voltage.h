//******************************************************************************************
//  File: PS_Voltage.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Voltage is a class which implements the SmartThings "Voltage Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an anlog input pin and then scale it to engineering units.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Voltage sensor1("voltage1", 120, 0, PIN_VOLTAGE, 0, 1023, 0, 5);
//
//			  st::PS_Voltage() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- double s_l - OPTIONAL - first argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double s_h - OPTIONAL - second argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double m_l - OPTIONAL - third argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
//				- double m_h - OPTIONAL - fourth argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output
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
//    2015-04-19  Dan & Daniel   Original Creation
//    2017-08-18  Dan Ogorchock  Modified to return floating point values instead of integer
//
//
//******************************************************************************************

#ifndef ST_PS_VOLTAGE_H
#define ST_PS_VOLTAGE_H

#include "PollingSensor.h"

namespace st
{
	class PS_Voltage: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;
			float m_fSensorValue;
			const int SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH;
			
		public:
			//constructor - called in your sketch's global variable declaration section
			PS_Voltage(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double s_l=0, double s_h=1023, double m_l=0, double m_h=5000);
			
			//destructor
			virtual ~PS_Voltage();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline float getSensorValue() const {return m_fSensorValue;}
				
			//sets
			void setPin(byte pin);
	};
}
#endif
