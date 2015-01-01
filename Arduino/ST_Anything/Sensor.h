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
			
			virtual void beSmart(const String &str);
			
			//all derived classes must implement these pure virtual functions
			virtual void init()=0;
			virtual void update()=0;
	
	};


}


#endif