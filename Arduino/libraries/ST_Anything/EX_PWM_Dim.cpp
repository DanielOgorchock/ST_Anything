//******************************************************************************************
//  File: EX_PWM_Dim.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_PWM_Dim is a class which implements the Hubitat "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_PWM_Dim executor1(F("dimmerSwitch1"), PIN_PWM, LOW, true, 1023);
//
//			  st::EX_PWM_Dim() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin_pwm - REQUIRED - the Arduino Pin to be used as a pwm output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//              - unsigned short analogWriteRangeVal - OPTIONAL - determines the range of input values for the analogWrite() call for only the ESP8266 boards.  Defaults to 1023 for backwards compatibility.
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
//    2019-12-19  Doug Johnson   Created new file based on EX_Switch_Dim to remove separate switch, and add dedicated smoothing
//    2020-04-01  Dan Ogorchock  Added compatability for traditional Arduino Boards
//    2025-11-29  Dan Ogorchock  Added special handling for ESP8266 0-1023 PWM range, as the v3.x ESP8266 Arduino
//                               board support package reverted the default range to 0-255 to match all other Arduino boards
//    2025-12-05  Dan Ogorchock  Fixed comments section 
//
//******************************************************************************************
#include "EX_PWM_Dim.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//private

	void EX_PWM_Dim::writeLevelToPin()
	{
		if((m_nCurrentLevel == 0) && (m_bSetState == HIGH))
		{
			for(int i=1; i < m_nSetLevel; i++)
			{
				m_nCurrentLevel++;
#if defined(ARDUINO_ARCH_ESP32)
				if (st::Executor::debug) {
					Serial.println(F("EX_Switch_Dim:: analogWrite not currently supported on ESP32!"));
				}
#elif defined(ARDUINO_ARCH_ESP8266)
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, m_analogWriteRange));
#else
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 255));
#endif
				delay(7);
			}
		}
		if((m_nCurrentLevel > 0) && (m_bSetState == LOW))
		{
			for(int i=m_nCurrentLevel; i > 0; i--)
			{
				m_nCurrentLevel--;
#if defined(ARDUINO_ARCH_ESP32)
				if (st::Executor::debug) {
					Serial.println(F("EX_Switch_Dim:: analogWrite not currently supported on ESP32!"));
				}
#elif defined(ARDUINO_ARCH_ESP8266)
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, m_analogWriteRange));
#else
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 255));
#endif
				delay(7);
			}
		}


		
		if((m_nCurrentLevel < m_nSetLevel)&& (m_bSetState == HIGH))
		{
			for(int i=m_nCurrentLevel; i < m_nSetLevel; i++)
			{
				m_nCurrentLevel++;
#if defined(ARDUINO_ARCH_ESP32)
				if (st::Executor::debug) {
					Serial.println(F("EX_Switch_Dim:: analogWrite not currently supported on ESP32!"));
				}
#elif defined(ARDUINO_ARCH_ESP8266)
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, m_analogWriteRange));
#else
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 255));
#endif
				delay(7);
			}
		}
		if(m_nCurrentLevel > m_nSetLevel)
		{
			for(int i=m_nCurrentLevel; i > m_nSetLevel; i--)
			{
				m_nCurrentLevel--;
#if defined(ARDUINO_ARCH_ESP32)
				if (st::Executor::debug) {
					Serial.println(F("EX_Switch_Dim:: analogWrite not currently supported on ESP32!"));
				}
#elif defined(ARDUINO_ARCH_ESP8266)
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, m_analogWriteRange));
#else
				analogWrite(m_nPinPWM, map(m_nCurrentLevel, 0, 100, 0, 255));
#endif
				delay(7);
			}
		}
		if(m_nCurrentLevel == 0)
		{
			m_bCurrentState = LOW;
		}
		else
		{
			m_bCurrentState = HIGH;
		}
	}

	//public
	//constructor
	EX_PWM_Dim::EX_PWM_Dim(const __FlashStringHelper *name, byte pinPWM, bool startingState, bool invertLogic, unsigned short analogWriteRangeVal) :
		Executor(name),
		m_bCurrentState(startingState),
		m_bInvertLogic(invertLogic),
		m_analogWriteRange(analogWriteRangeVal)
	{
		m_nCurrentLevel = startingState == HIGH ? 100 : 0;
		setPWMPin(pinPWM);
#if defined(ARDUINO_ARCH_ESP8266)
		analogWriteRange(m_analogWriteRange);
#endif		
	}

	//destructor
	EX_PWM_Dim::~EX_PWM_Dim()
	{

	}

	void EX_PWM_Dim::init()
	{
		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		refresh();
	}

	void EX_PWM_Dim::beSmart(const String &str)
	{
		//String s = str.substring(str.indexOf(' ') + 1, str.indexOf(':'));
		//String dur = str.substring(str.indexOf(':') + 1);
		//dur.trim();
		
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Executor::debug) {
			Serial.print(F("EX_PWM_Dim::beSmart s = "));
			Serial.println(s);
		}
		if (s == F("on"))
		{
			m_bSetState = HIGH;
		}
		else if (s == F("off"))
		{
			m_bSetState = LOW;
		}
		else //must be a set level command
		{
			s.trim();
			m_nSetLevel = byte(s.toInt());
			if (m_nSetLevel == 0)
			{
				m_bSetState = LOW;
			}
			else
			{
				m_bSetState = HIGH;
			}
		}

		writeLevelToPin();

		//Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		refresh();

	}

	void EX_PWM_Dim::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		Everything::sendSmartString(getName() + " " + m_nSetLevel);
	}


	void EX_PWM_Dim::setPWMPin(byte pin)
	{
		m_nPinPWM = pin;
		pinMode(m_nPinPWM, OUTPUT);
		writeLevelToPin();
	}
}
