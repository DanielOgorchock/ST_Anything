//******************************************************************************************
//  File: IS_ContactRelay.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_ContactRelay is a class which implements the SmartThings "Contact Sensor" device capability as well
//			  as a "Switch" device capability with a an automatic-turn-off time delay.  This is useful for controlling
//			  a garage door.  Use the input to mintor a magnetic contact sensor.  Use the output to control a relay to
//			  "press the garage door button" to open or close the garage door.
//
//			  It inherits from the st::InterruptSensor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_ContactRelay sensor6("contact", PIN_CONTACT, HIGH, true);
//
//			  st::IS_ContactRelay() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinInput - REQUIRED - the Arduino Pin to be used as a digital input
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - REQUIRED - true == INTERNAL_PULLUP
//				- byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Ouput should use inverted logic
//				- long delayTime - REQUIRED - the number of milliseconds to keep the output on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-07  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#include "IS_ContactRelay.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void IS_ContactRelay::writeStateToPin()
	{
		digitalWrite(m_nOutputPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

//public
	//constructor
	IS_ContactRelay::IS_ContactRelay(const String &name, byte pinInput, bool iState, bool pullup, byte pinOutput, bool startingState, bool invertLogic, unsigned long delayTime) :
		InterruptSensor(name, pinInput, iState, pullup),  //use parent class' constructor
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic),
		m_lDelayTime(delayTime),
		m_lTimeTurnedOn(0)
		{
			setOutputPin(pinOutput);
		}
	
	//destructor
	IS_ContactRelay::~IS_ContactRelay()
	{
	}
	
	void IS_ContactRelay::init()
	{
		//get current status of motion sensor by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
	}

	//update function 
	void IS_ContactRelay::update()
	{
		//Turn off digital output if timer has expired
		if ((m_bCurrentState == HIGH) && (millis() - m_lTimeTurnedOn >= m_lDelayTime))
		{	
			m_bCurrentState = LOW;
			writeStateToPin();
		}

		//check to see if input pin has changed state
		InterruptSensor::update();
	}

	void IS_ContactRelay::beSmart(const String &str)
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
		}
		else if (s == F("off"))
		{
			m_bCurrentState = LOW;
		}

		writeStateToPin();

		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}


	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void IS_ContactRelay::refresh()
	{
		Everything::sendSmartString(getName() + (getStatus() ? F(" closed") : F(" open")));
	}

	void IS_ContactRelay::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" closed"));
	}
	
	void IS_ContactRelay::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" open"));
	}

	void IS_ContactRelay::setOutputPin(byte pin)
	{
		m_nOutputPin = pin;
		pinMode(m_nOutputPin, OUTPUT);
		writeStateToPin();
	}

}