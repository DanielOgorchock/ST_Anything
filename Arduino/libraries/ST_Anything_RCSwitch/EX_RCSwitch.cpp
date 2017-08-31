//******************************************************************************************
//  File: EX_RCSwitch.cpp
//  Author: Dan G Ogorchock
//
//  Summary:  EX_RCSwitch is a class which implements the SmartThings "Switch" device capability.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RCSwitch executor1("switch1", PIN_RCSWITCH, 35754004, 26, 18976788, 26, 174, 1, 15, LOW);
//
//			  st::EX_RCSwitch() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte transmitterPin - REQUIRED - the Arduino Pin to be used as a digital output for the RCSwitch object's transmitter pin
//				- unsigned long onCode - REQUIRED - the "on" code for RCSwitch send() command
//				- unsigned int onLength - REQUIRED - the "on" code's length for RCSwitch send() command
//				- unsigned long offCode - REQUIRED - the "off" code for RCSwitch send() command
//				- unsigned int offLength - REQUIRED - the "off" code's length for RCSwitch send() command
//				- unsigned int pulseLength - REQUIRED - the length of the RF pulse for RCSwitch send() command
//				- byte protocol - OPTIONAL - defaults to "1" - the protocol for RCSwitch send() command
//				- byte repeatTransmits - OPTIONAL - defaults to "15" - the number of repeated transmits for RCSwitch send() command
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-26  Dan Ogorchock  Original Creation
//    2015-05-20  Dan Ogorchock  Improved to work with Etekcity ZAP 3F 433Mhz RF Outlets
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//******************************************************************************************

#include "EX_RCSwitch.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void EX_RCSwitch::writeStateToPin()
	{
		if (m_bCurrentState)
		{
			m_myRCSwitch.send(m_onCode, m_onLength);
		}
		else
		{
			m_myRCSwitch.send(m_offCode, m_offLength);
		}
	}
	


//public
	//constructor
	EX_RCSwitch::EX_RCSwitch(const __FlashStringHelper *name, byte transmitterPin, unsigned long onCode, unsigned int onLength, unsigned long offCode, unsigned int offLength, unsigned int pulseLength, byte protocol, byte repeatTransmits, bool startingState) :
		Executor(name),
		m_bCurrentState(startingState),
		m_myRCSwitch(RCSwitch()),
		m_onCode(onCode),
		m_onLength(onLength),
		m_offCode(offCode),
		m_offLength(offLength)
	{
		setPin(transmitterPin);
		m_myRCSwitch.setProtocol(protocol);					// set protocol (default is 1, will work for most outlets)
		m_myRCSwitch.setRepeatTransmit(repeatTransmits);	// set number of transmission repetitions.
		m_myRCSwitch.setPulseLength(pulseLength);			// Set pulse length.

	}

	//destructor
	EX_RCSwitch::~EX_RCSwitch()
	{
	
	}
	
	void EX_RCSwitch::init()
	{
		writeStateToPin();
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}

	void EX_RCSwitch::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if (st::Executor::debug) {
			Serial.print(F("EX_RCSwitch::beSmart s = "));
			Serial.println(s);
		}
		if(s==F("on"))
		{
			m_bCurrentState=HIGH;
		}
		else if(s==F("off"))
		{
			m_bCurrentState=LOW;
		}
		
		writeStateToPin();
		
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_RCSwitch::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_RCSwitch::setPin(byte pin)
	{
		m_nPin = pin;
		m_myRCSwitch.enableTransmit((int)m_nPin);
	}
}