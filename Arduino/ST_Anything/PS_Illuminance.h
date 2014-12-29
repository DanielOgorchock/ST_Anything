#ifndef ST_PS_ILLUMINANCE_H
#define ST_PS_ILLUMINANCE_H

#include "PollingSensor.h"

namespace st
{
	class PS_Illuminance: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;
			int m_nSensorValue;
			const int SENSOR_LOW, SENSOR_HIGH, MAPPED_LOW, MAPPED_HIGH;
			
		public:
			//constructor
			PS_Illuminance(const String &name, unsigned int interval, int offset, byte analogInputPin, int s_l=0, int s_h=1024, int m_l=1000, int m_h=0);
			
			//destructor
			virtual ~PS_Illuminance();
			
			virtual const String& beSmart(const String &str);

			virtual const String& getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			void setPin(byte pin);
	
	};
}



#endif