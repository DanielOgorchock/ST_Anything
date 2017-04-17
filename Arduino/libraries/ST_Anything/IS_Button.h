//******************************************************************************************
//  File: IS_Button.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Button is a class which implements the SmartThings "Contact Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Button sensor("button1", PIN_BUTTON1, 1000, LOW, true, 500);
//
//			  st::IS_Button() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - should be "button1", "button2", "button3", etc...
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital input
//				- long reqNumMillisHeld - Required - number of milliseconds required needed to detect a "Held" condition versus a "Pushed" (default = 1000)
//				- bool iState - OPTIONAL - LOW or HIGH - determines which value indicates the interrupt is true (default = HIGH)
//				- bool internalPullup - OPTIONAL - true == INTERNAL_PULLUP (default - true)
//				- long numReqCounts - OPTIONAL - number of counts before changing state of input (prevent false triggers) (default = 500)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2017-03-25  Dan            Original Creation
//
//
//******************************************************************************************

#ifndef ST_IS_BUTTON_H
#define ST_IS_BUTTON_H

#include "InterruptSensor.h"

namespace st
{
	class IS_Button: public InterruptSensor
	{
		private:
			unsigned long m_lTimeBtnPressed;	    //time when the digital input went high
			unsigned long m_lreqNumMillisHeld;  	//amount of time required to trigger "held" instead of "pushed"
			bool m_bFirstRun = true;				//used to prevent sending inadvertent button pushed/held message on startup

		public:
			//constructor - called in your sketch's global variable declaration section
			IS_Button(const __FlashStringHelper *name, byte pin, long reqNumMillisHeld = 1000, bool iState = LOW, bool internalPullup = true, long numReqCounts = 500); //(defaults to using internal pullup resistors)
			
			//destructor
			virtual ~IS_Button();
			
			//initialization function
			virtual void init();

			//called periodically by Everything class to ensure ST Cloud is kept up to date.  Not used for IS_Button.
			virtual void refresh();

			//handles what to do when interrupt is triggered 
			virtual void runInterrupt();

			//handles what to do when interrupt is ended 
			virtual void runInterruptEnded();
	
	};
}


#endif