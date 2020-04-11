//******************************************************************************************
//  File: PS_Ultrasonic.cpp
//  Authors: 
//
//  Summary:  PS_Ultrasonic is a class which implements a custom Level device capability.
//			  It inherits from the st::PollingSensor class.  
//
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::PS_Ultrasonic sensor1(F("ultrasonic1"), 60, 0, PIN_ULTRASONIC_T, PIN_ULTRASONIC_E);
//
//			  st::PS_Ultrasonic() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte digitalTriggerPin - REQUIRED - the Arduino Pin to be used as a digital output to trigger ultrasonic
//				- byte digitalEchoPin - REQUIRED - the Arduino Pin to be used as a digital input to read the echo
//
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    
//
//
//******************************************************************************************

#include "PS_Ultrasonic.h"

#include "Constants.h"
#include "Everything.h"


namespace st
{
//private
	

//public
	//constructor - called in your sketch's global variable declaration section
	PS_Ultrasonic::PS_Ultrasonic(const __FlashStringHelper *name, unsigned int interval, int offset, byte digitalTriggerPin, byte digitalEchoPin):
		PollingSensor(name, interval, offset),
		m_nSensorValue(0)
	{
		setPin(digitalTriggerPin,digitalEchoPin);
	}
	
	//destructor
	PS_Ultrasonic::~PS_Ultrasonic()
	{
		
	}

	//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
	void PS_Ultrasonic::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);
		Serial.print("st string #####  ");
		Serial.println(str);
		if (s.toInt() != 0) {
			st::PollingSensor::setInterval(s.toInt() * 1000);
			if (st::PollingSensor::debug) {
				Serial.print(F("PS_Ultrasonic::beSmart set polling interval to "));
				Serial.println(s.toInt());
			}
		}
		else {
			if (st::PollingSensor::debug)
			{
				Serial.print(F("PS_Ultrasonic::beSmart cannot convert "));
				Serial.print(s);
				Serial.println(F(" to an Integer."));
			}
		}
	}

	//function to get data from sensor and queue results for transfer to ST Cloud 
	void PS_Ultrasonic::getData()
	{
		
		//int m_nSensorValue=map(analogRead(m_nAnalogInputPin), SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH);
		long duration;
		// conan ford insert a loop here for filtering out outlier values
		// take the median of 9 samples
		int t_loopcount, t_loopcount2;
		int t_min_idx;
		long t_samploop[9];
		long t_swap;

		
		for(t_loopcount=0;t_loopcount<9; t_loopcount++)
		{
			// Delay after each loop to allow echos to disapate
			delay(500);
			// Clears the trigPin
			digitalWrite(m_nDigitalTriggerPin, LOW);
			delayMicroseconds(2);
			// Sets the trigPin on HIGH state for 10 micro seconds
			digitalWrite(m_nDigitalTriggerPin, HIGH);
			delayMicroseconds(10);
			digitalWrite(m_nDigitalTriggerPin, LOW);
			// Reads the echoPin, returns the sound wave travel time in microseconds
			t_samploop[t_loopcount] = pulseIn(m_nDigitalEchoPin, HIGH);
		}
		
		// Now get the median of the samples
		for(t_loopcount=0;t_loopcount<9; t_loopcount++)
		{
			t_min_idx=t_loopcount;
			for (t_loopcount2=t_loopcount+1; t_loopcount2<9; t_loopcount2++)
			{
				if(t_samploop[t_loopcount2] < t_samploop[t_min_idx])
						t_min_idx=t_loopcount2;
			}
			
			//swap(&t_samploop[t_min_idx], &t_samploop[t_loopcount]);
			t_swap = t_samploop[t_min_idx];
			t_samploop[t_min_idx]=t_samploop[t_loopcount];
			t_samploop[t_loopcount]=t_swap;
		}
		
		// take the middle value
		duration=t_samploop[4];	
		// Calculating the distance
		m_nSensorValue = duration*0.034/2;

		// queue the distance to send to smartthings 
		Everything::sendSmartString(getName() + " " + String(m_nSensorValue));
	}
	
	void PS_Ultrasonic::setPin(byte &trigPin,byte &echoPin)
	{
		m_nDigitalTriggerPin=trigPin;
		m_nDigitalEchoPin=echoPin;
		pinMode(m_nDigitalTriggerPin, OUTPUT); // Sets the trigPin as an Output
		pinMode(m_nDigitalEchoPin, INPUT); // Sets the echoPin as an Input
	}
}
