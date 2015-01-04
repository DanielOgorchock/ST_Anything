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
//
//
//******************************************************************************************

#include "Device.h"

namespace st
{
//private
	
//public
	//constructor
	Device::Device(const String &name):
		m_Name(name)
	{
		if(debug)
		{
			Serial.print(F("Device: New Device ID: "));
			Serial.println(m_Name);
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(debug)
		{
			Serial.print(F("Device: Destroyed Device ID: "));
			Serial.println(m_Name);
		}
	}

	void Device::refresh()
	{

	}

	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool Device::debug=false;
}