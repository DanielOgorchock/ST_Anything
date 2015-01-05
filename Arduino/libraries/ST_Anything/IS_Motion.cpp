//******************************************************************************************
//  File: IS_Motion.cpp
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

#include "IS_Motion.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Motion::IS_Motion(const String &name, byte pin, bool iState, bool pullup) :
		InterruptSensor(name, pin, iState, pullup)  //use parent class' constructor
		{
		}
	
	//destructor
	IS_Motion::~IS_Motion()
	{
	}
	
	void IS_Motion::init()
	{
		if (debug){
			Serial.println(F("IS_Motion: 30 second Motion Sensor Calibration Started..."));
		}
		//calibrate the PIR Motion Sensor
		digitalWrite(getInterruptPin(), LOW); 
		delay(30000);
		if (debug){
			Serial.println(F("IS_Motion: Motion Sensor Calibration Finished"));
		}

		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the motion sensor
	void IS_Motion::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" active") : F(" inactive")));
	}

	void IS_Motion::runInterrupt()
	{
		//add the "active" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" active"));
	}
	
	void IS_Motion::runInterruptEnded()
	{
		//add the "inactive" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" inactive"));
	}

}