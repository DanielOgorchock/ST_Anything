//******************************************************************************************
//  File: IS_Motion.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Motion is a class which implements the SmartThings "Motion Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Motion sensor5("motion", PIN_MOTION, HIGH);
//
//			  st::IS_Motion() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool iState - OPTIONAL - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - OPTIONAL - true == INTERNAL_PULLUP
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#ifndef ST_IS_MOTION_H
#define ST_IS_MOTION_H

#include "InterruptSensor.h"

namespace st
{
	class IS_Motion: public InterruptSensor
	{
		private:
			//inherits everything necessary from parent InterruptSensor Class
			
			unsigned int timer; 
			bool calibrated;
			
		public:
			//constructor - called in your sketch's global variable declaration section
			IS_Motion(const __FlashStringHelper *name, byte pin, bool iState, bool internalPullup = false); //(defaults to NOT using internal pullup resistors)
			
			//destructor
			virtual ~IS_Motion();
			
			//initialization function
			virtual void init();

			//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the motion sensor
			virtual void refresh();

			//handles what to do when interrupt is triggered 
			virtual void runInterrupt();

			//handles what to do when interrupt is ended 
			virtual void runInterruptEnded();
	
			//override update method
			virtual void update();
	
	};
}


#endif