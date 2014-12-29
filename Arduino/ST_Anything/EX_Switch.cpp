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
	EX_Switch::EX_Switch(const String &name, byte pin, bool statingState):
		Executor(name),
		m_bState=startingState
	{
		setPin(pin);
	}

	//destructor
	EX_Switch::~EX_Switch()
	{
	
	}
	
	
	const String& EX_Switch::beSmart(const String &str)
	{
		str=str.substring(str.indexof(' ')+1);
		if(str=="on")
		{
			m_bCurrentState=HIGH;
		}
		else if(str=="off")
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