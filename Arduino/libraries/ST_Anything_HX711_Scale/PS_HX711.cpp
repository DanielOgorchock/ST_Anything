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

#include "PS_HX711.h"

#include "Constants.h"
#include "Everything.h"
#include <EEPROM.h>

const int calVal_eepromAdress = 0;
namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_HX711::PS_HX711(const __FlashStringHelper *name, byte dout_pin, byte clk_pin, unsigned int interval, int offset):
		PollingSensor(name, interval, offset), m_nSensorValue(0)
	{
       m_n_dout_pin = dout_pin;
	   m_n_clk_pin = clk_pin;
	}
	
	//destructor
	PS_HX711::~PS_HX711()
	{
		
	}

	//Incoming data handler (receives commands and/or configuration data from the hub)
	void PS_HX711::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		String cmd = s.substring(0, s.indexOf(':'));

		if(cmd==F("calibrate")) {
			cmd = s.substring(s.indexOf(':') + 1);
			calibrate(cmd.toFloat());
		}
		else if(s==F("tare")) {
			tare();
		}
		else if(s==F("updated")) {
			updated();
		}
		else if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_HX711::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_HX711::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	void PS_HX711::init() {
 
		Serial.println(F("Initiating the PS_HX711 class"));
		// setup HX711
		scale.begin(m_n_dout_pin, m_n_clk_pin);   // Start scale on specified pins
		
		//get calibration factor from eeprom
        #if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);  
        #endif
        EEPROM.get(calVal_eepromAdress, calibration_factor); 
        EEPROM.get(calVal_eepromAdress + sizeof(int), calibration_offset); 

        Serial.println(F("PS_HX711::init  read the EEPROM"));
        Serial.println(calibration_factor); 
        Serial.println(calibration_offset); 
		
		scale.wait_ready();                    //Ensure scale is ready, this is a blocking function
		scale.set_scale();
		scale.set_offset(calibration_offset);  // Sets the calibration offset.
		Serial.println(F("Scale Set"));
		// tjo  commenting out  tare();
	}

	void PS_HX711::calibrate(const float weight)
	{
		float raw;
		Serial.print(F("calibrate routine called with parameter:"));
		Serial.println(weight);
		if (weight > 0.0)
		{
			scale.wait_ready();
			raw = scale.read_average(5);
			calibration_factor = (raw - calibration_offset) / weight;
			
			WriteEEPROM(calVal_eepromAdress, calibration_factor); 
			Serial.println(F("calibration saved to EEPROM"));			
			updated();
		}
		else {
			Serial.println(F("calibration weight must be positive"));
		}
	}

//called periodically to re-zero the scale
	void PS_HX711::tare() 
	{
		scale.wait_ready();
		scale.tare();                                       // Tare scale on startup
		scale.wait_ready();
		updated();
		calibration_offset = scale.read_average(5);
		WriteEEPROM(calVal_eepromAdress + sizeof(int), calibration_offset); 

		Serial.print(F("Scale Zeroed, with offset:"));
		Serial.println(calibration_offset);
	}

	//function to get data from sensor and queue results for transfer to hub 
	void PS_HX711::getData()
	{
		Serial.println(F("getData routine called"));

	    //Get the data
		float raw;
		scale.wait_ready();
		raw = scale.read_average(5);          // Read average of 5 raw value from scale

		Serial.print(F("Raw: "));
		Serial.print(raw);
		Serial.print(F(", Calibration factor: ")); // Prints calibration factor.
		Serial.print(calibration_factor);
		Serial.print(F(", lastValue: ")); // Prints lastValue .
		Serial.println(lastValue);


    // Assign the data to the sensor value.  m_nSensorValue is a
    // ST_Anything standard for the value of the sensor.


	// Only publish new value if difference of 500 or more in raw value
		if (abs(lastValue - raw) >= 500) {
			float reading;

			scale.wait_ready(); // Wait till scale is ready, this is blocking if your hardware is not connected properly.
			scale.set_scale(calibration_factor);  // Sets the calibration factor.
			reading = scale.get_units(5);        // Read average of 5 values  in g/Kg

			lastValue = raw;
			String m_nSensorValue = String(reading, 2);

		// To make it easier to debug print out our name and sensor value before sending it
			Serial.print(getName());
			Serial.println(m_nSensorValue);		

		// Send the value to our parent which will then update the device handler
			Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
		}
	}
	

	void PS_HX711::WriteEEPROM(int address, int value)
	{
		#if defined(ESP8266)|| defined(ESP32)
		EEPROM.begin(512);
		#endif
		EEPROM.put(address, value);
		#if defined(ESP8266)|| defined(ESP32)
		EEPROM.commit();
		#endif
	}

}