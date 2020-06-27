//******************************************************************************************
//  File: IS_LatchingRelaySwitch.h
//  Authors: Dan G Ogorchock
//
//  Summary:  IS_LatchingRelaySwitch is a class which implements the "Valve" device capability, where output1 opens a valve, and
//            output2 closes a valve.  It features optional automatic-turn-off time delay times for for both outputs.
//
//			  It inherits from the st::InterruptSensor class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_LatchingRelaySwitch sensor1(F("switch1"), PIN_INPUT, HIGH, true, 500, PIN_RELAY1, PIN_RELAY2, LOW, true, 1000, 1000);
//
//			  st::IS_LatchingRelaySwitch() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinInput - REQUIRED - the Arduino Pin to be used as a digital input to determine switch the state of latching relay
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - REQUIRED - true == INTERNAL_PULLUP
//				- long numReqCounts - REQUIRED - number of counts before changing state of input (prevent false alarms)
//				- byte pinOutput1 - REQUIRED - the Arduino Pin to be used as a digital output
//				- byte pinOutput2 - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "closed", HIGH = "open"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Output should use inverted logic (e.g. active high versus active low relays)
//				- long Output1Time - REQUIRED - the number of milliseconds to keep the output1 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//				- long Output2Time - REQUIRED - the number of milliseconds to keep the output2 on, DEFAULTS to 1000 milliseconds, 0 = will stay on
//              - bool initializeOutputs - OPTIONAL - determines if the digital outputs are activated during initialization/startup, defaults to 'false'
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2020-06-26  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_IS_LatchingRelaySwitch_H
#define ST_IS_LatchingRelaySwitch_H

#include "InterruptSensor.h"

namespace st
{
	class IS_LatchingRelaySwitch : public InterruptSensor  //inherits from parent InterruptSensor Class
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
			IS_LatchingRelaySwitch(const __FlashStringHelper *name, byte pinInput, bool iState, bool internalPullup, long numReqCounts, byte pinOutput1, byte pinOutput2, bool startingState = LOW, bool invertLogic = false, unsigned long Output1Time = 1000, unsigned long Output2Time = 1000, bool initializeOutputs = false);
			
			//destructor
			virtual ~IS_LatchingRelaySwitch();
			
			//initialization function
			virtual void init();

			//update function 
			void update();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch (digital output)
			virtual void beSmart(const String &str);

			//called periodically by Everything class to ensure ST Cloud is kept consistent with the state of the contact sensor
			virtual void refresh();

			//handles what to do when interrupt is triggered 
			virtual void runInterrupt();

			//handles what to do when interrupt is ended 
			virtual void runInterruptEnded();

			//gets
			//virtual byte getPin() const { return m_nOutputPin; }
			virtual bool getTimerActive() const { return m_bTimerPending; }
			virtual bool getStatus() const { return m_bCurrentState; }

	};
}


#endif
