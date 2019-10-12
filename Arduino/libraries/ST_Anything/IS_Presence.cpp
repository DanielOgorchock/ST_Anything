//******************************************************************************************
//  File: IS_Presence.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Presence is a class which implements the SmartThings "Presence Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Presence sensor1(F("presence1"), PIN_PRESENCE, LOW, true, 5000);
//
//			  st::IS_Presence() constructor requires the following arguments
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
//    2019-10-12  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#include "IS_Presence.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Presence::IS_Presence(const __FlashStringHelper *name, byte pin, bool iState, bool internalPullup, long numReqCounts) :
		InterruptSensor(name, pin, iState, internalPullup, numReqCounts)  //use parent class' constructor
		{
		}
	
	//destructor
	IS_Presence::~IS_Presence()
	{
	}
	
	void IS_Presence::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the presence sensor
	void IS_Presence::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" present") : F(" notpresent")));
	}

	void IS_Presence::runInterrupt()
	{
		//add the "present" event to the buffer to be queued for transfer to Hubitat/SmartThings
		Everything::sendSmartString(getName() + F(" present"));
	}
	
	void IS_Presence::runInterruptEnded()
	{
		//add the "notpresent" event to the buffer to be queued for transfer to Hubitat/SmartThings
		Everything::sendSmartString(getName() + F(" notpresent"));
	}

}
