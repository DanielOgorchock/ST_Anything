#include "InterruptSensor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void InterruptSensor::checkIfTriggered()
	{
		//if ((millis() % 10 == 0) && (millis() != m_lnglastmillis))
		//{
		//	m_lnglastmillis = millis();

			if (digitalRead(m_nInterruptPin) == m_bInterruptState && !m_bStatus) //new interrupt
			{
				m_bStatus = true;
				m_bInitRequired = false;
				runInterrupt();
			}
			else if ((digitalRead(m_nInterruptPin) != m_bInterruptState && m_bStatus) || m_bInitRequired) //interrupt has ended OR Init called us
			{
				m_bStatus = false;
				m_bInitRequired = false;
				runInterruptEnded();
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
	
	void InterruptSensor::init()
	{
		checkIfTriggered();
	}
	
	void InterruptSensor::update()
	{
		checkIfTriggered();
	}

	void InterruptSensor::runInterrupt()
	{
		if(debug)
		{
			Everything::sendSmartString("DEBUG: \""+getName()+"\" triggered (" + (m_bInterruptState?"HIGH)":"LOW)"));
		}
	}
	
	void InterruptSensor::runInterruptEnded()
	{
		if(debug)
		{
			Everything::sendSmartString("DEBUG: \""+getName()+"\" ended (" + (m_bInterruptState?"LOW)":"HIGH)"));
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