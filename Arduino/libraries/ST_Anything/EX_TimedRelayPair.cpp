//******************************************************************************************
//  File: EX_TimedRelayPair.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_TimedRelayPair is a class which implements the "Valve" device capability, where output1 opens a valve, and
//            output2 closes a valve.  It features optional automatic-turn-off time delay times for for both outputs.
//
//			  It inherits from the st::Executor class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_TimedRelayPair executor1(F("valve1"), PIN_RELAY1, PIN_RELAY2, LOW, true, 1000, 1000);
//
//			  st::EX_TimedRelayPair() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinOutput1 - REQUIRED - the Arduino Pin to be used as a digital output
//				- byte pinOutput2 - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "closed", HIGH = "open"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Output should use inverted logic (e.g. active high versus active low relays)
//				- long Output1Time - REQUIRED - the number of milliseconds to keep the output1 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//				- long Output2Time - REQUIRED - the number of milliseconds to keep the output2 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-10-30  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#include "EX_TimedRelayPair.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void EX_TimedRelayPair::writeStateToPin(byte pin, bool state)
	{
		digitalWrite(pin, m_bInvertLogic ? !state : state);
	}

//public
	//constructor
	EX_TimedRelayPair::EX_TimedRelayPair(const __FlashStringHelper *name, byte pinOutput1, byte pinOutput2, bool startingState, bool invertLogic, unsigned long Output1Time, unsigned long Output2Time) :
		Executor(name),
		m_nOutputPin1(pinOutput1),
		m_nOutputPin2(pinOutput2),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic),
		m_lOutput1Time(Output1Time),
		m_lOutput2Time(Output2Time),
		m_lTimeChanged(0),
		m_bTimerPending(false)

		{
			//set pin mode
			pinMode(m_nOutputPin1, OUTPUT);
			pinMode(m_nOutputPin2, OUTPUT);
			//update the digital outputs
			if (((m_bCurrentState == HIGH) && (m_lOutput1Time > 0)) || ((m_bCurrentState == LOW) && (m_lOutput2Time > 0)))
			{
				m_bTimerPending = true;
			}
			m_lTimeChanged = millis();
			writeStateToPin(m_nOutputPin1, m_bCurrentState);
			writeStateToPin(m_nOutputPin2, !m_bCurrentState);
		}
	
	//destructor
	EX_TimedRelayPair::~EX_TimedRelayPair()
	{
	}
	
	void EX_TimedRelayPair::init()
	{
		refresh();
	}

	//update function 
	void EX_TimedRelayPair::update()
	{

		if (m_bTimerPending)
		{
			if ((m_bCurrentState == HIGH) && (millis() - m_lTimeChanged >= m_lOutput1Time))
			{
				writeStateToPin(m_nOutputPin1, LOW);
			}
			else if ((m_bCurrentState == LOW) && (millis() - m_lTimeChanged >= m_lOutput2Time))
			{
				writeStateToPin(m_nOutputPin2, LOW);
			}
		}

	}

	void EX_TimedRelayPair::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Device::debug) {
			Serial.print(F("EX_TimedRelayPair::beSmart s = "));
			Serial.println(s);
		}

		//if ((s == F("open")) && (m_bCurrentState == LOW))
		if (s == F("open"))
			{
			if (m_bTimerPending)
			{
				if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
				m_bTimerPending = false;
			}
			
			m_bCurrentState = HIGH;

			//Save time turned on
			m_lTimeChanged = millis();

			//Increment number of active timers
			if ((!m_bTimerPending) && (m_lOutput1Time > 0))
			{
				st::Everything::bTimersPending++;
				m_bTimerPending = true;
			}
			//Queue the relay status update the ST Cloud 
			refresh();
			
			//update the digital outputs
			writeStateToPin(m_nOutputPin2, !m_bCurrentState);
			writeStateToPin(m_nOutputPin1, m_bCurrentState);
		}
		//else if ((s == F("close")) && (m_bCurrentState == HIGH))
		else if (s == F("close"))
		{
			if (m_bTimerPending)
			{
				if (st::Everything::bTimersPending > 0) st::Everything::bTimersPending--;
				m_bTimerPending = false;
			}

			m_bCurrentState = LOW;

			//Save time turned on
			m_lTimeChanged = millis();

			//Increment number of active timers
			if ((!m_bTimerPending) && (m_lOutput2Time > 0))
			{
				st::Everything::bTimersPending++;
				m_bTimerPending = true;
			}
			
			//Queue the relay status update the ST Cloud 
			refresh();
			
			//update the digital outputs
			writeStateToPin(m_nOutputPin1, m_bCurrentState);
			writeStateToPin(m_nOutputPin2, !m_bCurrentState);
		}
		
	}

	//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
	void EX_TimedRelayPair::refresh()
	{
		//Queue the relay status update the ST Cloud
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("open") : F("closed")));
	}

	//void EX_TimedRelayPair::setOutputPin(byte pin)
	//{
	//	pinMode(pin, OUTPUT);
	//}

}
