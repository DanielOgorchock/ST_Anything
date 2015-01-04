//******************************************************************************************
//  File: PS_Alarm.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Alarm is a class which implements the SmartThings "Alarm" device capability.
//			  It inherits from the st::PollingSensor class.  The current version implements the Siren as
//			  a digital output (uses a relay to switch the Siren on or off)
//
//			  TODO:  Should probably be a st::Executor
//			  TODO:  Possibly add Strobe capability in the future
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Alarm sensor4("alarm", 60000, 9000, PIN_ALARM, LOW, true);
//
//			  st::EX_Switch() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in milliseconds
//				- long offset - REQUIRED - the polling interval offset in milliseconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Ouput should use inverted logic
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#ifndef ST_PS_ALARM_H
#define ST_PS_ALARM_H

#include "PollingSensor.h"

namespace st
{
	class PS_Alarm: public PollingSensor
	{

	private:
		
		bool m_bCurrentState; //HIGH or LOW
		bool m_bInvertLogic;
		byte m_nPin;


		void writeStateToPin();

	public:
		//constructor
		PS_Alarm(const String &name, long interval, long offset, byte Pin, bool startingState = LOW, bool invertLogic = false);

		//destructor
		virtual ~PS_Alarm();

		//initialization function
		virtual void init();

		//function to get status of the Alarm Siren and queue results for transfer to ST Cloud 
		virtual void getData();

		//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
		virtual void beSmart(const String &str);

		//gets
		virtual byte getPin() const { return m_nPin; }

		//sets
		virtual void setPin(byte pin);

	
	};
}



#endif