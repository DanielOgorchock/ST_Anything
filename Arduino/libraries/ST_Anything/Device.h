//******************************************************************************************
//  File: Device.h
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

#ifndef ST_DEVICE_H
#define ST_DEVICE_H

#include <Arduino.h>

namespace st
{
	class Device
	{
		private:
			String m_Name;
			
		public:
			//constructor
			Device(const String &name);
			
			//destructor
			virtual ~Device();
			
			//function used by all devices to process data from SmartThings Shield - all derived classes must implement this pure virtual function
			virtual void beSmart(const String &str)=0; 
			
			//gets
			inline const String& getName() const {return m_Name;}
			
			//sets
			inline void setName(const String &str) {m_Name=str;}
			
			//debug flag to determine if debug print statements are executed (set value in your sketch)
			static bool debug;
	};
}

#endif