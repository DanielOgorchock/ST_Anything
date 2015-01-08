//******************************************************************************************
//  File: IS_ContactRelay.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  IS_ContactRelay is a class which implements the SmartThings "Contact Sensor" device capability as well
//			  as a "Switch" device capability with a an automatic-turn-off time delay.  This is useful for controlling
//			  a garage door.  Use the input to monitor a magnetic contact sensor.  Use the output to control a relay to
//			  "press the garage door button" to open/close the garage door.
//
//			  It inherits from the st::InterruptSensor class and clones most of the Executor Class
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::IS_ContactRelay sensor6("contact", PIN_CONTACT, HIGH, true);
//
//			  st::IS_ContactRelay() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pinInput - REQUIRED - the Arduino Pin to be used as a digital input
//				- bool iState - REQUIRED - LOW or HIGH - determines which value indicates the interrupt is true
//				- bool internalPullup - REQUIRED - true == INTERNAL_PULLUP
//				- byte pinOutput - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - REQUIRED - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - REQUIRED - determines whether the Arduino Digital Ouput should use inverted logic
//				- long delayTime - REQUIRED - the number of milliseconds to keep the output on
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-07  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_IS_CONTACTRELAY_H
#define ST_IS_CONTACTRELAY_H

#include "InterruptSensor.h"

namespace st
{
	class IS_ContactRelay : public InterruptSensor
	{
		private:
			//inherits everything necessary from parent InterruptSensor Class for the Contact Sensor

			//following are for the digital output
			bool m_bCurrentState;	//HIGH or LOW
			bool m_bInvertLogic;	//determines whether the Arduino Digital Ouput should use inverted logic
			byte m_nOutputPin;		//Arduino Pin used as a Digital Output for the switch - often connected to a relay or an LED
			unsigned long m_lDelayTime;		//number of milliseconds to keep digital output active before automatically turning off
			unsigned long m_lTimeTurnedOn;	//time when the digital output was turned on

			void writeStateToPin();	//function to update the Arduino Digital Output Pin

			
		public:
			//constructor - called in your sketch's global variable declaration section
			IS_ContactRelay(const String &name, byte pinInput, bool iState, bool pullup, byte pinOutput, bool startingState, bool invertLogic, unsigned long delayTime);
			
			//destructor
			virtual ~IS_ContactRelay();
			
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
			virtual byte getPin() const { return m_nOutputPin; }

			//sets
			virtual void setOutputPin(byte pin);
	};
}


#endif