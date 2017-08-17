//******************************************************************************************
//  File: Device.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::Device is the highest level generic class for either a st::Sensor or 
//			  st::Executor subclass.  
//			  In general, this file should not need to be modified.   
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2018-08-15  Dan Ogorchock  Workaround for strcpy_P() ESP32 crash bug
//
//******************************************************************************************

#include "Device.h"
#include "Everything.h"
#include "Constants.h"

namespace st
{
//private
	
//public
	//constructor
	Device::Device(const __FlashStringHelper *name):
		m_pName(name)
	{
		if(debug)
		{
			Serial.print(F("Device: New Device ID: "));
			Serial.println(getName());
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(debug)
		{
			Serial.print(F("Device: Destroyed Device ID: "));
			Serial.println(getName());
		}
	}

	void Device::refresh()
	{
		
	}
	
	const String Device::getName() const
	{
#if defined(ARDUINO_ARCH_ESP32)   
		return String((const char*)m_pName);  //strcpy_P() causes ESP32 to crash
#else
		char tmp[Constants::MAX_NAME_LENGTH];
		strcpy_P(tmp, (const char*)m_pName);
		return String(tmp);
#endif

	}
	

	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool Device::debug=false;
}