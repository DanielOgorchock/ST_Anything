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
			
			//gets
			inline const String& getName() const {return m_Name;}
			
			
			
			//sets
			inline void setName(const String &name) {m_Name=name;}
		

			static bool debug;
	};


}

#endif