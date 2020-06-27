//******************************************************************************************
//  File: IS_LatchingRelaySwitch.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  IS_LatchingRelaySwitch is a class which implements the "Valve" device capability, where output1 opens a valve, and
//            output2 closes a valve.  It features optional automatic-turn-off time delay times for for both outputs.
//
//			  It inherits from the st::InterruptSensor class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_LatchingRelaySwitch sensor1(F("switch1"), PIN_INPUT, HIGH, true, 500, PIN_RELAY1, PIN_RELAY2, LOW, true, 1000, 1000);
//
//			  st::IS_LatchingRelaySwitch() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinInput - REQUIRED - the Arduino Pin to be used as a digital input to determine the switch state of latching relay
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - REQUIRED - true == INTERNAL_PULLUP
//				- long numReqCounts - REQUIRED - number of counts before changing state of input (prevent false alarms)
//				- byte pinOutput1 - REQUIRED - the Arduino Pin to be used as a digital output
//				- byte pinOutput2 - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "closed", HIGH = "open"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Output should use inverted logic (e.g. active high versus active low relays)
//				- long Output1Time - REQUIRED - the number of milliseconds to keep the output1 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//				- long Output2Time - REQUIRED - the number of milliseconds to keep the output2 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//              - bool initializeOutputs - OPTIONAL - determines if the digital outputs are activated during initialization/startup, defaults to 'false'
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2020-06-26  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#include "IS_LatchingRelaySwitch.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void IS_LatchingRelaySwitch::writeStateToPin(byte pin, bool state)
	{
		digitalWrite(pin, m_bInvertLogic ? !state : state);
	}

//public
	//constructor
	IS_LatchingRelaySwitch::IS_LatchingRelaySwitch(const __FlashStringHelper *name, byte pinInput, bool iState, bool internalPullup, long numReqCounts, byte pinOutput1, byte pinOutput2, bool startingState, bool invertLogic, unsigned long Output1Time, unsigned long Output2Time, bool initializeOutputs) :
		InterruptSensor(name, pinInput, iState, internalPullup, numReqCounts),  //use parent class' constructor,
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

			if (initializeOutputs)
			{ 
				//update the digital outputs
				if (((m_bCurrentState == HIGH) && (m_lOutput1Time > 0)) || ((m_bCurrentState == LOW) && (m_lOutput2Time > 0)))
				{
					m_bTimerPending = true;
				}
				m_lTimeChanged = millis();
				writeStateToPin(m_nOutputPin1, m_bCurrentState); 
				writeStateToPin(m_nOutputPin2, !m_bCurrentState);
			}
			else
			{
				//Make sure both digital outputs are 'off' if initializeOutputs == false
				writeStateToPin(m_nOutputPin1, LOW);
				writeStateToPin(m_nOutputPin2, LOW);
			}
			

		}
	
	//destructor
	IS_LatchingRelaySwitch::~IS_LatchingRelaySwitch()
	{
	}
	
	void IS_LatchingRelaySwitch::init()
	{
		//get current status of switch by calling parent class's init() routine - no need to duplicate it here!
		InterruptSensor::init();
		//refresh();
	}

	//update function 
	void IS_LatchingRelaySwitch::update()
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

		//make sure to call the parent class' update() since we overrode it for the above timer logic
		InterruptSensor::update();
	}

	void IS_LatchingRelaySwitch::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Device::debug) {
			Serial.print(F("IS_LatchingRelaySwitch::beSmart s = "));
			Serial.println(s);
		}

		//if ((s == F("open")) && (m_bCurrentState == LOW))
		if (s == F("on"))
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
			//refresh();
			
			//update the digital outputs
			writeStateToPin(m_nOutputPin2, !m_bCurrentState);
			writeStateToPin(m_nOutputPin1, m_bCurrentState);
		}
		//else if ((s == F("close")) && (m_bCurrentState == HIGH))
		else if (s == F("off"))
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
			
			//Queue the relay status update the Hub 
			//refresh();
			
			//update the digital outputs
			writeStateToPin(m_nOutputPin1, m_bCurrentState);
			writeStateToPin(m_nOutputPin2, !m_bCurrentState);
		}
		
	}

	//called periodically by Everything class to ensure Hub is kept consistent with the state of the contact sensor
	void IS_LatchingRelaySwitch::refresh()
	{
		//Queue the relay status update the Hub
		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		Everything::sendSmartString(getName() + (getStatus() ? F(" off") : F(" on")));
	}

	void IS_LatchingRelaySwitch::runInterrupt()
	{
		//add the "closed" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" off"));
	}
	
	void IS_LatchingRelaySwitch::runInterruptEnded()
	{
		//add the "open" event to the buffer to be queued for transfer to the ST Shield
		Everything::sendSmartString(getName() + F(" on"));
	}

}
