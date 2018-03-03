//******************************************************************************************
//  File: PS_PulseCounter.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_PulseCounter is a class which implements the SmartThings "Power Meter"-style device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses a digital input to measure the 
//			  the number of counts between polling intervals.  At the polling interval, the pulse count is converted
//			  to engineering units via a linear conversion (engUnits = slope x counts + offset).
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_PulseCounter sensor3(F("power1"), 60, 5, PIN_PULSE, FALLING, INPUT_PULLUP, 1.0, 0);
//
//			  st::PS_PulseCounter() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must be in form of "power1", "power2", etc...
//				- int interval - REQUIRED - the polling interval in seconds
//				- int offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the GPIO Pin to be used as an digital input (Hardware Interrupt)
//				- byte inttype - REQUIRED - which type of Arduino interrupt to trigger the ISR (RISING, FALLING, CHANGE)
//				- byte inputmode - REQUIRED - Mode of the digital input Pin (INPUT, INPUT_PULLUP)
//				- float cnvslope - REQUIRED - Conversion to Engineering Units Slope
//				- float cnvoffset - REQUIRED - Conversion to Engineering Units Offset
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to 
//			  the ST Shield.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-03-31  Dan Ogorchock  Original Creation
//    2018-03-03  Dan Ogorchock  Improved code to make generic for all boards, not just Arduino MEGA 
//
//
//******************************************************************************************

#include "PS_PulseCounter.h"

#include "Constants.h"
#include "Everything.h"
//#include "PinChangeInt.h"

namespace st
{
	//private

	//This "Counts" variables must be declared here so they can be used in the Interrupt Service Routines (ISR)
	volatile unsigned long m_nCounts; //current count of interrupts (pulses)

	//These are the four Interrupt Service Routines (ISR) which must be unique for each interrupt
	void isrPulse() {
		m_nCounts++;
	}

//public

	//constructor - called in your sketch's global variable declaration section
	PS_PulseCounter::PS_PulseCounter(const __FlashStringHelper *name, unsigned int interval, int offset, byte inputpin, byte inttype, byte inputmode, float cnvslope, float cnvoffset) :
		PollingSensor(name, interval, offset),
		m_nInputMode(inputmode),
		m_nSensorValue(0),
		m_fCnvSlope(cnvslope),
		m_fCnvOffset(cnvoffset)
	{
		setPin(inputpin);
		m_nCounts = 0;
		m_pCounter = &m_nCounts;
		attachInterrupt(digitalPinToInterrupt(m_nInputPin), isrPulse, inttype);	
	}
	
	//destructor
	PS_PulseCounter::~PS_PulseCounter()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_PulseCounter::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		
		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_PulseCounter::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_PulseCounter::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud
	void PS_PulseCounter::getData()
	{
		if (m_pCounter)
		{
			noInterrupts();
				unsigned long tmpCounts = *m_pCounter;
				*m_pCounter = 0;
			interrupts();

			m_nSensorValue = long(m_fCnvSlope * tmpCounts + m_fCnvOffset);

		}
		else  //invalid Pin/Interrupt was requested, therefore we are in an error condition
		{
			m_nSensorValue = 0;
			if (st::PollingSensor::debug) {
				Serial.println(F("PS_PulseCounter::Something went wrong. Need to debug."));
			}
		}

		Everything::sendSmartString(getName() + " " + m_nSensorValue);
	}

	void PS_PulseCounter::setPin(byte pin)
	{
		m_nInputPin = pin;
		pinMode(m_nInputPin, m_nInputMode);
	}
}