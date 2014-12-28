#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include "Arduino.h"

namespace st
{
	class Constants
	{
		public:
			enum PINS_CHOICE {PINS_0_1, PINS_2_3};
			
			static const byte MAX_SENSOR_COUNT=10;
			static const byte MAX_EXECUTOR_COUNT=10;
			static const String IGNORE_STRING; //definition in Constants.cpp 
			static const byte RETURN_STRING_RESERVE=50;
			static const Constants::PINS_CHOICE THING_SHIELD_PINS=PINS_2_3; //can change to PINS_0_1
			static const bool WAIT_FOR_JOIN_AT_START=true; //if true, the sketch will stall until the shield has joined the hub
	};
}


#endif