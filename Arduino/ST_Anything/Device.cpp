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
			Serial.println("New device constructed with name: \"" + m_Name + "\"");
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(debug)
		{
			Serial.println("Device destroyed with name: \"" + m_Name + "\"");
		}
	}
	
	bool Device::debug=false;
}