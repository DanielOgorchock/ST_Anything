//******************************************************************************************
//  File: PS_Ultrasonic.cpp
//  Authors: 
//
//  Summary:  PS_Ultrasonic is a class which implements a custom Level device capability.
//			  It inherits from the st::PollingSensor class.  
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Ultrasonic sensor1("ultrasonic1", 60, 0, PIN_ULTRASONIC_T, PIN_ULTRASONIC_E);
//
//			  st::PS_Ultrasonic() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte digitalTriggerPin - REQUIRED - the Arduino Pin to be used as a digital output to trigger ultrasonic
//				- byte digitalEchoPin - REQUIRED - the Arduino Pin to be used as a digital input to read the echo
//
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    
//
//
//******************************************************************************************

#ifndef ST_PS_Ultrasonic_H
#define ST_PS_Ultrasonic_H

#include "PollingSensor.h"

namespace st
{
	class PS_Ultrasonic : public PollingSensor
	{
		private:
			byte m_nDigitalTriggerPin;
			byte m_nDigitalEchoPin;
			float m_nSensorValue;
			
		public:
			//constructor - called in your sketch's global variable declaration section
			PS_Ultrasonic(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalTriggerPin, byte digitalEchoPin);
			
			//destructor
			virtual ~PS_Ultrasonic();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			//inline byte getPin() const {return m_nAnalogInputPin;}
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			void setPin(byte &trigPin,byte &echoPin);
	};
}
#endif
