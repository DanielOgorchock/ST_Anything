#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include "Arduino.h"

namespace st
{
	class Constants
	{
		public:
			enum PINS_CHOICE {PINS_0_1, PINS_2_3};
			
			static const byte MAX_SENSOR_COUNT=15;
			static const byte MAX_EXECUTOR_COUNT=15;
			static const String IGNORE_STRING; //definition in Constants.cpp 
			static const Constants::PINS_CHOICE THING_SHIELD_PINS=PINS_2_3; //can change to PINS_0_1
	};
}


#endif