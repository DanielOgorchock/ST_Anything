//******************************************************************************************
//  File: PS_SoundPressureLevel.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  PS_SoundPressureLevel is a class which implements the "Sound Pressure Level" device capability.
//			  It inherits from the st::PollingSensor class.  The current version uses an analog input to measure the 
//			  voltage on an anlog input pin and then scale it to engineering units.
//
//			  The last four arguments of the constructor are used as arguments to an Arduino map() function which 
//			  is used to scale the analog input readings (e.g. 0 to 1024) to Engineering Units before sending to SmartThings. 
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example: static st::PS_SoundPressureLevel sensor1(F("soundPressureLevel1"), 60, 0, PIN_SPL, 0, 1024, 0.0, 165.0, 50);
//
//			  st::PS_SoundPressureLevel() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- long interval - REQUIRED - the polling interval in seconds
//				- long offset - REQUIRED - the polling interval offset in seconds - used to prevent all polling sensors from executing at the same time
//				- byte pin - REQUIRED - the Arduino Pin to be used as an analog input
//				- double s_l - OPTIONAL - first argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - minimum raw AI value
//				- double s_h - OPTIONAL - second argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - maximum raw AI value
//				- double m_l - OPTIONAL - third argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - Engineering Unit Min (or Max if inverting)
//				- double m_h - OPTIONAL - fourth argument of Arduino map(s_l,s_h,m_l,m_h) function to scale the output - Engineering Unit Max (or Min if inverting)
//              - long m_nHighSpeedPollingInterval - OPTIONAL - number of milliseconds between high speed analog reads - defaults to 50ms
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
//    2019-07-08  Dan Ogorchock  Original Creation
//
//
//******************************************************************************************
#ifndef ST_PS_SOUNDPRESSURELEVEL_H
#define ST_PS_SOUNDPRESSURELEVEL_H

#include "PollingSensor.h"

namespace st
{
	class PS_SoundPressureLevel: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;
			float m_fSensorValue;
			double SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH;
			long m_nPrevMillis;
			long m_nHighSpeedPollingInterval;

			float map_double(double x, double in_min, double in_max, double out_min, double out_max);

		public:
			//constructor - called in your sketch's global variable declaration section
			PS_SoundPressureLevel(const __FlashStringHelper *name, unsigned int interval, int offset, byte analogInputPin, double s_l=0, double s_h=1024, double m_l=0.0, double m_h=165.0, long HighSpeedPollingInterval = 50);
			
			//destructor
			virtual ~PS_SoundPressureLevel();
			
			//SmartThings Shield data handler (receives configuration data from ST - polling interval, and adjusts on the fly)
			virtual void beSmart(const String &str);

			//update function 
			virtual void update();

			//function to get data from sensor and queue results for transfer to ST Cloud 
			virtual void getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline float getSensorValue() const {return m_fSensorValue;}
				
			//sets
			void setPin(byte pin);
	};
}
#endif
