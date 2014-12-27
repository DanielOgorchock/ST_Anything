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
			Sensor(const String &name);
			
			//destructor
			virtual ~Sensor();
			
			virtual const String& beSmart(const String &str);
			
			//all derived classes must implement these pure virtual functions
			virtual const String& init()=0;
			virtual const String& update()=0;
	
	};


}


#endif