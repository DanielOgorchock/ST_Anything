//******************************************************************************************
//  File: IS_Smoke.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Smoke is a class which implements the SmartThings "Smoke Detector" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Smoke sensor6("smoke1", PIN_SMOKE, HIGH, false, 500);
//
//			  st::IS_Smoke() constructor requires the following arguments
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
//    2015-01-03  Dan & Daniel   Original Creation
//	  2015-03-17  Dan Ogorchock  Added optional "numReqCounts" constructor argument/capability
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "IS_Smoke.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Smoke::IS_Smoke(const __FlashStringHelper *name, byte pin, bool iState, bool pullup, long numReqCounts) :
		InterruptSensor(name, pin, iState, pullup, numReqCounts)  //use parent class' constructor
		{
		}
	
	//destructor
	IS_Smoke::~IS_Smoke()
	{
	}
	
	void IS_Smoke::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void IS_Smoke::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" clear") : F(" detected")));
	}

	void IS_Smoke::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" clear"));
	}
	
	void IS_Smoke::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" detected"));
	}

}