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
//
//******************************************************************************************

#include "PollingSensor.h"

namespace st
{
	class PS_Generic: public PollingSensor
	{
		private:
			char m_nSensorValue;  //converted to a string so all data can be passed in one call
			
		public:
			//constructor - called in your sketch's global variable declaration section
			//this can be modified to accepting more varialbes into it by modifying the
			//line.  For example if you wanted to assign a pin or a variable you could add
			//a  ", int myVariable" after the offset below and then use that within your program.
			//you would need to also update the associated line in the .cpp file.
			PS_Generic(const __FlashStringHelper *name, unsigned int interval, int offset);
			
			//destructor
			virtual ~PS_Generic();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//initialization routine
			virtual void init();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			
	};
}

