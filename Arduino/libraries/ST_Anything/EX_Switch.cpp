//******************************************************************************************
//  File: EX_Switch.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  EX_Switch is a class which implements the SmartThings "Switch" device capability.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Switch executor1("switch1", PIN_SWITCH, LOW, true);
//
//			  st::EX_Switch() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "EX_Switch.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void EX_Switch::writeStateToPin()
	{
		digitalWrite(m_nPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}
	


//public
	//constructor
	EX_Switch::EX_Switch(const __FlashStringHelper *name, byte pin, bool startingState, bool invertLogic) :
		Executor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic)
	{
		setPin(pin);
	}

	//destructor
	EX_Switch::~EX_Switch()
	{
	
	}
	
	void EX_Switch::init()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}

	void EX_Switch::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if (st::Executor::debug) {
			Serial.print(F("EX_Switch::beSmart s = "));
			Serial.println(s);
		}
		if(s==F("on"))
		{
			m_bCurrentState=HIGH;
		}
		else if(s==F("off"))
		{
			m_bCurrentState=LOW;
		}
		
		writeStateToPin();
		
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_Switch::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_Switch::setPin(byte pin)
	{
		m_nPin=pin;
		pinMode(m_nPin, OUTPUT);
		writeStateToPin();
	}
}