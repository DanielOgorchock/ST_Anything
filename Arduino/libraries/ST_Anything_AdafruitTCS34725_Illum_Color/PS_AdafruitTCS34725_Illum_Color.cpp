//******************************************************************************************
//  File: PS_AdafruitTCS34725_Illum_Color.cpp
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
//    2019-07-01  Dan.t		 	 Added support for websocket Logging, st::debugPrint and st::debugPrintln
//
//
//******************************************************************************************

#include "PS_AdafruitTCS34725_Illum_Color.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_AdafruitTCS34725_Illum_Color::PS_AdafruitTCS34725_Illum_Color(const __FlashStringHelper *name, unsigned int interval, int offset, tcs34725IntegrationTime_t integrationTime, tcs34725Gain_t gain) :
		PollingSensor(name, interval, offset),
		tcs(integrationTime, gain)
	{

	}
	
	//destructor
	PS_AdafruitTCS34725_Illum_Color::~PS_AdafruitTCS34725_Illum_Color()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_AdafruitTCS34725_Illum_Color::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				st::debugPrint(F("PS_AdafruitTCS34725_Illum_Color::beSmart set polling interval to "));
				st::debugPrintln(String(s.toInt()));
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				st::debugPrint(F("PS_AdafruitTCS34725_Illum_Color::beSmart cannot convert "));
				st::debugPrint(s);
				st::debugPrintln(F(" to an Integer."));
			}
		}
	}

	void PS_AdafruitTCS34725_Illum_Color::init() {
		st::debugPrintln("Initializing the TCS34725 sensor...");
		if (tcs.begin()) {
			st::debugPrintln("Found sensor.   tcs.begin = true");
		}
		else {
			st::debugPrintln("No TCS34725 found... check your connections");
		}

		delay(1000); //give sensor time to get its first data sample ready to be read
		getData();
	}
	
	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_AdafruitTCS34725_Illum_Color::getData()
	{
  		tcs.getRawData(&m_nred, &m_ngreen, &m_nblue, &m_nclear);
		m_ncolorTemp = tcs.calculateColorTemperature(m_nred, m_ngreen, m_nblue);
		m_nlux = tcs.calculateLux(m_nred, m_ngreen, m_nblue);

		//st::debugPrint("Color Temp: "); st::debugPrint(String(m_ncolorTemp, DEC)); st::debugPrint(" K - ");
		//st::debugPrint("Lux: "); st::debugPrint(String(m_nlux, DEC)); st::debugPrint(" - ");
		//st::debugPrint(3"R: "); st::debugPrint(String(m_nred, DEC)); st::debugPrint(" ");
		//st::debugPrint("G: "); st::debugPrint(String(m_ngreen, DEC)); st::debugPrint(" ");
		//st::debugPrint("B: "); st::debugPrint(String(m_nblue, DEC)); st::debugPrint(" ");
		//st::debugPrint("C: "); st::debugPrint(String(m_nclear, DEC)); st::debugPrint(" ");
		//st::debugPrintln(F(" "));

		String strSensorValue = String(m_nlux, DEC) + ':' + String(m_ncolorTemp, DEC) + ':' + String(m_nred, DEC) + ':' + String(m_ngreen, DEC) + ':' + String(m_nblue, DEC) + ':' + String(m_nclear, DEC);

		//Send data to SmartThings/Hubitat
		Everything::sendSmartString(getName() + " " + strSensorValue);

	}
	
}
