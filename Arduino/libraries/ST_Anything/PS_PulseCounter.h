//******************************************************************************************
//  File: PS_PulseCounter.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_PulseCounter is a class which implements the SmartThings "Power Meter"-style device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  the number of counts between polling intervals.  At the polling interval, the pulse count is converted
//			  to engineering units via a linear conversion (engUnits = slope x counts + offset).
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_PulseCounter sensor3(F("power1"), 60, 5, PIN_PULSE, FALLING, INPUT_PULLUP, 1.0, 0);
//
//			  st::PS_PulseCounter() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must be in form of "power1", "power2", etc...
//				- int interval - REQUIRED - the polling interval in seconds
//				- int offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the GPIO Pin to be used as an digital input (Hardware Interrupt)
//				- byte inttype - REQUIRED - which type of Arduino interrupt to trigger the ISR (RISING, FALLING, CHANGE)
//				- byte inputmode - REQUIRED - Mode of the digital input Pin (INPUT, INPUT_PULLUP)
//				- float cnvslope - REQUIRED - Conversion to Engineering Units Slope
//				- float cnvoffset - REQUIRED - Conversion to Engineering Units Offset
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
//    2015-03-31  Dan Ogorchock  Original Creation
//    2018-03-03  Dan Ogorchock  Improved code to make generic for all boards, not just Arduino MEGA 
//
//
//******************************************************************************************


#ifndef ST_PS_PULSECOUNTER_H
#define ST_PS_PULSECOUNTER_H

#include "PollingSensor.h"

namespace st
{
	class PS_PulseCounter : public PollingSensor
	{
		private:
			byte m_nInputPin;				  //input pin connected to the pulse generator
			byte m_nInputMode;                //input mode (INPUT or INPUT_PULLUP)
			unsigned long m_nSensorValue;	  //current sensor value (m_nSensorValue = Long(m_fCnvSlope * m_nCounts + m_fCnvOffset))
			float m_fCnvSlope;				  //Linear Conversion Slope
			float m_fCnvOffset;				  //Linear Conversion Offset
			volatile unsigned long* m_pCounter;		  //Point to the correct Counter variable;

		public:


			//constructor - called in your sketch's global variable declaration section
			PS_PulseCounter(const __FlashStringHelper *name, unsigned int interval, int offset, byte inputpin, byte inttype, byte inputmode, float cnvslope, float cnvoffset);
			
			//destructor
			virtual ~PS_PulseCounter();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getPin() const {return m_nInputPin;}
			inline long getSensorValue() const {return m_nSensorValue;}

			//sets
			void setPin(byte pin);

			friend void isrPulse();

	};
}



#endif