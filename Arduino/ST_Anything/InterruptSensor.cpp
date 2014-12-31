#include "InterruptSensor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	const String& InterruptSensor::checkIfTriggered()
	{
		//if ((millis() % 10 == 0) && (millis() != m_lnglastmillis))
		//{
		//	m_lnglastmillis = millis();

			if (digitalRead(m_nInterruptPin) == m_bInterruptState && !m_bStatus) //new interrupt
			{
				m_bStatus = true;
				m_bInitRequired = false;
				return runInterrupt();
			}
			else if ((digitalRead(m_nInterruptPin) != m_bInterruptState && m_bStatus) || m_bInitRequired) //interrupt has ended OR Init called us
			{
				m_bStatus = false;
				m_bInitRequired = false;
				return runInterruptEnded();
			}
			else //still in the middle of an interrupt or interrupt hasn't triggered
			{
				return Constants::IGNORE_STRING;
			}
		//}
		//else //not time to check the digital input pin
		//{
		//	return Constants::IGNORE_STRING;
		//}
	}



//public
	//constructor
	InterruptSensor::InterruptSensor(const String &name, byte pin, bool iState, bool pullup):
		Sensor(name),
		m_bInterruptState(iState),
		m_bStatus(false),
		m_bPullup(pullup),
		m_bInitRequired(true)//,
		//m_lnglastmillis(0)
		{
			setInterruptPin(pin);
		}
	
	//destructor
	InterruptSensor::~InterruptSensor()
	{
	
	}
	
	const String& InterruptSensor::init()
	{
		return checkIfTriggered();
	}
	
	const String& InterruptSensor::update()
	{
		return checkIfTriggered();
	}

	const String& InterruptSensor::runInterrupt()
	{
		if(debug)
		{
			Everything::Return_String.remove(0);
			Everything::Return_String+="DEBUG: \""+getName()+"\" triggered (" + (m_bInterruptState?"HIGH)":"LOW)");
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
			Everything::Return_String.remove(0);
			Everything::Return_String+="DEBUG: \""+getName()+"\" ended (" + (m_bInterruptState?"LOW)":"HIGH)");
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