#ifndef ST_IS_MOTION_H
#define ST_IS_MOTION_H

#include "InterruptSensor.h"

namespace st
{
	class IS_Motion: public InterruptSensor
	{
		private:
			//inherits everything necessary from parent InterrupSensor Class
			
		public:
			//constructor
			IS_Motion(const String &name, byte pin, bool iState, bool internalPullup = false); //(defaults to NOT using internal pullup resistors)
			
			//destructor
			virtual ~IS_Motion();
			
			
			virtual void init();
			virtual void runInterrupt();
			virtual void runInterruptEnded();
	
	};
}


#endif