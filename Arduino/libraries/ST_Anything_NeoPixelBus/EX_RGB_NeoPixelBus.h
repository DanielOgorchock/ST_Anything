//******************************************************************************************
//  File: EX_RGB_NeoPixelBus.h
//  Authors: Allan (vseven) based on EX_Switch_Dim by Dan G Ogorchock
//
//  Summary:  EX_RGB_NeoPixelBus is a class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RGB_NeoPixelBus executor1("rgbSwitch1", PIXEL_COUNT, PIN_RGB);
//
//			  st::EX_RGB_NeoPixelBus() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name.
//				- uint16_t pixelCount - REQUIRED - the number of programmable pixels in the RGB strip
//				- uint8_t pinRGB- REQUIRED - the pin to use for data writes
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2016-04-30  Dan Ogorchock  Original Creation
//    2018-08-14  Dan Ogorchock  Modified to avoid compiler errors on ESP32 since it currently does not support "analogWrite()"
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2017-10-06  Allan (vseven) Modified original code from EX_Switch_Dim to be used for RGB lighting
//    2020-08-01  Allan (vseven) Modified the EX_RGB_Dim for use with addressable LED strips using the NeoPixelBus library
//
//******************************************************************************************
#ifndef ST_EX_RGB_NeoPixelBus
#define ST_EX_RGB_NeoPixelBus

#include "Executor.h"

#include <NeoPixelBus.h>

namespace st
{
	class EX_RGB_NeoPixelBus: public Executor
	{
		private:
			bool m_bCurrentState;		//HIGH or LOW
			String m_sCurrentHEX;		//HEX value of color currently set

			void writeCommandToOutput();	//function to update the Arduino Digital Output Pin

			// NeoPixelBus object.  Modify for the correct parameter list: https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object
			NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> m_nStrip;		
	public:
			//constructor - called in your sketch's global variable declaration section
			EX_RGB_NeoPixelBus(const __FlashStringHelper *name, uint16_t pixelCount, uint8_t pinRGB);
			
			//destructor
			virtual ~EX_RGB_NeoPixelBus();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch along with HEX value for LEDs)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual bool getStatus() const { return m_bCurrentState; } //whether the switch is HIGH or LOW
			virtual String getHEX() const { return m_sCurrentHEX; }	// color value in HEX

			//sets
	};
}

#endif
