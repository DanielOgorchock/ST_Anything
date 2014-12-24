#include "InterruptSensor.h"

#include "Constants.h"

namespace st
{
//private
	String InterruptSensor::checkIfTriggered()
	{
		if(digitalRead(m_nInterruptPin)==m_bInterruptState && !m_bStatus) //new interrupt
		{
			m_bStatus=true;
			return runInterrupt();
		}
		else if(digitalRead(m_nInterruptPin)!=m_bInterruptState && m_bStatus) //interrupt has ended
		{
			m_bStatus=false;
			return runInterruptEnded();
		}
		else //still in the middle of an interrupt or interrupt hasn't triggered
		{
			return Constants::IGNORE_STRING;
		}
		
	
	}



//public
	//constructor
	InterruptSensor::InterruptSensor(const String &name, byte pin, bool iState, bool pullup):
		Sensor(name),
		m_bInterruptState(iState),
		m_bStatus(false),
		m_bPullup(pullup)
		{
			setInterruptPin(pin);
		}
	
	//destructor
	InterruptSensor::~InterruptSensor()
	{
	
	}
	
	String InterruptSensor::init()
	{
		return Constants::IGNORE_STRING;
	}
	
	String InterruptSensor::update()
	{
		return checkIfTriggered();
	}

	String InterruptSensor::runInterrupt()
	{
		if(debug)
		{
			return "DEBUG: \""+getName()+"\"'s interrupt triggered (" + (m_bInterruptState?"HIGH)":"LOW)");
		}
		else
		{
			return Constants::IGNORE_STRING;
		}
	}
	
	String InterruptSensor::runInterruptEnded()
	{
		if(debug)
		{
			return "DEBUG: \""+getName()+"\"'s interrupt has ended (" + (m_bInterruptState?"LOW)":"HIGH)");
		}
		else
		{
			return Constants::IGNORE_STRING;
		}
	}
	
	void InterruptSensor::setInterruptPin(byte pin)
	{
		m_nInterruptPin=pin;
		if(!m_bPullup)
		{
			pinMode(m_nInterruptPin, INPUT);
		}
		else
		{
			pinMode(m_nInterruptPin, INPUT_PULLUP);
		}
	}
	
	bool InterruptSensor::debug=false;
}