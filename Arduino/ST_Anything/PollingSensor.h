#ifndef ST_POLLINGSENSOR_H
#define ST_POLLINGSENSOR_H

#include "Sensor.h"

namespace st
{
	class PollingSensor: public Sensor
	{
		private:
			unsigned long m_nPreviousTime; //in milliseconds
			long m_nDeltaTime; //in milliseconds
			unsigned long m_nInterval; //in milliseconds
			
			virtual bool checkInterval(); //returns true and resets dTime if interval has been reached
			
		public:
			//constructor
			PollingSensor(const String &name, unsigned long interval, long offset=0);
			
			//destructor
			virtual ~PollingSensor();
			
			virtual void init();
			virtual void update();
			
			virtual bool getData();
			
			virtual void offset(long os) {m_nDeltaTime-=os;} //offset the delta time from its current value
	
			static bool DEBUG;
	
	};
}



#endif