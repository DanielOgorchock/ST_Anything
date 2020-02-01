//******************************************************************************************
//  File: IS_Motion.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_Motion is a class which implements the SmartThings "Motion Sensor" device capability.
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_Motion sensor5(F("motion1"), PIN_MOTION, HIGH, false, 500);
//
//			  st::IS_Motion() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool iState - OPTIONAL - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - OPTIONAL - true == INTERNAL_PULLUP
//				- long numReqCounts - OPTIONAL - number of counts before changing state of input (prevent false alarms)
//              - long inactiveTimeout - OPTIONAL - number of milliseconds motion must be inactive before sending update to hub (default = 0)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//	  2016-09-03  Dan Ogorchock  Added optional "numReqCounts" constructor argument/capability
//    2017-01-25  Dan Ogorchock  Corrected issue with INPUT_PULLUP per request of Jiri Culik
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2020-01-31  Dan Ogorchock  Added optional inactivity timeout
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
	IS_Motion::IS_Motion(const __FlashStringHelper *name, byte pin, bool iState, bool pullup, long numReqCounts, long inactiveTimeout) :
		InterruptSensor(name, pin, iState, pullup, numReqCounts),  //use parent class' constructor
		calibrated(false),
		m_inactiveTimeout(inactiveTimeout),
		m_inactiveTimerRunning(false)
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
		timer=millis();
		//delay(30000);
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the motion sensor
	void IS_Motion::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" active") : F(" inactive")));
	}

	void IS_Motion::runInterrupt()
	{
		if (m_inactiveTimerRunning == false) {
			//add the "active" event to the buffer to be queued for transfer to the ST Shield
			Everything::sendSmartString(getName() + F(" active"));
		}
		//cancel any inactivity timer that may be running
		m_inactiveTimerRunning = false;

	}
	
	void IS_Motion::runInterruptEnded()
	{
		//start inactivity timer 
		m_inactiveTimer = millis();
		m_inactiveTimerRunning = true;
	}
	
	void IS_Motion::update()
	{
		if (calibrated) {
			InterruptSensor::update();

			if ((m_inactiveTimerRunning == true) && (millis() > m_inactiveTimer + m_inactiveTimeout)) {
				m_inactiveTimerRunning = false;
				//add the "inactive" event to the buffer to be queued for transfer to the ST Shield
				Everything::sendSmartString(getName() + F(" inactive"));
			}
		}
		else
			if(millis()>timer+30000)
			{
				calibrated=true;
				
				//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
				setInterruptPin(getInterruptPin());
				InterruptSensor::init();
				
				if (debug)
				{
					Serial.println(F("IS_Motion: Motion Sensor Calibration Finished"));
				}
			}
	}

}
