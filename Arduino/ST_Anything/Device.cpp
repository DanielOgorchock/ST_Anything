#include "Device.h"


namespace st
{
//private
	
//public
	//constructor
	Device::Device(const String &name):
		m_Name(name)
	{
		if(DEBUG)
		{
			Serial.println("New device constructed with name: \"" + m_Name + "\"");
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(DEBUG)
		{
			Serial.println("Device destroyed with name: \"" + m_Name + "\"");
		}
	}
	
	bool Device::DEBUG=false;
}