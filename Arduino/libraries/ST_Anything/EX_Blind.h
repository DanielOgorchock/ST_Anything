//******************************************************************************************
//  File: EX_Blind.h
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
#ifndef ST_EX_Blind
#define ST_EX_Blind

#include "Executor.h"

namespace st
{
	class EX_Blind: public Executor
	{
		private:
			bool m_bCurrentState;	//HIGH or LOW
			bool m_bInvertLogic;	//determines whether the Arduino Digital Output should use inverted logic
			byte m_nPin;			//Arduino Pin used as a Digital Output for up
            byte m_dPin;            //Arduino Pin used as a Digital Output for Down
		
			void writeStateToPin();	//function to update the Arduino Digital Output Pin
		
		public:
			//constructor - called in your sketch's global variable declaration section
			EX_Blind(const __FlashStringHelper *name, byte upin, byte dpin, bool startingState = LOW, bool invertLogic = false);
			
			//destructor
			virtual ~EX_Blind();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getPin() const {return m_nPin;}
			
			virtual bool getStatus() const { return m_bCurrentState; }	//whether the switch is HIGH or LOW

			//sets
			virtual void setPin(byte upin, byte dpin);
		
	};
}

#endif