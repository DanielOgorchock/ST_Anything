//******************************************************************************************
//  File: PS_Power.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Power is a class which implements the SmartThings "Power Meter" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an analog input pin via the EmonLib.  This produce the Irms current of the Current Transformer.
//            The Irms current is then multiplied by the voltage constant passed in to produce Power in Watts.
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Power sensor1(F("power1"), 120, 0, PIN_POWER, 30.0, 1480, 120.0);
//
//			  st::PS_Power() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//              - double ICAL - REQUIRED - EmonLib Calibration Constant
//				- unsigned int numSamples - OPTIONAL - defaults to 1480, number of analog readings to use for calculating the Irms Current
//              - float voltage - OPTIONAL - defaults to 120, AC voltage of the mains line being monitored
//				- byte filterConstant - OPTIONAL - Value from 5% to 100% to determine how much filtering/averaging is performed 100 = none (default), 5 = maximum
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-02-17  Dan Ogorchock  Original Creation
//    2019-09-19  Dan Ogorchock  Added filtering optional argument to help reduce noisy signals
//
//
//******************************************************************************************
#ifndef ST_PS_Power_H
#define ST_PS_Power_H

#include <EmonLib.h>
#include "PollingSensor.h"

namespace st
{
	class PS_Power: public PollingSensor
	{
		private:
			EnergyMonitor emon1;
			byte m_nAnalogInputPin;
			double m_fICAL;
			unsigned int m_nNumSamples;
			float m_fVoltage;
			double m_fIrms; 
			double m_fApparentPower;
			float m_fFilterConstant;        //Filter constant % as floating point from 0.00 to 1.00

		public:
			//constructor - called in your sketch's global variable declaration section
			PS_Power(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double ICAL, unsigned int NumSamples=1480, float voltage=120.0, byte filterConstant = 100);
			
			//destructor
			virtual ~PS_Power();
			
			//initialization function
			virtual void init();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline float getSensorValue() const {return m_fApparentPower;}
				
			//sets
			void setPin(byte pin);
	};
}
#endif
