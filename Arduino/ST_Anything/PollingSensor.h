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
			unsigned int m_nInterval; //in milliseconds
			
			bool checkInterval(); //returns true and resets dTime if interval has been reached
			
		public:
			//constructor
			PollingSensor(const String &name, unsigned int interval, int offset=0);
			
			//destructor
			virtual ~PollingSensor();
			
			virtual void init();
			virtual void update();
			
			virtual void offset(int os) {m_nDeltaTime-=os;} //offset the delta time from its current value
	
			static bool DEBUG;
	
	};
}



#endif