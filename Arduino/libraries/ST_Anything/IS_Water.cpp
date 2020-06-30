//******************************************************************************************
//  File: IS_Water.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Water is a class which implements the Hubitat "Water Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Water sensor6(F("water1"), PIN_WATER, HIGH, true, 500);
//
//			  st::IS_Water() constructor requires the following arguments
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
//    2020-06-30  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************


#include "IS_Water.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Water::IS_Water(const __FlashStringHelper *name, byte pin, bool iState, bool internalPullup, long numReqCounts) :
		InterruptSensor(name, pin, iState, internalPullup, numReqCounts)  //use parent class' constructor
		{
		}
	
	//destructor
	IS_Water::~IS_Water()
	{
	}
	
	void IS_Water::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//called periodically by Everything class to ensure the Hub is kept consistent with the state of the water sensor
	void IS_Water::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" wet") : F(" dry")));
	}

	void IS_Water::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the hub
		Everything::sendSmartString(getName() + F(" wet"));
	}
	
	void IS_Water::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the hub
		Everything::sendSmartString(getName() + F(" dry"));
	}

}
