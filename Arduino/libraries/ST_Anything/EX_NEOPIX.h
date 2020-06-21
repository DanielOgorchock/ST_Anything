//******************************************************************************************
//  File: EX_NEOPIX.cpp
//  Authors: Justin Eltoft based on EX_RGB_Dim by Dan G Ogorchock
//
//  Summary:  EX_NEOPIX is a class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//            It inherits from the st::Executor class.
//
//            Create an instance of this class in your sketch's global variable section
//            For Example:  st::EX_NEOPIX executor1("rgbSwitch1", PIN_NEOPIX);
//
//            st::EX_NEOPIX() constructor requires the following arguments
//              - String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//              - byte pin_neopix - REQUIRED - the Arduino Pin to be used as the one wire bus for neopixels
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2020-06-20  Justin Eltoft  Original Creation
//
//******************************************************************************************
#ifndef ST_EX_NEOPIX
#define ST_EX_NEOPIX

#include <Adafruit_NeoPixel.h>
#include "Executor.h"

namespace st
{
	class EX_NEOPIX: public Executor
	{
		private:
            Adafruit_NeoPixel *m_pixel;
			bool m_bCurrentState;	//HIGH or LOW
			byte m_nPinNeoPix;		//Arduino Pin used for Neopixel
			String m_sCurrentHEX;	//HEX value of color currently set

			void writeRGBToPins();	//function to update the Arduino PWM Output Pins

		public:
			//constructor - called in your sketch's global variable declaration section
			EX_NEOPIX(const __FlashStringHelper *name, byte pinNeoPix);
			
			//destructor
			virtual ~EX_NEOPIX();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch along with HEX value for LEDs)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getPin() const { return m_nPinNeoPix; }

			virtual bool getStatus() const { return m_bCurrentState; } //whether the switch is HIGH or LOW
			virtual String getHEX() const { return m_sCurrentHEX; }	// color value in HEX

			//sets
			virtual void setPin(byte pin);
	};
}

#endif
