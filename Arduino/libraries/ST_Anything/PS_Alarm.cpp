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

	void PS_Alarm::init()
	{
		getData();
	}

	void PS_Alarm::getData()
	{
		Everything::sendSmartString(getName() + (m_bCurrentState == HIGH ? F(" both") : F(" off")));
	}

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