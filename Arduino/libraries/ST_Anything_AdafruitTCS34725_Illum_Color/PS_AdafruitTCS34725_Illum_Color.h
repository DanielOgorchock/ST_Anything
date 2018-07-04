//******************************************************************************************
//  File: PS_AdafruitTCS34725_Illum_Color.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_AdafruitTCS34725_Illum_Color is a class which implements the SmartThings "Illuminance Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  It uses I2C communication to measure the RGB Illuminace from a TCS34725 sensor.
//			  
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_AdafruitTCS34725_Illum_Color sensor1(F("illuminancergb1"), 60, 0);
//
//			  st::PS_AdafruitTCS34725_Illum_Color() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- tcs34725IntegrationTime_t integrationTime - OPTIONAL - defaults to TCS34725_INTEGRATIONTIME_154MS
//				- tcs34725Gain_t gain - OPTIONAL - defaults to TCS34725_GAIN_4X
//
//              Valid Integration Time Values:
//				  TCS34725_INTEGRATIONTIME_2_4MS = 0xFF,   /**<  2.4ms - 1 cycle    - Max Count: 1024  */
//				  TCS34725_INTEGRATIONTIME_24MS = 0xF6,    /**<  24ms  - 10 cycles  - Max Count: 10240 */
//				  TCS34725_INTEGRATIONTIME_50MS = 0xEB,    /**<  50ms  - 20 cycles  - Max Count: 20480 */
//				  TCS34725_INTEGRATIONTIME_101MS = 0xD5,   /**<  101ms - 42 cycles  - Max Count: 43008 */
//				  TCS34725_INTEGRATIONTIME_154MS = 0xC0,   /**<  154ms - 64 cycles  - Max Count: 65535 */
//				  TCS34725_INTEGRATIONTIME_700MS = 0x00    /**<  700ms - 256 cycles - Max Count: 65535 */
//
//				Valid Gain Values:
//				  TCS34725_GAIN_1X = 0x00,    /**<  No gain  */
//				  TCS34725_GAIN_4X = 0x01,    /**<  4x gain  */
//				  TCS34725_GAIN_16X = 0x02,   /**<  16x gain */
//				  TCS34725_GAIN_60X = 0x03    /**<  60x gain */

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
//    2018-07-01  Dan Ogorchock  Cleaned up the design, added ability to adjust configuration, and fixed the comments section
//
//
//******************************************************************************************

#ifndef ST_PS_AdafruitTCS34725_Illum_Color_H
#define ST_PS_AdafruitTCS34725_Illum_Color_H

#include "PollingSensor.h"
#include "Adafruit_TCS34725.h"

namespace st
{
	class PS_AdafruitTCS34725_Illum_Color: public PollingSensor
	{
		private:
			Adafruit_TCS34725 tcs;  //Adafruit TCS34725 object
			uint16_t m_nlux;		//lux
			uint16_t m_ncolorTemp;	//color temperature
			uint16_t m_nred;		//red value
			uint16_t m_ngreen;		//green value
			uint16_t m_nblue;		//blue value
			uint16_t m_nclear;		//clear value

		public:
			//constructor - called in your sketch's global variable declaration section
			PS_AdafruitTCS34725_Illum_Color(const __FlashStringHelper *name, unsigned int interval, int offset, tcs34725IntegrationTime_t integrationTime = TCS34725_INTEGRATIONTIME_154MS, tcs34725Gain_t gain = TCS34725_GAIN_4X);
			
			//destructor
			virtual ~PS_AdafruitTCS34725_Illum_Color();
			
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
