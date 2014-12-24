#include "InterruptSensor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	const String& InterruptSensor::checkIfTriggered()
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
	InterruptSensor::InterruptSensor(byte id, byte pin, bool iState, bool pullup):
		Sensor(id),
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
	
	const String& InterruptSensor::init()
	{
		return Constants::IGNORE_STRING;
	}
	
	const String& InterruptSensor::update()
	{
		return checkIfTriggered();
	}

	const String& InterruptSensor::runInterrupt()
	{
		if(debug)
		{
			Everything::Return_String="DEBUG: \""+String(getId())+"\" triggered (" + (m_bInterruptState?"HIGH)":"LOW)");
			return Everything::Return_String;
		}
		else
		{
			return Constants::IGNORE_STRING;
		}
	}
	
	const String& InterruptSensor::runInterruptEnded()
	{
		if(debug)
		{
			Everything::Return_String="DEBUG: \""+String(getId())+"\" ended (" + (m_bInterruptState?"LOW)":"HIGH)");
			return Everything::Return_String;
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