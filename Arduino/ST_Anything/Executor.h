#ifndef ST_EXECUTOR_H
#define ST_EXECUTOR_H

#include "Device.h"

namespace st
{
	class Executor: public Device
	{
		private:
			
		
		
		
		
		public:
			//constructor
			Executor(const String &name);
			
			//destructor
			virtual ~Executor();
		
			virtual void init();
			
			virtual void refresh(); //called periodically by Everything class
		
			static bool debug;
	
	};


}



#endif