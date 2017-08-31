//******************************************************************************************
//  File: S_TimedRelay.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  S_TimedRelay is a class which implements the SmartThings "Relay" device capability.  It features
//			  an automatic-turn-off time delay for a relay to emulate a button press.
//
//			  It inherits from the st::Sensor class and clones much from the st::Executor Class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::S_TimedRelay sensor1("relaySwitch1", PIN_RELAY, LOW, true, 1000, 0, 1);
//
//			  st::S_TimedRelay() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Ouput should use inverted logic
//				- long onTime - REQUIRED - the number of milliseconds to keep the output on, DEFGAULTS to 1000 milliseconds
//				- long offTime - OPTIONAL - the number of milliseconds to keep the output off, DEFAULTS to 0
//				- intnumCycles - OPTIONAL - the number of times to repeat the on/off cycle, DEFAULTS to 1
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-12-29  Dan Ogorchock  Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "S_TimedRelay.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void S_TimedRelay::writeStateToPin()
	{
		digitalWrite(m_nOutputPin, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

//public
	//constructor
	S_TimedRelay::S_TimedRelay(const __FlashStringHelper *name, byte pinOutput, bool startingState, bool invertLogic, unsigned long onTime, unsigned long offTime, unsigned int numCycles) :
		Sensor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic),
		m_lOnTime(onTime),
		m_lOffTime(offTime),
		m_iNumCycles(numCycles),
		m_iCurrentCount(numCycles),
		m_lTimeChanged(0),
		m_bTimerPending(false)
		{
			setOutputPin(pinOutput);
			if (numCycles < 1)
			{
				m_iNumCycles = 1;
				m_iCurrentCount = 1;
				
				Serial.println(F("S_TimedRelay:: INVALID Number of Cycles Requested!  Must be at least 1.  Setting to 1."));
				
			}
		}
	
	//destructor
	S_TimedRelay::~S_TimedRelay()
	{
	}
	
	void S_TimedRelay::init()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}

	//update function 
	void S_TimedRelay::update()
	{
		if (m_iCurrentCount < m_iNumCycles)
		{
			//Turn off digital output if timer has expired
			if ((m_bCurrentState == HIGH) && (millis() - m_lTimeChanged >= m_lOnTime))
			{	
				m_bCurrentState = LOW;
				writeStateToPin();
				m_lTimeChanged = millis();
			}
			else if ((m_bCurrentState == LOW) && (millis() - m_lTimeChanged >= m_lOffTime))
			{	
				//add one to the current count since we finished an on/off cycle, and turn on output if needed
				m_iCurrentCount++;
				if (m_iCurrentCount < m_iNumCycles)
				{
					m_bCurrentState = HIGH;
					writeStateToPin();
					m_lTimeChanged = millis();
				}
				
			}
			
			//Check to see if we just finished the requested number of cycles
			if (m_iCurrentCount == m_iNumCycles)
			{
				//Decrement number of active timers
				if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
				m_bTimerPending = false;

				//Queue the relay status update the ST Cloud
				Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
			}
		}
	}

	void S_TimedRelay::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Device::debug) {
			Serial.print(F("S_TimedRelay::beSmart s = "));
			Serial.println(s);
		}
		if ((s == F("on")) && (m_bCurrentState == LOW))
		{
			m_bCurrentState = HIGH;

			//Save time turned on
			m_lTimeChanged = millis();

			//Increment number of active timers
			if (!m_bTimerPending)
			{
				st::Everything::bTimersPending++;
				m_bTimerPending = true;
			}
			//Queue the relay status update the ST Cloud 
			Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
			
			//Set the initial count to zero
			m_iCurrentCount = 0;

			//update the digital output
			writeStateToPin();
		}
		else if ((s == F("off")) && (m_bCurrentState == HIGH))
		{
			m_bCurrentState = LOW;

			//Decrement number of active timers
			if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
			m_bTimerPending = false;
			
			//Queue the relay status update the ST Cloud 
			Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
			
			//Reset the count to the number of required cycles to prevent Update() routine from running if someone sends an OFF command
			m_iCurrentCount = m_iNumCycles;

			//update the digital output
			writeStateToPin();
		}
		
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void S_TimedRelay::refresh()
	{
		//Queue the relay status update the ST Cloud
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}

	void S_TimedRelay::setOutputPin(byte pin)
	{
		m_nOutputPin = pin;
		pinMode(m_nOutputPin, OUTPUT);
		writeStateToPin();
	}

}