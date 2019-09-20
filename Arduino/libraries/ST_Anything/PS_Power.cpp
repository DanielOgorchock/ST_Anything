//******************************************************************************************
//  File: PS_Power.cpp
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_Power is a class which implements the SmartThings "Power Meter" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an analog input pin via the EmonLib.  This produce the Irms current of the Current Transformer.
//            The Irms current is then multiplied by the voltage constant passed in to produce Power in Watts.
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Power sensor1(F("power1"), 120, 0, PIN_POWER, 30.0, 1480, 120.0);
//
//			  st::PS_Power() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//              - double ICAL - REQUIRED - EmonLib Calibration Constant
//				- unsigned int numSamples - OPTIONAL - defaults to 1480, number of analog readings to use for calculating the Irms Current
//              - float voltage - OPTIONAL - defaults to 120, AC voltage of the mains line being monitored
//				- byte filterConstant - OPTIONAL - Value from 5% to 100% to determine how much filtering/averaging is performed 100 = none (default), 5 = maximum
//
//			  TODO:  Determine a method to persist the ST Cloud's Polling Interval data
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2019-02-17  Dan Ogorchock  Original Creation
//    2019-09-19  Dan Ogorchock  Added filtering optional argument to help reduce noisy signals
//
//
//******************************************************************************************
#include "PS_Power.h"

#include "Constants.h"
#include "Everything.h"
#include <math.h>

namespace st
{
//private

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Power::PS_Power(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double ICAL, unsigned int NumSamples, float voltage, byte filterConstant) :
		PollingSensor(name, interval, offset),
		emon1(),
		m_nAnalogInputPin(analogInputPin),
		m_fICAL(ICAL),
		m_fIrms(0.0),
		m_fApparentPower(0.0),
		m_nNumSamples(NumSamples),
		m_fVoltage(voltage)
	{
		setPin(analogInputPin);

		//check for upper and lower limit and adjust accordingly
		if ((filterConstant <= 0) || (filterConstant >= 100))
		{
			m_fFilterConstant = 1.0;
		}
		else if (filterConstant <= 5)
		{
			m_fFilterConstant = 0.05;
		}
		else
		{
			m_fFilterConstant = float(filterConstant) / 100;
		}

		emon1.current(m_nAnalogInputPin, m_fICAL);             // Current: input pin, calibration.
	}
	

	//destructor
	PS_Power::~PS_Power()
	{
		
	}

	void PS_Power::init()
	{
		//Make sure the EmonLib has some data accumulated before anything is transmitted to ST/Hubitat
		m_fIrms = emon1.calcIrms(m_nNumSamples);
		m_fIrms = emon1.calcIrms(m_nNumSamples);
		m_fIrms = emon1.calcIrms(m_nNumSamples);
		m_fIrms = emon1.calcIrms(m_nNumSamples);
		m_fIrms = emon1.calcIrms(m_nNumSamples);

		m_fApparentPower = m_fIrms * m_fVoltage; //Calcuate Apparent Power

		//Send initial data to ST/Hubitat
		getData();
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Power::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Power::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Power::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//function to get data from sensor and queue results for transfer to ST/Hubitat 
	void PS_Power::getData()
	{
		double tempValue = 0;

		m_fIrms = emon1.calcIrms(m_nNumSamples);  // Calculate Irms only

		tempValue = m_fIrms * m_fVoltage; //Calcuate Apparent Power

		m_fApparentPower = (m_fFilterConstant * tempValue) + (1 - m_fFilterConstant) * m_fApparentPower;

		Everything::sendSmartString(getName() + " " + String(m_fApparentPower));
	}
	
	void PS_Power::setPin(byte pin)
	{
		m_nAnalogInputPin=pin;
	}
}
