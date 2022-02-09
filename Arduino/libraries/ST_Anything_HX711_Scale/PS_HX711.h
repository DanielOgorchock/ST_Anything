//******************************************************************************************
//  File: PS_HX711.h
//  Authors: Malcolm Dobson + Tim OCallaghan (Based on original programming by Dan G Ogorchock & Daniel J Ogorchock (Father and Son) )
//
//  Summary:  PS_HX711 is a class which implements only the SmartThings "Sensor" device capability.
//			  It inherits from the st::PollingSensor class.  The current version is made to be used as a framework 
//                for more complicated programming
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_HX711 sensor1(F("weight1"), 120, 0);
//
//			  st::PS_HX711() constructor requires the following arguments
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
//    2021-01-19  Malcolm Dobson Redone to support HX711 weight sensor
//    2021-01-23  Tim OCallaghan Pass pins on constructor and add storing of calibration factor in EEPROM and don't tare in init
//    2021-01-24  Malcolm Dobson Save the offset also in eeprom and then read and set in init
//    2022-02-08  Dan Ogorchock  Minor comments clean up
//
//
//******************************************************************************************

#include "PollingSensor.h"
#include <HX711.h>

namespace st
{
	class PS_HX711: public PollingSensor
	{
		private:
			char m_nSensorValue;  //converted to a string so all data can be passed in one call
			HX711 scale;
			int calibration_factor = -16900; // Defines factor we'll use for calibrating.
			int calibration_offset = 0; // Defines zero offset we'll use for calibrating.
			int lastValue = -1;
			byte m_n_dout_pin;          //esp pin for hx711 dout 
			byte m_n_clk_pin;           //esp pin for hx711 clk
		public:
			//constructor - called in your sketch's global variable declaration section
			//this can be modified to accepting more varialbes into it by modifying the
			//line.  For example if you wanted to assign a pin or a variable you could add
			//a  ", int myVariable" after the offset below and then use that within your program.
			//you would need to also update the associated line in the .cpp file.
			PS_HX711(const __FlashStringHelper *name, byte dout_pin, byte clk_pin, unsigned int interval, int offset);
			
			//destructor
			virtual ~PS_HX711();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//initialization routine
			virtual void init();

			//calibrate scale
			void calibrate(const float weight);

			//reset scale to zero
			void tare(); 

			//update device, so next read will publish a new value
			inline void updated() {lastValue = -1;}
	
			void WriteEEPROM(int address, int value);

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			// set a calibration factor so the scale report accurate weight
			void setCalibration(const int factor) {calibration_factor = factor;}	
			inline void clearLastValue() {lastValue = -1;}	
	};
}
