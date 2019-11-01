//******************************************************************************************
//  File: EX_TimedRelayPair.h
//  Authors: Dan G Ogorchock
//
//  Summary:  EX_TimedRelayPair is a class which implements the "Valve" device capability, where output1 opens a valve, and
//            output2 closes a valve.  It features optional automatic-turn-off time delay times for for both outputs.
//
//			  It inherits from the st::Executor class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_TimedRelayPair executor1(F("valve1"), PIN_RELAY1, PIN_RELAY2, LOW, true, 1000, 1000);
//
//			  st::EX_TimedRelayPair() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinOutput1 - REQUIRED - the Arduino Pin to be used as a digital output
//				- byte pinOutput2 - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "closed", HIGH = "open"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Output should use inverted logic (e.g. active high versus active low relays)
//				- long Output1Time - REQUIRED - the number of milliseconds to keep the output1 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//				- long Output2Time - REQUIRED - the number of milliseconds to keep the output2 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-10-30  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_EX_TimedRelayPair_H
#define ST_EX_TimedRelayPair_H

#include "Executor.h"

namespace st
{
	class EX_TimedRelayPair : public Executor  //inherits from parent Executor Class
	{
		private:
			
			//following are for the digital output
			bool m_bCurrentState;	        //HIGH or LOW
			bool m_bInvertLogic;	        //determines whether the Arduino Digital Output should use inverted logic
			byte m_nOutputPin1;		        //Arduino Pin used as a Digital Output for the switch - often connected to a relay or an LED
			byte m_nOutputPin2;		        //Arduino Pin used as a Digital Output for the switch - often connected to a relay or an LED
			unsigned long m_lOutput1Time;	//number of milliseconds to keep digital output HIGH before automatically turning off
			unsigned long m_lOutput2Time;	//number of milliseconds to keep digital output HIGH before automatically turning off
			unsigned long m_lTimeChanged;	//time when the digital output was last changed
			bool m_bTimerPending;		    //true if waiting on relay timer to expire

			void writeStateToPin(byte, bool);	//function to update the Arduino Digital Output Pin
			
		public:
			//constructor - called in your sketch's global variable declaration section
			EX_TimedRelayPair(const __FlashStringHelper *name, byte pinOutput1, byte pinOutput2, bool startingState = LOW, bool invertLogic = false, unsigned long Output1Time = 1000, unsigned long Output2Time = 1000);
			
			//destructor
			virtual ~EX_TimedRelayPair();
			
			//initialization function
			virtual void init();

			//update function 
			void update();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output)
			virtual void beSmart(const String &str);

			//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
			virtual void refresh();

			//gets
			//virtual byte getPin() const { return m_nOutputPin; }
			virtual bool getTimerActive() const { return m_bTimerPending; }
			virtual bool getStatus() const { return m_bCurrentState; }

			//sets
			//virtual void setOutputPin(byte pin);
	};
}


#endif
