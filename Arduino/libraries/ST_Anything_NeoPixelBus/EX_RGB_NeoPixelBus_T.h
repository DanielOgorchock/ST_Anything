//******************************************************************************************
//  File: EX_RGB_NeoPixelBus_T.h
//  Authors: Allan (vseven) based on EX_Switch_Dim by Dan G Ogorchock
//
//  Summary:  EX_RGB_NeoPixelBus_T is a templated class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RGB_NeoPixelBus_T<FEATURE, METHOD> executor1("rgbSwitch1", PIXEL_COUNT, OUTPUT_PIN);
//			  
//			  Each strip can have a different LED feature (the RGB order) and method for writting the data.
//			  See https://github.com/Makuna/NeoPixelBus/wiki/ESP32-NeoMethods for the full list.  For example on a ESP32 you 
//			  can call up to 8 different strips by using the RTM method as such:
//
//			  st::EX_RGB_NeoPixelBus_T<NeoGrbFeature, NeoEsp32Rmt1800KbpsMethod> executor1("rgbSwitch1", 60, 25);
//			  st::EX_RGB_NeoPixelBus_T<NeoRgbFeature, NeoEsp32Rmt2800KbpsMethod> executor2("rgbSwitch2", 150, 26);
//			  st::EX_RGB_NeoPixelBus_T<NeoGrbFeature, NeoEsp32Rmt3800KbpsMethod> executor3("rgbSwitch3", 30, 27);
//
//			  st::EX_RGB_NeoPixelBus_T() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name.
//				- uint16_t pixelCount - REQUIRED - the number of programmable pixels in the RGB strip
//				- uint8_t outputPIN1 - REQUIRED - the primary pin to use for data writes
//				- uint8_t outputPIN2 - OPTIONAL - for methods that require two pins, like DotStar strips, this will be the second pin
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
//    2020-08-14  Allan (vseven) Converted the library into a template library to allow multiple different LED strips
//
//******************************************************************************************
#ifndef ST_EX_RGB_NeoPixelBus_T
#define ST_EX_RGB_NeoPixelBus_T

#include "Executor.h"
#include "Constants.h"
#include "Everything.h"

#include "NeoPixelBus.h"

namespace st
{
	template<typename T_COLOR_FEATURE, typename T_METHOD>
	class EX_RGB_NeoPixelBus_T: public Executor
	{
		private:
			bool m_bCurrentState;			//HIGH or LOW
			String m_sCurrentHEX = "#000000";	//HEX value of color currently set

			// NeoPixelBus object.
			NeoPixelBus<T_COLOR_FEATURE, T_METHOD> m_nStrip;

			void writeCommandToOutput()
			{
				uint8_t R = 0;
				uint8_t G = 0;
				uint8_t B = 0;

				if (st::Executor::debug) {
					Serial.println("m_bCurrentState: " + String(m_bCurrentState));
					Serial.println("m_sCurrentHEX: " + String(m_sCurrentHEX));
				}

				if (m_bCurrentState == HIGH){
					// Our status is on so get the RGB value from the hex
					String hexstring = m_sCurrentHEX;
					long number = (long) strtol( &hexstring[1], NULL, 16);
		      			// Split them up into r, g, and b values
	      				R = number >> 16;
	      				G = number >> 8 & 0xFF;
	      				B = number & 0xFF;

				} else {
					// Status is off so turn off LED
					R = 0;
      					G = 0;
	      				B = 0;
				}
			
				if (st::Executor::debug) {
					Serial.print(F("subString R:G:B = "));
					Serial.println(String(R) + ":" + String(G) + ":" + String(B));
					Serial.print(F("Total number of pixels reported by strip: "));
					Serial.println(String(m_nStrip.PixelCount()));
				}


				// Write to our output
		      		RgbColor myColor = RgbColor(R, G, B);
				m_nStrip.ClearTo(myColor);

				if (st::Executor::debug) {
					Serial.println(F("All pixels set.  Sending show command."));
				}

				// then display it
				m_nStrip.Show();
			}
	
		public:
			//constructor - called in your sketch's global variable declaration section
			EX_RGB_NeoPixelBus_T(const __FlashStringHelper *name, uint16_t pixelCount, uint8_t outputPin1):
				Executor(name),
				m_nStrip(pixelCount,outputPin1)
			{

			}

			EX_RGB_NeoPixelBus_T(const __FlashStringHelper *name, uint16_t pixelCount, uint8_t outputPin1, uint8_t outputPin2):
				Executor(name),
				m_nStrip(pixelCount,outputPin1,outputPin2)
			{

			}

			//destructor
			~EX_RGB_NeoPixelBus_T()
			{
			
			}
			
			//gets
			bool getStatus() const { return m_bCurrentState; } //whether the switch is HIGH or LOW
			String getHEX() const { return m_sCurrentHEX; }	// color value in HEX

			//sets

			//initialize
			void init()
			{
				Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	
				if (st::Executor::debug) {
					Serial.println("EX_RGB_NeoPixelBus_T - " + getName() + " init called. Sending Begin to RGB strip to clear it out.");
				}
				// On startup make sure strip is turned off
  				m_nStrip.Begin();
				m_nStrip.Show();
			}

			//functions
			void beSmart(const String &str)
			{
				String s=str.substring(str.indexOf(' ')+1);
				if (st::Executor::debug) {
					Serial.print(F("EX_RGB_NeoPixelBus_T::beSmart s = "));
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

			void refresh()
			{
				Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
			}
	};
}
#endif
