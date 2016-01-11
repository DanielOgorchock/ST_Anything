//******************************************************************************************
//  File: InterruptSensor.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::InterruptSensor is a generic class which inherits from st::Sensor.  This is the
//			  parent class for the st::IS_Motion, IS_Contact, and IS_DoorControl classes.
//			  In general, this file should not need to be modified.   
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//	  2015-03-17  Dan			 Added optional "numReqCounts" constructor argument/capability
//
//
//******************************************************************************************

#include "InterruptSensor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

	//Checks to see if the pin has changed state.  If so calls appropriate function.
	void InterruptSensor::checkIfTriggered()
	{
			if (digitalRead(m_nInterruptPin) == m_bInterruptState && !m_bStatus) //new interrupt
			{
				m_nCurrentDownCount = m_nRequiredCounts;
				m_nCurrentUpCount++;
				if (m_nCurrentUpCount >= m_nRequiredCounts)
				{
					m_bStatus = true;
					m_bInitRequired = false;
					runInterrupt();
				}
			}
			else if ((digitalRead(m_nInterruptPin) != m_bInterruptState && m_bStatus) || m_bInitRequired) //interrupt has ended OR Init called us
			{
				m_nCurrentUpCount = 0;
				m_nCurrentDownCount--;
				if (m_nCurrentDownCount <= 0)
				{
					m_bStatus = false;
					m_bInitRequired = false;
					runInterruptEnded();
				}
			}
	}

//public
	//constructor
	InterruptSensor::InterruptSensor(const __FlashStringHelper *name, byte pin, bool iState, bool pullup, long numReqCounts) :
		Sensor(name),
		m_bInterruptState(iState),
		m_bStatus(false),
		m_bPullup(pullup),
		m_bInitRequired(true),
		m_nRequiredCounts(numReqCounts),
		m_nCurrentUpCount(0),
		m_nCurrentDownCount(numReqCounts)
		{
			setInterruptPin(pin);
		}
	
	//destructor
	InterruptSensor::~InterruptSensor()
	{
	
	}
	
	//initialization function
	void InterruptSensor::init()
	{
		checkIfTriggered();
	}
	
	//update function 
	void InterruptSensor::update()
	{
		checkIfTriggered();
	}

	//handles start of an interrupt - all derived classes should implement this virtual function
	void InterruptSensor::runInterrupt()
	{
		if(debug)
		{
			Everything::sendSmartString(getName()+F(" triggered ") + (m_bInterruptState?F("HIGH"):F("LOW)")));
		}
	}
	
	//handles the end of an interrupt - all derived classes should implement this virtual function
	void InterruptSensor::runInterruptEnded()
	{
		if(debug)
		{
			Everything::sendSmartString(getName()+F(" ended ") + (m_bInterruptState?F("LOW)"):F("HIGH)")));
		}
	}
	
	//sets the pin to be monitored, and set the Arduino pinMode based on constructor data
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
	
	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool InterruptSensor::debug=false;
}