//******************************************************************************************
//  File: EX_RGBW_NeoPixelBus.h
//  Authors: Allan (vseven) based on EX_Switch_Dim by Dan G Ogorchock
//
//  Summary:  EX_RGBW_NeoPixelBus is a class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RGBW_NeoPixelBus executor1("rgbSwitch1", PIXEL_COUNT, PIN_RGB);
//
//			  st::EX_RGBW_NeoPixelBus() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name.
//				- uint16_t pixelCount - REQUIRED - the number of programmable pixels in the RGB strip
//				- uint8_t pinRGB - REQUIRED - the pin to use for data writes
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
#include "EX_RGBW_NeoPixelBus.h"

#include "Constants.h"
#include "Everything.h"

#include <NeoPixelBus.h>

namespace st
{

//private
	void EX_RGBW_NeoPixelBus::writeCommandToOutput()
	{
		uint8_t R;
		uint8_t G;
		uint8_t B;
		uint8_t W;

		if (st::Executor::debug) {
			Serial.println("m_bCurrentState: " + String(m_bCurrentState));
			Serial.println("m_sCurrentHEX: " + String(m_sCurrentHEX));
		}


		if (m_bCurrentState == HIGH){
			// Our status is on so get the RGB value from the hex
			String hexstring = m_sCurrentHEX;
			long number = (long) strtol( &hexstring[1], NULL, 16);
      			// Split them up into r, g, b values
      			R = number >> 24;
      			G = number >> 16 & 0xFF;
      			B = number >> 8 & 0xFF;
			W = number & 0xFF;
		} else {
			// Status is off so turn off LED
			R = 00;
      			G = 00;
      			B = 00;
      			W = 00;
		}
		
		// Any adjustments to the colors can be done here before sending the commands.  For example if red is always too bright reduce it:
		// R = R * 0.95

		// Write to our output

		if (st::Executor::debug) {
			Serial.print(F("subString R:G:B:W = "));
			Serial.println(String(R) + ":" + String(G) + ":" + String(B) + ":" + String(W));
		}

		// set all pixels to the same color

		RgbwColor myColor = RgbwColor(R, G, B, W);


		if (st::Executor::debug) {
			Serial.print(F("Total number of pixels reported by strip: "));
			Serial.println(String(m_nStrip.PixelCount()));
			//Serial.print(F("Color being sent: "));
			//Serial.println(String(myColor.r + myColor.g + myColor.b));
		}

		m_nStrip.ClearTo(myColor);

		if (st::Executor::debug) {
			Serial.println(F("All pixels set.  Sending show command."));
		}

		// then display it
		m_nStrip.Show();
	}

//public
	//constructor
	EX_RGBW_NeoPixelBus::EX_RGBW_NeoPixelBus(const __FlashStringHelper *name, uint16_t count, uint8_t pin):
		Executor(name),
		m_nStrip(count,pin)
	{

	}

	//destructor
	EX_RGBW_NeoPixelBus::~EX_RGBW_NeoPixelBus()
	{
	
	}
	
	void EX_RGBW_NeoPixelBus::init()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
		
		if (st::Executor::debug) {
			Serial.println("init called. Sending Begin to strip.");
		}
		// On startup make sure strip is turned off
  		m_nStrip.Begin();
		m_nStrip.Show();
	}

	void EX_RGBW_NeoPixelBus::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if (st::Executor::debug) {
			Serial.print(F("EX_RGBW_NeoPixelBus::beSmart s = "));
			Serial.println(s);
		}
		if(s==F("on"))
		{
			m_bCurrentState=HIGH;
		}
		else if(s==F("off"))
		{
			m_bCurrentState=LOW;
		}
		else //must be a set color command
		{
			s.trim();
			m_sCurrentHEX = s;
		}

		writeCommandToOutput();

		//Send data to SmartThings/Hubitat
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_RGBW_NeoPixelBus::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
}
