//******************************************************************************************
//  File: EX_Servo.h
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_Servo is a class which implements the SmartThings/Hubitat "Switch Level" device capability.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Servo executor1(F("servo1"), PIN_SERVO, INITIAL_ANGLE, false, 1000);
//
//			  st::EX_Servo() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin_pwm - REQUIRED - the Arduino Pin to be used as a pwm output
//				- int startingAngle - OPTIONAL - the value desired for the initial angle of the servo motor (defaults to 90)
//              - bool detachAfterMove - OPTIONAL - determines if servo motor is powered down after move using following timeout (defaults to false) 
//              - int servoMoveTime - OPTIONAL - determines how long after the servo is moved that the servo is powered down if the above is true (defaults to 1000ms)
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2018-06-23  Dan Ogorchock  Original Creation
//    2018-06-24  Dan Ogorchock  Since ESP32 does not support SERVO library, exclude all code to prevent compiler error
//    2018-08-19  Dan Ogorchock  Added feature to optionally allow servo to be powered down after a move
//
//
//******************************************************************************************
#ifndef ST_EX_SERVO
#define ST_EX_SERVO

#if not defined(ARDUINO_ARCH_ESP32)

#include "Executor.h"
#include <Servo.h>

namespace st
{
	class EX_Servo: public Executor
	{
		private:
			Servo m_Servo;          //Servo object
			byte m_nPinPWM;			//Arduino Pin used as a PWM Output for the switch level capability
			int m_nCurrentLevel;	//Switch Level value from SmartThings (0 to 100)
			int m_nCurrentAngle;	//Switch Level value from SmartThings (0 to 100)
			bool m_bDetachAfterMove; //Issue a servo.detach() after servo move is complete
			int m_nServoMoveTime;   //number of milliseconds to delay() after a servo move before issuing servo.detach()

			void writeAngleToPin();	//function to update the Arduino PWM Output Pin

		public:
			//constructor - called in your sketch's global variable declaration section
			EX_Servo(const __FlashStringHelper *name, byte pinPWM, int startingAngle = 90, bool detachAfterMove = false, int servoMoveTime = 1000);
			
			//destructor
			virtual ~EX_Servo();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output) and the LEVEL (PWM Output)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud / Hubitat Hub (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getPWMPin() const { return m_nPinPWM; }

			virtual bool getAngle() const { return m_nCurrentAngle; }	//Angle of the Servo
			virtual bool getLevel() const { return m_nCurrentLevel; }	//Dim Level from ST/Hubitat

			//sets
			virtual void setPWMPin(byte pin);
	};
}

#endif
#endif