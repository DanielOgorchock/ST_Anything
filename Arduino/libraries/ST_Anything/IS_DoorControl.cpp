//******************************************************************************************
//  File: IS_DoorControl.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_DoorControl is a class which implements the SmartThings "Door Control" device capability.  It features
//			  an automatic-turn-off time delay for a relay to actuate a button press.  This is useful for controlling
//			  a garage door.  Use the input to monitor a magnetic door contact sensor.  Use the output to control a relay to
//			  "press the garage door button" to open/close the garage door.
//
//			  It inherits from the st::InterruptSensor class and clones much from the st::Executor Class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_DoorControl sensor3("doorControl1", PIN_CONTACT_DOOR_1, LOW, true, PIN_RELAY_DOOR_1, LOW, true, 1000);
//
//			  st::IS_DoorControl() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinInput - REQUIRED - the Arduino Pin to be used as a digital input
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - REQUIRED - true == INTERNAL_PULLUP
//				- byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Output should use inverted logic
//				- long delayTime - REQUIRED - the number of milliseconds to keep the output on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-07  Dan Ogorchock  Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "IS_DoorControl.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void IS_DoorControl::writeStateToPin()
	{
		digitalWrite(m_nOutputPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

//public
	//constructor
	IS_DoorControl::IS_DoorControl(const __FlashStringHelper *name, byte pinInput, bool iState, bool pullup, byte pinOutput, bool startingState, bool invertLogic, unsigned long delayTime) :
		InterruptSensor(name, pinInput, iState, pullup),  //use parent class' constructor
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic),
		m_lDelayTime(delayTime),
		m_lTimeTurnedOn(0),
		m_bTimerPending(false)
		{
			setOutputPin(pinOutput);
		}
	
	//destructor
	IS_DoorControl::~IS_DoorControl()
	{
	}
	
	void IS_DoorControl::init()
	{
		//get current status of contact sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//update function 
	void IS_DoorControl::update()
	{
		//Turn off digital output if timer has expired
		if ((m_bCurrentState == HIGH) && (millis() - m_lTimeTurnedOn >= m_lDelayTime))
		{	
			m_bCurrentState = LOW;
			writeStateToPin();
			//Decrement number of active timers
			if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
			m_bTimerPending = false;
		}

		//check to see if input pin has changed state
		InterruptSensor::update();
	}

	void IS_DoorControl::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::InterruptSensor::debug) {
			Serial.print(F("IS_ContactRelay::beSmart s = "));
			Serial.println(s);
		}
		if (s == F("on"))
		{
			m_bCurrentState = HIGH;

			//Save time turned on
			m_lTimeTurnedOn = millis();

			//Increment number of active timers
			if (!m_bTimerPending)
			{
				st::Everything::bTimersPending++;
				m_bTimerPending = true;
			}
			//Queue the door status update the ST Cloud 
			Everything::sendSmartStringNow(getName() + (getStatus() ? F(" opening") : F(" closing")));
		}
		else if (s == F("off"))
		{
			m_bCurrentState = LOW;

			//Decrement number of active timers
			if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
			m_bTimerPending = false;
		}
		
		//update the digital output
		writeStateToPin();
	}


	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void IS_DoorControl::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" closed") : F(" open")));
	}

	void IS_DoorControl::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" closed"));
	}
	
	void IS_DoorControl::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" open"));
	}

	void IS_DoorControl::setOutputPin(byte pin)
	{
		m_nOutputPin = pin;
		pinMode(m_nOutputPin, OUTPUT);
		writeStateToPin();
	}

}