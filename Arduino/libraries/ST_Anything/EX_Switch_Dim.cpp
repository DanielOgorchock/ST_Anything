//******************************************************************************************
//  File: EX_Switch_Dim.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_Switch_Dim is a class which implements the SmartThings "Switch" and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Switch_Dim executor1(F("dimmerSwitch1"), PIN_SWITCH, PIN_LEVEL, LOW, true);
//
//			  st::EX_Switch_Dim() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- byte pin_pwm - REQUIRED - the Arduino Pin to be used as a pwm output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2016-04-30  Dan Ogorchock  Original Creation
//    2018-08-14  Dan Ogorchock  Modified to avoid compiler errors on ESP32 since it currently does not support "analogWrite()"
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2018-08-30  Dan Ogorchock  Adding reporting of 'level'
//    2018-12-06  Dan Ogorchock  Fixed Comments
//    2019-04-10  Dan Ogorchock  Corrected analogWrite() call for ESP8266 platform
//    2019-07-01  Dan.t		 	 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//
//******************************************************************************************
#include "EX_Switch_Dim.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//private
	void EX_Switch_Dim::writeStateToPin()
	{
		digitalWrite(m_nPinSwitch, m_bInvertLogic ? !m_bCurrentState : m_bCurrentState);
	}

	void EX_Switch_Dim::writeLevelToPin()
	{
#if defined(ARDUINO_ARCH_ESP32)
		if (st::Executor::debug) {
			st::debugPrintln(F("EX_Switch_Dim:: analogWrite not currently supported on ESP32!"));
		}
#elif defined(ARDUINO_ARCH_ESP8266)
		analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 1023));
#else
		analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 255));
#endif
	}

	//public
	//constructor
	EX_Switch_Dim::EX_Switch_Dim(const __FlashStringHelper *name, byte pinSwitch, byte pinPWM, bool startingState, bool invertLogic) :
		Executor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic)
	{
		m_nCurrentLevel = startingState == HIGH ? 100 : 0;
		setSwitchPin(pinSwitch);
		setPWMPin(pinPWM);
	}

	//destructor
	EX_Switch_Dim::~EX_Switch_Dim()
	{

	}

	void EX_Switch_Dim::init()
	{
		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		refresh();
	}

	void EX_Switch_Dim::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Executor::debug) {
			st::debugPrint(F("EX_Switch_Dim::beSmart s = "));
			st::debugPrintln(s);
		}
		if (s == F("on"))
		{
			m_bCurrentState = HIGH;
		}
		else if (s == F("off"))
		{
			m_bCurrentState = LOW;
		}
		else //must be a set level command
		{
			s.trim();
			m_nCurrentLevel = byte(s.toInt());
			if (m_nCurrentLevel == 0)
			{
				m_bCurrentState = LOW;
			}
			else
			{
				m_bCurrentState = HIGH;
			}
		}

		writeStateToPin();
		writeLevelToPin();

		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		refresh();

	}

	void EX_Switch_Dim::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		Everything::sendSmartString(getName() + " " + m_nCurrentLevel);
	}

	void EX_Switch_Dim::setSwitchPin(byte pin)
	{
		m_nPinSwitch = pin;
		pinMode(m_nPinSwitch, OUTPUT);
		writeStateToPin();
	}

	void EX_Switch_Dim::setPWMPin(byte pin)
	{
		m_nPinPWM = pin;
		pinMode(m_nPinPWM, OUTPUT);
		writeLevelToPin();
	}
}
