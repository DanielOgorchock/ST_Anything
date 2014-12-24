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
			Executor(byte id);
			
			//destructor
			virtual ~Executor();
		
			virtual const String& init();
		
			static bool debug;
	
	};


}



#endif