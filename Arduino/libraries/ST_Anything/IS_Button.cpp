//******************************************************************************************
//  File: IS_Button.cpp
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

#include "IS_Button.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Button::IS_Button(const __FlashStringHelper *name, byte pin, long reqNumMillisHeld, bool iState, bool internalPullup, long numReqCounts) :
		InterruptSensor(name, pin, iState, internalPullup, numReqCounts),  //use parent class' constructor
		m_lreqNumMillisHeld(reqNumMillisHeld)
		{
		}
	
	//destructor
	IS_Button::~IS_Button()
	{
	}
	
	void IS_Button::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
		//m_lTimeBtnPressed = 0;
	}

	//called periodically by Everything class to ensure ST Cloud is kept up yo date. HOWEVER, not useful for the IS_Button.
	void IS_Button::refresh()
	{
		//Do Nothing since sending extraneous button presses would cause bad behavior in ST.
	}

	void IS_Button::runInterrupt()
	{
		//Capture time of button down event so we can figure it whether to send "pushed" or "held" on button release
		m_lTimeBtnPressed = millis();

		//Serial.println("IS_Button: in runInterrupt()");
	}
	
	void IS_Button::runInterruptEnded()
	{
		//Serial.println("IS_Button: in runInterruptEnded()");
		//Serial.println(millis());
		//Serial.println(m_lTimeBtnPressed);
		//Serial.println(m_lreqNumMillisHeld);

		if (!m_bFirstRun)  //Prevent sending data to SmartThings during initial startup
		{
			if (millis() < (m_lTimeBtnPressed + m_lreqNumMillisHeld))
			{
				//add the "pushed" event to the buffer to be queued for transfer to SmartThings
				Everything::sendSmartString(getName() + F(" pushed"));
			}
			else if (millis() >= (m_lTimeBtnPressed + m_lreqNumMillisHeld))
			{
				//add the "held" event to the buffer to be queued for transfer to SmartThings
				Everything::sendSmartString(getName() + F(" held"));
			}
		}
		else
		{
			m_bFirstRun = false;
		}

	}

}