#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include "Arduino.h"

namespace st
{
	class Constants
	{
		public:
			static const unsigned int MAX_SENSOR_COUNT=50;
			static const unsigned int MAX_EXECUTOR_COUNT=50;
			static const String IGNORE_STRING; 
	};
}


#endif