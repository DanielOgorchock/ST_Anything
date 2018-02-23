//******************************************************************************************
//  File: EX_RGBW_Dim.h
//  Authors: Allan (vseven) based on EX_Switch_Dim by Dan G Ogorchock
//
//  Summary:  EX_RGBW_Dim is a class which implements the SmartThings "Color Control", "Switch", and "Switch Level" device capabilities.
//			  It inherits from the st::Executor class.
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_RGBW_Dim executor1("rgbwSwitch1", PIN_R, PIN_G, PIN_B, PIN_W, true, 0, 1, 2, 3);
//
//			  st::EX_RGBW_Dim() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin_r - REQUIRED - the Arduino Pin to be used as a digital output for Red.
//				- byte pin_g - REQUIRED - the Arduino Pin to be used as a digital output for Green.
//				- byte pin_b - REQUIRED - the Arduino Pin to be used as a digital output for Blue.
//				- byte pin_w - REQUIRED - the Arduino Pin to be used as a digital output for White.
//				- bool commonAnode - REQUIRED - determines whether the LED uses a common Anode or Cathode.  True for Anode.
//				- byte channel_r - OPTIONAL - PWM channel used for Red on a ESP32.
//				- byte channel_g - OPTIONAL - PWM channel used for Green on a ESP32.
//				- byte channel_b - OPTIONAL - PWM channel used for Blue on a ESP32.
//				- byte channel_w - OPTIONAL - PWM channel used for Whitw on a ESP32.

//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2016-04-30  Dan Ogorchock  Original Creation
//    2018-08-14  Dan Ogorchock  Modified to avoid compiler errors on ESP32 since it currently does not support "analogWrite()"
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2017-10-08  Allan (vseven) Modified original code from EX_RGBW_Dim to be used for RGB lighting
//    2017-10-12  Allan (vseven) Modified EX_RGBW_Dim for support of a White LEd channel
//
//******************************************************************************************
#include "EX_RGBW_Dim.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	void EX_RGBW_Dim::writeRGBWToPins()
	{
	int subStringR;
	int subStringG;
	int subStringB;
	int subStringW;

	if (m_bCurrentState == HIGH) {
		// Our status is on so get the RGBW value from the hex
		String hexstring = m_sCurrentHEX;
		long number = (long) strtol( &hexstring[1], NULL, 16);
      		// Split them up into r, g, b, w values
      		subStringR = number >> 24;
      		subStringG = number >> 16 & 0xFF;
      		subStringB = number >> 8 & 0xFF;
		subStringW = number & 0xFF;
	} else {
		// Status is off so turn off LED
		subStringR = 00;
      		subStringG = 00;
      		subStringB = 00;
      		subStringW = 00;
	}
		
      	if(m_bCommonAnode) {
        	// A hex value of 00 will translate to 255 for a common anode.  However the  
        	// ledcWrite seems to need a 256 to turn off so we are adding one here.
        	subStringR = 255 - subStringR + 1;
        	subStringG = 255 - subStringG + 1;
        	subStringB = 255 - subStringB + 1;
        	subStringW = 255 - subStringW + 1;
      	} 
	// Write to outputs.  Use ledc for ESP32, analogWrite for everything else.

	if (st::Executor::debug) {
		Serial.print(F("subString R:G:B:W = "));
		Serial.println(String(subStringR) + ":" + String(subStringG) + ":" + String(subStringB) + ":" + String(subStringW));
	}

	// Any adjustments to the colors can be done here before sending the commands.  For example if red is always too bright reduce it:
	// subStringR = subStringR * 0.95

	#if defined(ARDUINO_ARCH_ESP32)
		ledcWrite(m_nChannelR, subStringR);
	#else
		analogWrite(m_nPinR, subStringR);
	#endif

	#if defined(ARDUINO_ARCH_ESP32)
		ledcWrite(m_nChannelG, subStringG);
	#else
		analogWrite(m_nPinG, subStringG);
	#endif

	#if defined(ARDUINO_ARCH_ESP32)
		ledcWrite(m_nChannelB, subStringB);
	#else
		analogWrite(m_nPinB, subStringB);
	#endif

	#if defined(ARDUINO_ARCH_ESP32)
		ledcWrite(m_nChannelW, subStringW);
	#else
		analogWrite(m_nPinW, subStringW);
	#endif

	}

//public
	//constructor
	EX_RGBW_Dim::EX_RGBW_Dim(const __FlashStringHelper *name, byte pinR, byte pinG, byte pinB, byte pinW, bool commonAnode, byte channelR, byte channelG, byte channelB, byte channelW):
		Executor(name),
		m_bCommonAnode(commonAnode)
	{
		setRedPin(pinR, channelR);
		setGreenPin(pinG, channelG);
		setBluePin(pinB, channelB);
		setWhitePin(pinB, channelW);
	}

	//destructor
	EX_RGBW_Dim::~EX_RGBW_Dim()
	{
	
	}
	
	void EX_RGBW_Dim::init()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
	}

	void EX_RGBW_Dim::beSmart(const String &str)
	{
		String s=str.substring(str.indexOf(' ')+1);
		if (st::Executor::debug) {
			Serial.print(F("EX_RGBW_Dim::beSmart s = "));
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

		writeRGBWToPins();

		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_RGBW_Dim::refresh()
	{
		Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
	}
	
	void EX_RGBW_Dim::setRedPin(byte pin, byte channel)
	{
		m_nPinR = pin;
		m_nChannelR = channel;

		#if defined(ARDUINO_ARCH_ESP32)
			ledcAttachPin(m_nPinR, m_nChannelR);
  			ledcSetup(m_nChannelR, 5000, 8);
		#else
			pinMode(m_nPinR, OUTPUT);
		#endif
	}
	void EX_RGBW_Dim::setGreenPin(byte pin, byte channel)
	{
		m_nPinG = pin;
		m_nChannelG = channel;

		#if defined(ARDUINO_ARCH_ESP32)
			ledcAttachPin(m_nPinG, m_nChannelG);
  			ledcSetup(m_nChannelG, 5000, 8);
		#else
			pinMode(m_nPinG, OUTPUT);
		#endif
	}
	void EX_RGBW_Dim::setBluePin(byte pin, byte channel)
	{
		m_nPinB = pin;
		m_nChannelB = channel;

		#if defined(ARDUINO_ARCH_ESP32)
			ledcAttachPin(m_nPinB, m_nChannelB);
  			ledcSetup(m_nChannelB, 5000, 8);
		#else
			pinMode(m_nPinB, OUTPUT);
		#endif
	}
	void EX_RGBW_Dim::setWhitePin(byte pin, byte channel)
	{
		m_nPinW = pin;
		m_nChannelW = channel;

		#if defined(ARDUINO_ARCH_ESP32)
			ledcAttachPin(m_nPinW, m_nChannelW);
  			ledcSetup(m_nChannelW, 5000, 8);
		#else
			pinMode(m_nPinW, OUTPUT);
		#endif
	}

}
