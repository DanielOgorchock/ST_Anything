//******************************************************************************************
//  File: Sensor.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  st::Sensor is a generic class which inherits from st::Device.  This is the
//			  parent class for the st::PollingSensor and st::InterruptSensor classes.
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

#include "Sensor.h"

#include "Constants.h"

namespace st
{
//private


//public
	//constructor
	Sensor::Sensor(const __FlashStringHelper *name):
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