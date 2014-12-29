#ifndef ST_PS_WATER_H
#define ST_PS_WATER_H

#include "PollingSensor.h"

namespace st
{
	class PS_Water: public PollingSensor
	{
		private:
			byte m_nAnalogInputPin;
			int m_nSensorValue;
			
		public:
			//constructor
			PS_Water(const String &name, unsigned int interval, int offset, byte analogInputPin);
			
			//destructor
			virtual ~PS_Water();
			
			//virtual const String& init();
			virtual const String& getData();
			
			//gets
			inline byte getPin() const {return m_nAnalogInputPin;}
			inline byte getSensorValue() const {return m_nSensorValue;}
				
			//sets
			void setPin(byte pin);
	
	};
}



#endif