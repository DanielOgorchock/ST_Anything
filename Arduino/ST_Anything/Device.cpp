#include "Device.h"


namespace st
{
//private
	
//public
	//constructor
	Device::Device(byte id):
		m_nId(id)
	{
		if(debug)
		{
			Serial.println("New Device ID: " + m_nId);
		}
	}
	
	//destructor
	Device::~Device()
	{
		if(debug)
		{
			Serial.println("Destroyed Device ID: " + m_nId);
		}
	}
	
	bool Device::debug=false;
}