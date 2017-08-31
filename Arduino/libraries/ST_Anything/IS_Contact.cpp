//******************************************************************************************
//  File: IS_Contact.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Contact is a class which implements the SmartThings "Contact Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Contact sensor6("contact1", PIN_CONTACT, HIGH, true, 500);
//
//			  st::IS_Contact() constructor requires the following arguments
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

#include "IS_Contact.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Contact::IS_Contact(const __FlashStringHelper *name, byte pin, bool iState, bool internalPullup, long numReqCounts) :
		InterruptSensor(name, pin, iState, internalPullup, numReqCounts)  //use parent class' constructor
		{
		}
	
	//destructor
	IS_Contact::~IS_Contact()
	{
	}
	
	void IS_Contact::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void IS_Contact::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" closed") : F(" open")));
	}

	void IS_Contact::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" closed"));
	}
	
	void IS_Contact::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" open"));
	}

}