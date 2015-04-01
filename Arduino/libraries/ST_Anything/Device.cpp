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
#include "Everything.h"
#include "Constants.h"
#include <EEPROM.h>

namespace st
{
//private
	
//public
	//constructor
	Device::Device(const char *name):
		m_pEeprom(Everything::EepromPointer)
	{
		String nameStr(name);
		Everything::EepromPointer+=nameStr.length()+2;
		
		//write string into EEPROM
		int i;
		for(i=0; i<nameStr.length() && i<Constants::MAX_NAME_LENGTH-1; ++i)
		{
			EEPROM.update(i+m_pEeprom, nameStr.charAt(i));
		}
		EEPROM.update(i+m_pEeprom, 0);
		
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
			String name;
			name.reserve(Constants::MAX_NAME_LENGTH-1);
			
			//populate string from EEPROM
			int i=1;
			char c=EEPROM.read(m_pEeprom);
			while(c!=0)
			{
				name.concat(c);
				c=EEPROM.read(m_pEeprom+i);
				++i;
			}
			
			return name;
			
	}
	

	//debug flag to determine if debug print statements are executed (set value in your sketch)
	bool Device::debug=false;
}