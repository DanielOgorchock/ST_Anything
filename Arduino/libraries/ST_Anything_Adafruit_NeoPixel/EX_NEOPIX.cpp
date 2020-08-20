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
#include "EX_NEOPIX.h"
#include <Adafruit_NeoPixel.h>

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
    void EX_NEOPIX::writeRGBToPins()
    {
        int subStringR;
        int subStringG;
        int subStringB;

        if (m_bCurrentState == HIGH)
        {
            // Our status is on so get the RGB value from the hex
            String hexstring = m_sCurrentHEX;
            long number = (long) strtol( &hexstring[1], NULL, 16);
            // Split them up into r, g, b values
            subStringR = number >> 16;
            subStringG = number >> 8 & 0xFF;
            subStringB = number & 0xFF;
        }
        else
        {
            // Status is off so turn off LED
            subStringR = 00;
            subStringG = 00;
            subStringB = 00;
        }

        // Write to outputs.  Use ledc for ESP32, analogWrite for everything else.
        if (st::Executor::debug) {
            Serial.print(F("subString R:G:B = "));
            Serial.println(String(subStringR) + ":" + String(subStringG) + ":" + String(subStringB));
        }

        // Any adjustments to the colors can be done here before sending the commands.  For example if red is always too bright reduce it:
        // subStringR = subStringR * 0.95

        m_pixel->setPixelColor(0, m_pixel->Color(subStringR, subStringG, subStringB));
        m_pixel->show();
    }

//public
    //constructor
    EX_NEOPIX::EX_NEOPIX(const __FlashStringHelper *name, byte pinNeoPix):
        Executor(name)
    {
        setPin(pinNeoPix);
    }

    //destructor
    EX_NEOPIX::~EX_NEOPIX()
    {
    
    }
    
    void EX_NEOPIX::init()
    {
        Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH ? F("on") : F("off")));
    }

    void EX_NEOPIX::beSmart(const String &str)
    {
        String s=str.substring(str.indexOf(' ')+1);
        if (st::Executor::debug) {
            Serial.print(F("EX_NEOPIX::beSmart s = "));
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

        writeRGBToPins();

        Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
    }
    
    void EX_NEOPIX::refresh()
    {
        Everything::sendSmartString(getName() + " " + (m_bCurrentState == HIGH?F("on"):F("off")));
    }
    
    void EX_NEOPIX::setPin(byte pin)
    {
        m_nPinNeoPix = pin;

        m_pixel = new Adafruit_NeoPixel(1, pin, NEO_GRB + NEO_KHZ800);
        m_pixel->begin();

        m_pixel->clear();
        m_pixel->show();

    }
}
