//******************************************************************************************
//  File: PS_Alarm.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Alarm is a class which implements the SmartThings "Alarm" device capability.
//			  It inherits from the st::PollingSensor class.  The current version implements the Siren as
//			  a digital output (uses a relay to switch the Siren on or off)
//
//			  TODO:  Should probably be a st::Executor
//			  TODO:  Possibly add Strobe capability in the future
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Alarm sensor4("alarm", 60000, 9000, PIN_ALARM, LOW, true);
//
//			  st::PS_Alarm() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in milliseconds
//				- long offset - REQUIRED - the polling interval offset in milliseconds - used to prevent all polling sensors from executing at the same time
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

#include "PS_Alarm.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//private
	void PS_Alarm::writeStateToPin()
	{
		digitalWrite(m_nPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

	//public
	//constructor
	PS_Alarm::PS_Alarm(const String &name, long interval, long offset, byte pin, bool startingState, bool invertLogic) :
		PollingSensor(name, interval, offset),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic)
	{
		setPin(pin);
	}

	//destructor
	PS_Alarm::~PS_Alarm()
	{

	}

	//initialization routine
	void PS_Alarm::init()
	{
		getData();	//update ST Cloud with initial data
	}

	//function to get status of the Alarm Siren and queue results for transfer to ST Cloud 
	void PS_Alarm::getData()
	{
		Everything::sendSmartString(getName() + (m_bCurrentState == HIGH ? F(" both") : F(" off")));
	}

	//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
	void PS_Alarm::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (debug) {
			Serial.print(F("PS_Alarm::beSmart s = "));
			Serial.println(s);
		}
		if (s == F("both"))
		{
			m_bCurrentState = HIGH;
		}
		else if (s == F("off"))
		{
			m_bCurrentState = LOW;
		}

		writeStateToPin();

		getData();
	}

	void PS_Alarm::setPin(byte pin)
	{
		m_nPin = pin;
		pinMode(m_nPin, OUTPUT);
		writeStateToPin();
	}
}