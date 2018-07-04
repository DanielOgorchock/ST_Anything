//******************************************************************************************
//  File: PS_MAX44009_Illuminance.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_MAX44009_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the Illuminace from a MAX44009 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_MAX44009_Illuminance sensor1(F("illuminance1"), 60, 0);  (simple, uses defaults)
//			  For Example:  st::PS_MAX44009_Illuminance sensor1(F("illuminance1"), 60, 0, MAX44009_A0_LOW); (full user control of settings)
//
//			  st::PS_MAX44009_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//              - uint8_t addr -OPTIONAL - defaults to MAX44009_A0_LOW
//
//
//              I2C address options
//                MAX44009_A0_LOW             0x4A      //< Pin A0 pulled Low
//                MAX44009_A0_HIGH            0x4B      //< Pin A0 pulled Hi
//
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
//    2018-07-03  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_PS_MAX44009_Illuminance_H
#define ST_PS_MAX44009_Illuminance_H

#include "PollingSensor.h"
#include "Max44009.h"

namespace st
{
	class PS_MAX44009_Illuminance: public PollingSensor
	{
		private:
			Max44009 myLux;         //MAX44009 object
			float m_fLux;		    //lux

		public:
			//constructor - called in your sketch's global variable declaration section
			PS_MAX44009_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, uint8_t addr = MAX44009_A0_LOW);
			
			//destructor
			virtual ~PS_MAX44009_Illuminance();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();
			
			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
				
			//sets
			
	};
}
#endif
