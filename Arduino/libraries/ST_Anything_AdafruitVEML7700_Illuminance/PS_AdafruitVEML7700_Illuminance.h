//******************************************************************************************
//  File: PS_AdafruitVEML7700_Illuminance.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitVEML7700_Illuminance is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the Illuminace from a VEML7700 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitVEML7700_Illuminance sensor1(F("illuminance1"), 60, 0);  (simple, uses defaults)
//			  For Example:  st::PS_AdafruitVEML7700_Illuminance sensor1(F("illuminance1"), 60, 0, VEML7700_IT_50MS, VEML7700_GAIN_1_8); (full user control of settings)
//
//			  st::PS_AdafruitVEML7700_Illuminance() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- uint8_t integrationTime - OPTIONAL - defaults to VEML7700_IT_50MS
//				- uint8_t gain - OPTIONAL - defaults to VEML7700_GAIN_1_8
//
//
//              I2C address options
//                  VEML7700_I2CADDR_DEFAULT 0x10		//< Default address
//
//              Valid Integration Time Values:
//					VEML7700_IT_100MS           0x00
//					VEML7700_IT_200MS           0x01
//					VEML7700_IT_400MS           0x02
//					VEML7700_IT_800MS           0x03
//					VEML7700_IT_50MS            0x08	//< Default integration time
//					VEML7700_IT_25MS            0x0C
//
//				Valid Gain Values:
//					VEML7700_GAIN_1             0x00
//					VEML7700_GAIN_2             0x01
//					VEML7700_GAIN_1_8           0x02	//< Default gain
//					VEML7700_GAIN_1_4           0x03
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
//    2019-09-28  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************

#ifndef ST_PS_AdafruitVEML7700_Illuminance_H
#define ST_PS_AdafruitVEML7700_Illuminance_H

#include "PollingSensor.h"
#include "Adafruit_VEML7700.h"

namespace st
{
	class PS_AdafruitVEML7700_Illuminance: public PollingSensor
	{
		private:
			Adafruit_VEML7700 veml;  //Adafruit VEML7700 object
			uint8_t m_nGain;
			uint8_t m_nIntegrationTime;
			long m_nLux;		//lux

		public:
			//constructor - called in your sketch's global variable declaration section
			PS_AdafruitVEML7700_Illuminance(const __FlashStringHelper *name, unsigned int interval, int offset, uint8_t integrationTime = VEML7700_IT_50MS, uint8_t gain = VEML7700_GAIN_1_8);
			
			//destructor
			virtual ~PS_AdafruitVEML7700_Illuminance();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);
			
			//initialization routine
			virtual void init();
			
			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
				
			//sets
			
	};
}
#endif
