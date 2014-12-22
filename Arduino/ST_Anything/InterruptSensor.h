#ifndef ST_INTERRUPTSENSOR_H
#define ST_INTERRUPTSENSOR_H

#include "Sensor.h"

namespace st
{
	class InterruptSensor: public Sensor
	{
		private:
			byte m_nInterruptPin;
			bool m_bInterruptState; //the state (HIGH/LOW) in which an interrupt has triggered
			bool m_bStatus; //true==interrupted
			bool m_bPullup;
			
			bool checkIfTriggered(); //returns true if new interrupt is triggered
			
		public:
			//constructor
			InterruptSensor(const String &name, byte pin, bool iState, bool internalPullup=false); //(defaults to using a HIGH value as triggering interrupt)
			
			//destructor
			virtual ~InterruptSensor();
			
			
			virtual void init();
			virtual void update();
			
			
			//gets
			inline byte getInterruptPin() const {return m_nInterruptPin;}
			inline bool getInterruptState() const {return m_bInterruptState;}
			inline bool getStatus() const {return m_bStatus;} //whether or not the device is currently interrupted
			
			
			//sets
			void setInterruptPin(byte pin);
			void setInterruptState(bool b) {m_bInterruptState=b;}
	
			
			static bool DEBUG;
	
	};
}


#endif