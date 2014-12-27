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
			Serial.print(F("New Device ID: "));
			Serial.println(m_Name);
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(debug)
		{
			Serial.print(F("Destroyed Device ID: "));
			Serial.println(m_Name);
		}
	}
	
	bool Device::debug=false;
}