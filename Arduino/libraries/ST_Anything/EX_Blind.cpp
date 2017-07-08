//******************************************************************************************
//  File: EX_Blind.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  EX_Blind is a class which implements the SmartThings "Switch" device capability.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Blind executor1("switch", PIN_SWITCH, LOW, true);
//
//			  st::EX_Blind() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#include "EX_Blind.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void EX_Blind::writeStateToPin()
	{
        if (m_bCurrentState == HIGH) {
            if (st::Executor::debug) {Serial.println(F("EX_Blind::Going Up"));}
            digitalWrite(m_nPin, m_bInvertLogic ? HIGH : LOW);
            delay(500);
            digitalWrite(m_nPin, m_bInvertLogic ? LOW : HIGH);
        }
        else
        {
            if (st::Executor::debug) {Serial.println(F("EX_Blind::Going Down"));}
            digitalWrite(m_dPin, m_bInvertLogic ? HIGH : LOW);
            delay(500);
            digitalWrite(m_dPin, m_bInvertLogic ? LOW : HIGH);
        }
	}
	


//public
	//constructor
	EX_Blind::EX_Blind(const __FlashStringHelper *name, byte upin, byte dpin, bool startingState, bool invertLogic) :
		Executor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic)
	{
		setPin(upin, dpin);
	}

	//destructor
	EX_Blind::~EX_Blind()
	{
	
	}
	
	void EX_Blind::init()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("open") : F("closed")));
        
	}

	void EX_Blind::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if (st::Executor::debug) {
			Serial.print(F("EX_Blind::beSmart s = "));
			Serial.println(s);
		}
		if(s==F("open"))
		{
			m_bCurrentState=HIGH;
		}
		else if(s==F("close"))
		{
			m_bCurrentState=LOW;
		}
        else if(s==F("stop"))
        {
            byte stop_pin = 14;
            pinMode(stop_pin, OUTPUT);
            if (st::Executor::debug) {Serial.println(F("EX_Blind::My"));}
            digitalWrite(stop_pin, m_bInvertLogic ? HIGH : LOW);
            delay(500);
            digitalWrite(stop_pin, m_bInvertLogic ? LOW : HIGH);
            return;
        }
        
		writeStateToPin();
		
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("open"):F("closed")));
	}
	
	void EX_Blind::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("open"):F("closed")));
	}
	
	void EX_Blind::setPin(byte upin, byte dpin)
	{
		m_nPin=upin;
		pinMode(m_nPin, OUTPUT);
        digitalWrite(m_nPin, m_bInvertLogic ? LOW : HIGH);
        m_dPin=dpin;
        pinMode(m_dPin, OUTPUT);
        digitalWrite(m_dPin, m_bInvertLogic ? LOW : HIGH);
	}
}