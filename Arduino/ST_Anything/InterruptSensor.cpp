#include "InterruptSensor.h"


namespace st
{
//private
	bool InterruptSensor::checkIfTriggered()
	{
		if(digitalRead(m_nInterruptPin)==m_bInterruptState && !m_bStatus) //new interrupt
		{
			m_bStatus=true;
			if(DEBUG)
			{
			Serial.println(getName()+"'s interrupt triggered");
			}
			return true;
		}
		else if(digitalRead(m_nInterruptPin)!=m_bInterruptState && m_bStatus) //interrupt has ended
		{
			m_bStatus=false;
			return false;
		}
		else //still in the middle of an interrupt or interrupt hasn't triggered
		{
			return false;
		}
		
	
	}



//public
	//constructor
	InterruptSensor::InterruptSensor(const String &name, byte pin, bool iState, bool pullup):
		Sensor(name),
		m_nInterruptPin(pin),
		m_bInterruptState(iState),
		m_bStatus(false)
		{
			if(!pullup)
			{
				pinMode(m_nInterruptPin, INPUT);
			}
			else
			{
				pinMode(m_nInterruptPin, INPUT_PULLUP);
			}
		}
	
	//destructor
	InterruptSensor::~InterruptSensor()
	{
	
	}
	
	void InterruptSensor::init()
	{
	
	}
	
	void InterruptSensor::update()
	{
		checkIfTriggered();
	}

	
	bool InterruptSensor::DEBUG=false;
}