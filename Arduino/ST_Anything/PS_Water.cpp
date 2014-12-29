#include "PS_Water.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor
	PS_Water::PS_Water(const String &name, unsigned int interval, int offset, byte analogInputPin):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0)
	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_Water::~PS_Water()
	{
		
	}

	//const String& PS_Water::init()
	//{
	//	return getData();
	//}
	
	const String& PS_Water::getData()
	{
		int m_nSensorValue = analogRead(m_nAnalogInputPin);
		
		Everything::Return_String.remove(0);
		Everything::Return_String+=getName() + " " + (m_nSensorValue<100?"dry":"wet");
		return Everything::Return_String;
		
	}
	
	void PS_Water::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}