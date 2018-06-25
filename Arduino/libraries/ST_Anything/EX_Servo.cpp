//******************************************************************************************
//  File: EX_Servo.cpp
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_Servo is a class which implements the SmartThings/Hubitat "Switch Level" device capability.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Servo executor1(F("servo1"), PIN_SERVO, INITIAL_ANGLE);
//
//			  st::EX_Servo() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin_pwm - REQUIRED - the Arduino Pin to be used as a pwm output
//				- int startingAngle - OPTIONAL - the value desired for the initial angle of the servo motor (defaults to 90)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2018-06-23  Dan Ogorchock  Original Creation
//    2018-06-24  Dan Ogorchock  Since ESP32 does not support SERVO library, exclude all code to prevent compiler error
//
//
//******************************************************************************************
#include "EX_Servo.h"

#include "Constants.h"
#include "Everything.h"

#if not defined(ARDUINO_ARCH_ESP32)

namespace st
{
	//private
	void EX_Servo::writeAngleToPin()
	{
		m_nCurrentAngle = map(m_nCurrentLevel, 0, 99, 0, 180);
		m_Servo.write(m_nCurrentAngle);

		if (st::Executor::debug) {
			Serial.print(F("EX_Servo:: Servo motor angle set to "));
			Serial.println(m_nCurrentAngle);
		}


	}

	//public
	//constructor
	EX_Servo::EX_Servo(const __FlashStringHelper *name, byte pinPWM, int startingAngle) :
		Executor(name),
		m_Servo(),
		m_nCurrentAngle(startingAngle)
	{
		setPWMPin(pinPWM);
		m_Servo.attach(m_nPinPWM);
		m_nCurrentLevel = map(m_nCurrentAngle, 0, 180, 0, 99);
	}

	//destructor
	EX_Servo::~EX_Servo()
	{

	}

	void EX_Servo::init()
	{
		writeAngleToPin();
		refresh();
	}

	void EX_Servo::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		if (st::Executor::debug) {
			Serial.print(F("EX_Servo::beSmart s = "));
			Serial.println(s);
		}
		
		s.trim();
		
		m_nCurrentLevel = int(s.toInt());

		writeAngleToPin();

		refresh();

	}

	void EX_Servo::refresh()
	{
		Everything::sendSmartString(getName() + " " + m_nCurrentLevel);
	}

	void EX_Servo::setPWMPin(byte pin)
	{
		m_nPinPWM = pin;
	}
}

#endif