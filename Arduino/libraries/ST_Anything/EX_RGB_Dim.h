	//******************************************************************************************
//  File: EX_RGB_Dim.h
//  Authors: Allan (vseven) based on EX_Switch_Dim by Dan G Ogorchock
//
//  Summary:  EX_RGB_Dim is a class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RGB_Dim executor1("rgbSwitch1", PIN_R, PIN_G, PIN_B, true, 0, 1, 2);
//
//			  st::EX_RGB_Dim() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name.
//				- byte pin_r - REQUIRED - the Arduino Pin to be used as a digital output for Red.
//				- byte pin_g - REQUIRED - the Arduino Pin to be used as a digital output for Green.
//				- byte pin_b - REQUIRED - the Arduino Pin to be used as a digital output for Blue.
//				- bool commonAnode - REQUIRED - determines whether the LED uses a common Anode or Cathode.  True for Anode.
//				- byte channel_r - OPTIONAL - PWM channel used for Red on a ESP32.
//				- byte channel_g - OPTIONAL - PWM channel used for Green on a ESP32.
//				- byte channel_b - OPTIONAL - PWM channel used for Blue on a ESP32.
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2016-04-30  Dan Ogorchock  Original Creation
//    2018-08-14  Dan Ogorchock  Modified to avoid compiler errors on ESP32 since it currently does not support "analogWrite()"
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2017-10-06  Allan (vseven) Modified original code from EX_Switch_Dim to be used for RGB lighting
//
//******************************************************************************************
#ifndef ST_EX_RGB_DIM
#define ST_EX_RGB_DIM

#include "Executor.h"

namespace st
{
	class EX_RGB_Dim: public Executor
	{
		private:
			bool m_bCurrentState;	//HIGH or LOW
			bool m_bCommonAnode;	//TRUE or FALSE
			byte m_nPinR;		//Arduino Pin used as a PWM Output for Red
			byte m_nPinG;		//Arduino Pin used as a PWM Output for Green
			byte m_nPinB;		//Arduino Pin used as a PWM Output for Blue
			byte m_nChannelR;	//PWM Channel used for Red output
			byte m_nChannelG;	//PWM Channel used for Green output
			byte m_nChannelB;	//PWM Channel used for Blue output
			String m_sCurrentHEX;	//HEX value of color currently set

			void writeRGBToPins();	//function to update the Arduino PWM Output Pins

		public:
			//constructor - called in your sketch's global variable declaration section
			EX_RGB_Dim(const __FlashStringHelper *name, byte pinR, byte pinG, byte pinB, bool commonAnode, byte channelR = 0, byte channelG = 0, byte channelB = 0);
			
			//destructor
			virtual ~EX_RGB_Dim();

			//initialization routine
			virtual void init();

			//SmartThings Shield data handler (receives command to turn "on" or "off" the switch along with HEX value for LEDs)
			virtual void beSmart(const String &str);
			
			//called periodically to ensure state of the switch is up to date in the SmartThings Cloud (in case an event is missed)
			virtual void refresh();
			
			//gets
			virtual byte getRedPin() const { return m_nPinR; }
			virtual byte getGreenPin() const { return m_nPinG; }
			virtual byte getBluePin() const { return m_nPinB; }

			virtual byte getRedChannel() const { return m_nChannelR; }
			virtual byte getGreenChannel() const { return m_nChannelG; }
			virtual byte getBlueChannel() const { return m_nChannelB; }

			virtual bool getStatus() const { return m_bCurrentState; } //whether the switch is HIGH or LOW
			virtual String getHEX() const { return m_sCurrentHEX; }	// color value in HEX

			//sets
			virtual void setRedPin(byte pin,byte channel);
			virtual void setGreenPin(byte pin,byte channel);
			virtual void setBluePin(byte pin,byte channel);
		
	};
}

#endif
