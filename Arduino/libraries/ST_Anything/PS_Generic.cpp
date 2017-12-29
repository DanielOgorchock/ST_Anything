//******************************************************************************************
//  File: PS_Generic.h
//  Authors: Allan (vseven) (Based on original programming by Dan G Ogorchock & Daniel J Ogorchock (Father and Son) )
//
//  Summary:  PS_Generic is a class which implements only the SmartThings "Sensor" device capability.
//			  It inherits from the st::PollingSensor class.  The current version is made to be used as a framework 
//                for more complicated programming
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Generic sensor1(F("generic1"), 120, 0);
//
//			  st::PS_Generic() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//
//			  This class supports receiving configuration data from the SmartThings cloud via the ST App.  A user preference
//			  can be configured in your phone's ST App, and then the "Configure" tile will send the data for all sensors to 
//			  the ST Shield.  For PollingSensors, this data is handled in the beSMart() function.
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//    2017-10-20  Allan (vseven) Modified original PS_Illuminance library for use with a generic sensor
//    2017-12-28  Dan Ogorchock  Fixed bug with improper init() definition
//
//******************************************************************************************
#include "PS_Generic.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private


//public
	//constructor - called in your sketch's global variable declaration section
	//this can be modified to accepting more varialbes into it by modifying the
	//next line.  For example if you wanted to assign a pin or a variable you could add
	//a  ", myVariable" after the offset below and then use that within your program.
	//you would need to also update the associated line in the header file.
	PS_Generic::PS_Generic(const __FlashStringHelper *name, unsigned int interval, int offset):
		PollingSensor(name, interval, offset),m_nSensorValue(0)
	{

	}
	
	//destructor
	PS_Generic::~PS_Generic()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Generic::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Generic::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Generic::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_Generic::init() {
		// This is where you would add any initialization for your custom code.  For example if you 
		// are using a Adafruit sensor this is where you would setup the sensor. 
		Serial.println("Initiating the generic class.");
	}

	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_Generic::getData()
	{
		Serial.println("getData routine called");

    //  Here is where you would do whatever you need to do to get your data.  Some basic 
    // sample data is provided below just to see values come back in SmartThings.  We firt
    // define a integer called someValue then assign it a value of 15.
		uint16_t someValue;
		someValue = 15;


    // Here is where you would assign the data to the sensor value.  m_nSensorValue is a
    // ST_Anything standard for the value of the sensor, in this case our generic sensor.
    // In this example I'm converting everything to a String to make it easier to use if
    // you are trying to send up multiple values.  For example you can add the values together
    // as a tring with a colon seperating them then in the DTH split the values back out.
		String m_nSensorValue = String(someValue, DEC);


    // To make it easier to debug print out our name and sensor value before sending it
		Serial.println(getName());
		Serial.println(m_nSensorValue);		


    // Send the value to our parent which will then update the device handler
		Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
	}
	
}
