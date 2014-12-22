#ifndef ST_DEVICE_H
#define ST_DEVICE_H

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
			const String& getName() const {return m_Name;}
			
			
			
			//sets
			void setName(const String &name) {m_Name=name;}
			
	}


}

#endif