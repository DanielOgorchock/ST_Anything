#include "EX_Switch.h"


namespace st
{
//private
	void EX_Switch::writeStateToPin()
	{
		digitalWrite(m_nPin, m_bCurrentState);
	}
	


//public
	//constructor
	EX_Switch::EX_Switch(const String &name, byte pin, bool startingState):
		Executor(name),
		m_bCurrentState(startingState)
	{
		setPin(pin);
	}

	//destructor
	EX_Switch::~EX_Switch()
	{
	
	}
	
	
	const String& EX_Switch::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if(s=="on")
		{
			m_bCurrentState=HIGH;
		}
		else if(s=="off")
		{
			m_bCurrentState==LOW;
		}
		
		writeStateToPin();
	}
	
	
	void EX_Switch::setPin(byte pin)
	{
		m_nPin=pin;
		writeStateToPin();
	}


}