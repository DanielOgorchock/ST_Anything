#ifndef ST_DEVICE_H
#define ST_DEVICE_H

#include <Arduino.h>

namespace st
{
	class Device
	{
		private:
			byte m_nId;
		
		
		
		public:
			//constructor
			Device(byte id);
			
			//destructor
			virtual ~Device();
			
			virtual const String& beSmart(const String &str)=0; 
			
			//gets
			inline byte getId() const {return m_nId;}
			
			//sets
			inline void setId(byte id) {m_nId=id;}
		

			static bool debug;
	};


}

#endif