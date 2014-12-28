#include "IS_Motion.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor
	IS_Motion::IS_Motion(const String &name, byte pin, bool iState, bool pullup) :
		InterruptSensor(name, pin, iState, pullup)
		{
		}
	
	//destructor
	IS_Motion::~IS_Motion()
	{
	}
	
	const String& IS_Motion::init()
	{
		Serial.println(F("Motion Sensor Calibration Started..."));
		digitalWrite(getInterruptPin(), LOW); 
		delay(30000);
		Serial.println(F("Motion Sensor Calibration Finished"));

		return InterruptSensor::init();
	}

	const String& IS_Motion::runInterrupt()
	{
		Everything::Return_String.remove(0);
		Everything::Return_String += getName() + " active";
		return Everything::Return_String;
	}
	
	const String& IS_Motion::runInterruptEnded()
	{
		Everything::Return_String.remove(0);
		Everything::Return_String += getName() + " inactive";
		return Everything::Return_String;
	}

}