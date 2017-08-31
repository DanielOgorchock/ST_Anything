//******************************************************************************************
//  File: EX_RCSwitch.h
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
#ifndef ST_EX_RCSWITCH
#define ST_EX_RCSWITCH

#include "RCSwitch.h"
#include "Executor.h"

namespace st
{
	class EX_RCSwitch: public Executor
	{
		private:
			bool m_bCurrentState;	//HIGH or LOW
			byte m_nPin;			//Arduino Pin used as a RC Transmitter
			RCSwitch m_myRCSwitch;	//RCSwitch Object
			unsigned long m_onCode;		//RCSwitch On Code
			unsigned int m_onLength;	//RCSwitch On Length
			unsigned long m_offCode;	//RCSwitch Off Code
			unsigned int m_offLength;	//RCSwitch Off Length
		
			void writeStateToPin();	//function to update the Arduino digital output pin via RCSwitch switchOn and switchOff commands
		
		public:
			//constructor - called in your sketch's global variable declaration section
			EX_RCSwitch(const __FlashStringHelper *name, byte transmitterPin, unsigned long onCode, unsigned int onLength, unsigned long offCode, unsigned int offLength, unsigned int pulseLength, byte protocol = 1, byte repeatTransmits = 15, bool startingState = LOW);
			
			//destructor
			virtual ~EX_RCSwitch();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getPin() const {return m_nPin;}
			
			//sets
			virtual void setPin(byte pin);
		
	};
}

#endif