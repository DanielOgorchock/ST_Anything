//******************************************************************************************
//  File: PS_AdafruitTCS34725_Illum_Color.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitTCS34725_Illum_Color is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  value of a simple photo resistor.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (0 to 1024) to Lux before sending to SmartThings.  The
//			  defaults for this sensor are based on the device used during testing.  
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitTCS34725_Illum_Color sensor1("illuminance1", 120, 0, PIN_ILLUMINANCE, 0, 1023, 0, 1000);
//
//			  st::PS_AdafruitTCS34725_Illum_Color() constructor requires the following arguments
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
//    2017-09-07  Allan (vseven) Modified original PS_Illuminance library for use with the Adafruit TCS34725 sensor
//    2017-12-29  Allan (vseven) Fixed bug with improper init() definition per Dans guidance
//
//
//******************************************************************************************

#ifndef ST_PS_AdafruitTCS34725_Illum_Color_H
#define ST_PS_AdafruitTCS34725_Illum_Color_H

#include "PollingSensor.h"

namespace st
{
	class PS_AdafruitTCS34725_Illum_Color: public PollingSensor
	{
		private:
			char m_nSensorValue;  //converted to a string so all data can be passed in one call
			
		public:
			//constructor - called in your sketch's global variable declaration section
			PS_AdafruitTCS34725_Illum_Color(const __FlashStringHelper *name, unsigned int interval, int offset);
			
			//destructor
			virtual ~PS_AdafruitTCS34725_Illum_Color();
			
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
#endif
