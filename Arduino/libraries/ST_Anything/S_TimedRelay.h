//******************************************************************************************
//  File: S_TimedRelay.h
//  Authors: Dan G Ogorchock
//
//  Summary:  S_TimedRelay is a class which implements the SmartThings "Relay" device capability.  It features
//			  an automatic-turn-off time delay for a relay to emulate a button press.
//
//			  It inherits from the st::Sensor class and clones much from the st::Executor Class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::S_TimedRelay sensor1("relaySwitch1", PIN_RELAY, LOW, true, 1000, 0, 1);
//
//			  st::S_TimedRelay() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Ouput should use inverted logic
//				- long onTime - REQUIRED - the number of milliseconds to keep the output on, DEFGAULTS to 1000 milliseconds
//				- long offTime - OPTIONAL - the number of milliseconds to keep the output off, DEFAULTS to 0
//				- intnumCycles - OPTIONAL - the number of times to repeat the on/off cycle, DEFAULTS to 1
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-12-29  Dan Ogorchock  Original Creation
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#ifndef ST_S_TIMEDRELAY_H
#define ST_S_TIMEDRELAY_H

#include "Sensor.h"

namespace st
{
	class S_TimedRelay : public Sensor  //inherits from parent Sensor Class
	{
		private:
			
			//following are for the digital output
			bool m_bCurrentState;	//HIGH or LOW
			bool m_bInvertLogic;	//determines whether the Arduino Digital Output should use inverted logic
			byte m_nOutputPin;		//Arduino Pin used as a Digital Output for the switch - often connected to a relay or an LED
			unsigned long m_lOnTime;		//number of milliseconds to keep digital output HIGH before automatically turning off
			unsigned long m_lOffTime;		//number of milliseconds to keep digital output LOW before automatically turning on
			unsigned int m_iNumCycles;		//number of on/off cycles of the digital output 
			unsigned int m_iCurrentCount;	//current number of on/off cycles of the digital output
			unsigned long m_lTimeChanged;	//time when the digital output was last changed
			bool m_bTimerPending;		//true if waiting on relay timer to expire

			void writeStateToPin();	//function to update the Arduino Digital Output Pin
			
		public:
			//constructor - called in your sketch's global variable declaration section
			S_TimedRelay(const __FlashStringHelper *name, byte pinOutput, bool startingState = LOW, bool invertLogic = false, unsigned long onTime = 1000, unsigned long offTime = 0, unsigned int numCycles = 1);
			
			//destructor
			virtual ~S_TimedRelay();
			
			//initialization function
			virtual void init();

			//update function 
			void update();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output)
			virtual void beSmart(const String &str);

			//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
			virtual void refresh();

			//gets
			virtual byte getPin() const { return m_nOutputPin; }
			virtual bool getTimerActive() const { return m_bTimerPending; }

			//sets
			virtual void setOutputPin(byte pin);
	};
}


#endif