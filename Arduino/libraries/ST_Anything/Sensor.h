//******************************************************************************************
//  File: Sensor.h
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

#ifndef ST_SENSOR_H
#define ST_SENSOR_H

#include "Device.h"

namespace st
{
	//abstract
	class Sensor: public Device
	{
		private:
			
		public:
			//constructor
			Sensor(const __FlashStringHelper *name);
			
			//destructor
			virtual ~Sensor();
			
			//SmartThings Shield data handler
			virtual void beSmart(const String &str);
			
			//all derived classes must implement these pure virtual functions
			virtual void init()=0;
			virtual void update()=0;
	
	};


}


#endif