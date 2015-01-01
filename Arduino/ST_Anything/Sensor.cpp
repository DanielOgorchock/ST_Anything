#include "Sensor.h"

#include "Constants.h"

namespace st
{
//private


//public
	//constructor
	Sensor::Sensor(const String &name):
		Device(name)
	{
	
	}
	
	//destructor
	Sensor::~Sensor()
	{
	
	}
	
	void Sensor::beSmart(const String &str)
	{
		//Each derived class should implement this if they are interfacing with SmartThings over the internet.
	}

}