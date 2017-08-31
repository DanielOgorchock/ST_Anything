//******************************************************************************************
//  File: IS_CarbonMonoxide.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Smoke is a class which implements the SmartThings "Carbon Monoxide Detector" device capability.
//			  It inherits from the st::IS_Smoke class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_CarbonMonoxide sensor6("carbonMonoxide1", PIN_CO, HIGH, false, 500);
//
//			  st::IS_CarbonMonoxide() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital input
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - OPTIONAL - true == INTERNAL_PULLUP
//				- long numReqCounts - OPTIONAL - number of counts before changing state of input (prevent false alarms)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-04-19  Dan & Daniel   Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#ifndef ST_IS_CARBONMONOXIDE_H
#define ST_IS_CARBONMONOXIDE_H

#include "InterruptSensor.h"

namespace st
{
	class IS_CarbonMonoxide: public InterruptSensor
	{
		private:
			//inherits everything necessary from parent IS_Smoke Class
			
		public:
			//constructor - called in your sketch's global variable declaration section
			IS_CarbonMonoxide(const __FlashStringHelper *name, byte pin, bool iState, bool internalPullup = false, long numReqCounts = 0); //(defaults to NOT using internal pullup resistors, and required counts = 0)
			
			//destructor
			virtual ~IS_CarbonMonoxide();

			//initialization function
			virtual void init();

			//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the sensor
			virtual void refresh();

			//handles what to do when interrupt is triggered 
			virtual void runInterrupt();

			//handles what to do when interrupt is ended 
			virtual void runInterruptEnded();

	};
}


#endif