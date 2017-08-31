//******************************************************************************************
//  File: EX_Alarm.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  EX_Alarm is a class which implements the SmartThings "Alarm" device capability.
//			  It inherits from the st::PollingSensor class.  The current version implements the Siren as
//			  a digital output (uses a relay to switch the Siren on or off)
//
//			  TODO:  Possibly add Strobe capability in the future
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Alarm executor2("alarm1", PIN_ALARM, LOW, true, PIN_STROBE);
//
//			  st::EX_Alarm() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//				- byte pinStrobe - OPTOINAL - If supplied, will allow separate SIREN and STROBE outputs
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//	  2017-04-20  Dan Ogorchock	 Add optional Strobe functionality
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#ifndef ST_EX_Alarm_H
#define ST_EX_Alarm_H

#include "Executor.h"

enum Alarm_States { off, both, siren, strobe};

namespace st
{
	class EX_Alarm : public Executor
	{

	private:
		
		bool m_bInvertLogic;
		byte m_nPin;
		byte m_nPinStrobe;
		bool m_bUseStrobe;
		Alarm_States m_nCurrentAlarmState;

		void writeStateToPin();

	public:
		//constructor
		EX_Alarm(const __FlashStringHelper *name, byte Pin, bool startingState = LOW, bool invertLogic = false, byte pinStrobe = 255);

		//destructor
		virtual ~EX_Alarm();

		//initialization function
		virtual void init();

		//called periodically to ensure state of the Alarm Siren is up to date in the SmartThings Cloud (in case an event is missed)
		virtual void refresh();

		//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
		virtual void beSmart(const String &str);

		//gets
		virtual byte getPin() const { return m_nPin; }
		virtual byte getStrobePin() const { return m_nPinStrobe; }

		//sets
		virtual void setPin(byte pin);

	};
}



#endif