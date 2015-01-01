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
			long m_nInterval; //in milliseconds
			long m_nOffset; //in milliseconds
			
			virtual bool checkInterval(); //returns true and resets dTime if interval has been reached
			
		public:
			//constructor
			PollingSensor(const String &name, long interval, long offset=0);
			
			//destructor
			virtual ~PollingSensor();
			
			virtual void init();
			virtual void update();
			
			virtual void getData();
			
			
			virtual void offset(long os) {m_nOffset=os;} //offset the delta time from its current value
			virtual void setInterval(long interval) {m_nInterval=interval;}
	
			static bool debug;
	
	};
}



#endif