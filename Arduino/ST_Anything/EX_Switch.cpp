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
	EX_Switch::EX_Switch(const String &name, byte pin, bool startingState, bool invertLogic) :
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