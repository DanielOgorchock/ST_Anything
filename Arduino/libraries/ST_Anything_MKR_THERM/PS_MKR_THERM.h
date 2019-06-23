//******************************************************************************************
//  File: PS_MKR_THERM.h
//  Authors: Dan G Ogorchock 
//
//  Summary:  PS_MKR_THERM is a class which implements the SmartThings "Temperature Measurement" 
//			  device capability.
//			  It inherits from the st::PollingSensor class.  This class implements support for 
//            Arduino MKR THERM Thermocouple Shield.    
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_MKR_THERM sensor1(F("temperature1"), 120, 3);
//
//			  st::PS_TemperatureHumidity() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
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
//    2019-06-23  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_PS_MKR_THERM_H
#define ST_PS_MKR_THERM_H

#include "PollingSensor.h"

#include <Arduino_MKRTHERM.h>

namespace st
{
	class PS_MKR_THERM: public PollingSensor
	{
		private:
			double m_dblTemperatureSensorValue;		//current Temperature value

		public:

			//constructor - called in your sketch's global variable declaration section
			PS_MKR_THERM(const __FlashStringHelper *name, unsigned int interval, int offset);
			
			//destructor
			virtual ~PS_MKR_THERM();

			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline int getTemperatureSensorValue() const { return int(m_dblTemperatureSensorValue); }
				
			//sets
	
	};
}



#endif