//******************************************************************************************
//  File: PS_10kThermistor.cpp
//  Authors: D. Johnson (_M2) based on the work of Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_10kThermistor is a class which implements the Hubitat "Temperature Measurement" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  value of a simple thermal resistor using another 10k resistor as a voltage divider.
//
//			  The last four arguments of the constructor are used as arguments to configure the resistance of the parts.  
//			  The first number is the resistance of the thermistor. Second is R1. Third is the thermistor coefficient. Last is your preference of units.
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_10kThermistor sensor1(F("temperature1"), 120, 0, PIN_THERMISTOR, 10000, 10000, 3300, "F");
//
//			  st::PS_10kThermistor() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- int tl - OPTIONAL - resistance of the thermocouple at the nominal temperature (usually 25C, 77F)
//				- int r1 - OPTIONAL - actual measured resistance of the voltage divider resistor
//				- int BCOEFF - OPTIONAL - The beta coefficient of the thermistor (usually 3000-4000). Tweak this number to calibrate.
//				- int tempNOM - OPTIONAL - The nominal temperature of the thermistor @10k (usually 25C, 77F).
//				- int unit - OPTIONAL - Use the letter F for Farhenheit, C for Celsius
//
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//    2017-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//	  2019-12-23  D. Johnson	 Created 10k_Thermistor using PS_Illuminance as example
//
//******************************************************************************************

#include "PS_10kThermistor.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_10kThermistor::PS_10kThermistor(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, int t1, int r1, int BCOEFF, int tempNom, char unit):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0),
		thermResistance(t1),
		r1Resistance(r1),
		BetaCoeff(BCOEFF),
		UNIT(unit),
		TEMPNOMINAL(tempNom)

	{
		setPin(analogInputPin);
	}
	
	//destructor
	PS_10kThermistor::~PS_10kThermistor()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_10kThermistor::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_10kThermistor::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_10kThermistor::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_10kThermistor::getData()
	{
		int NUMSAMPLES = 10;
		float samples[NUMSAMPLES];
		for (int i=0; i< NUMSAMPLES; i++) 
		{
			samples[i] = analogRead(m_nAnalogInputPin);
		}
	   
		// average all the samples out
		float average = 0;
		for (int i=0; i< NUMSAMPLES; i++) 
		{
		   average += samples[i];
		}
		average /= NUMSAMPLES;
		// convert the value to resistance
		average = 1023 / average - 1;
		average = r1Resistance / average;

		float reading;
		float Temp1F;
		float Temp1C;		
		reading = average / thermResistance;     // (R/Ro)
		reading = log(reading);                  // ln(R/Ro)
		reading /= BetaCoeff;                    // 1/B * ln(R/Ro)
		reading += 1.0 / (TEMPNOMINAL + 273.15); // + (1/To)
		reading = 1.0 / reading;                 // Invert
		Temp1C = reading -= 273.15;              // convert to C
		Temp1F = (Temp1C * 1.8) + 32;            // convert to F                          
		
		if(UNIT == 'F'){
			m_nSensorValue = Temp1F;
		}
		else {
			m_nSensorValue = Temp1C;
		}
		
		Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
	}
	void PS_10kThermistor::setPin(byte pin)
	{
		m_nAnalogInputPin = pin;
	}	
}
