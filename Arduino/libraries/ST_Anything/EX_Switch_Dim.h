//******************************************************************************************
//  File: EX_Switch_Dim.h
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_Switch_Dim is a class which implements the SmartThings "Switch" and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Switch_Dim executor1("dimmerSwitch1", PIN_SWITCH, PIN_LEVEL, LOW, true);
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
//
//
//******************************************************************************************
#ifndef ST_EX_SWITCH_DIM
#define ST_EX_SWITCH_DIM

#include "Executor.h"

namespace st
{
	class EX_Switch_Dim: public Executor
	{
		private:
			bool m_bCurrentState;	//HIGH or LOW
			bool m_bInvertLogic;	//determines whether the Arduino Digital Output should use inverted logic
			byte m_nPinSwitch;		//Arduino Pin used as a Digital Output for the switch - often connected to a relay or an LED
			byte m_nPinPWM;			//Arduino Pin used as a PWM Output for the switch level capability
			byte m_nCurrentLevel;	//Switch Level value from SmartThings (0 to 100)

			void writeStateToPin();	//function to update the Arduino Digital Output Pin
			void writeLevelToPin();	//function to update the Arduino PWM Output Pin

		public:
			//constructor - called in your sketch's global variable declaration section
			EX_Switch_Dim(const __FlashStringHelper *name, byte pinSwitch, byte pinPWM, bool startingState = LOW, bool invertLogic = false);
			
			//destructor
			virtual ~EX_Switch_Dim();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output) and the LEVEL (PWM Output)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getSwitchPin() const {return m_nPinSwitch;}
			virtual byte getPWMPin() const { return m_nPinPWM; }

			virtual bool getStatus() const { return m_bCurrentState; }	//whether the switch is HIGH or LOW
			virtual bool getLevel() const { return m_nCurrentLevel; }	//Dim Level of the switch

			//sets
			virtual void setSwitchPin(byte pin);
			virtual void setPWMPin(byte pin);
		
	};
}

#endif