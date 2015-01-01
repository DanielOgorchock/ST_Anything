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
	
	void IS_Motion::init()
	{
		if (debug){
			Serial.println(F("Motion Sensor Calibration Started..."));
		}
		digitalWrite(getInterruptPin(), LOW); 
		delay(30000);
		if (debug){
			Serial.println(F("Motion Sensor Calibration Finished"));
		}

		InterruptSensor::init();
	}

	void IS_Motion::runInterrupt()
	{
		Everything::sendSmartString(getName() + F(" active"));
	}
	
	void IS_Motion::runInterruptEnded()
	{
		Everything::sendSmartString(getName() + F(" inactive"));
	}

}