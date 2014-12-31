#ifndef ST_EX_SWITCH
#define ST_EX_SWITCH

#include "Executor.h"

namespace st
{
	class EX_Switch: public Executor
	{
		private:
			bool m_bCurrentState; //HIGH or LOW
			byte m_nPin;
		
			void writeStateToPin();
		
		public:
			//constructor
			EX_Switch(const String &name, byte pin, bool startingState);
			
			//destructor
			virtual ~EX_Switch();

			virtual const String& init();

			virtual const String& beSmart(const String &str);
			
			
			//gets
			virtual byte getPin() const {return m_nPin;}
			
			//sets
			virtual void setPin(byte pin);
		
	};
}

#endif