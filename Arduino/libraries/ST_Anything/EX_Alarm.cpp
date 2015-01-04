//******************************************************************************************
//  File: EX_Alarm.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  EX_Alarm is a class which implements the SmartThings "Alarm" device capability.
//			  It inherits from the st::Executor class.  The current version implements the Siren as
//			  a digital output (uses a relay to switch the Siren on or off)
//
//			  TODO:  Possibly add Strobe capability in the future
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Alarm executor2("alarm", PIN_ALARM, LOW, true);
//
//			  st::EX_Alarm() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Ouput should use inverted logic
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#include "EX_Alarm.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//private
	void EX_Alarm::writeStateToPin()
	{
		digitalWrite(m_nPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

	//public
	//constructor
	EX_Alarm::EX_Alarm(const String &name, byte pin, bool startingState, bool invertLogic) :
		Executor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic)
	{
		setPin(pin);
	}

	//destructor
	EX_Alarm::~EX_Alarm()
	{

	}

	//initialization routine
	void EX_Alarm::init()
	{
		refresh();	//update ST Cloud with initial data
	}

	//called periodically to ensure state of the Alarm Siren is up to date in the SmartThings Cloud (in case an event is missed)
	void EX_Alarm::refresh()
	{
		Everything::sendSmartString(getName() + (m_bCurrentState == HIGH ? F(" siren") : F(" off")));
	}

	//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
	void EX_Alarm::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (debug) {
			Serial.print(F("EX_Alarm::beSmart s = "));
			Serial.println(s);
		}
		if ((s == F("both")) || (s == F("siren")))
		{
			m_bCurrentState = HIGH;
		}
		else if (s == F("off"))
		{
			m_bCurrentState = LOW;
		}

		writeStateToPin();

		refresh();
	}

	void EX_Alarm::setPin(byte pin)
	{
		m_nPin = pin;
		pinMode(m_nPin, OUTPUT);
		writeStateToPin();
	}
}