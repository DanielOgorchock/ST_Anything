#ifndef ST_DEVICE_H
#define ST_DEVICE_H

#include <Arduino.h>

namespace st
{
	class Device
	{
		private:
			String m_Name;
			
		
		public:
			//constructor
			Device(const String &name);
			
			//destructor
			virtual ~Device();
			
			virtual void beSmart(const String &str)=0; 
			
			//gets
			inline const String& getName() const {return m_Name;}
			
			//sets
			inline void setName(const String &str) {m_Name=str;}
		

			static bool debug;
	};


}

#endif