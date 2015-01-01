#ifndef ST_EX_SWITCH
#define ST_EX_SWITCH

#include "Executor.h"

namespace st
{
	class EX_Switch: public Executor
	{
		private:
			bool m_bCurrentState; //HIGH or LOW
			bool m_bInvertLogic;
			byte m_nPin;
		
			void writeStateToPin();
		
		public:
			//constructor
			EX_Switch(const String &name, byte pin, bool startingState = LOW, bool invertLogic = false);
			
			//destructor
			virtual ~EX_Switch();

			virtual void init();

			virtual void beSmart(const String &str);
			
			virtual void refresh();
			
			//gets
			virtual byte getPin() const {return m_nPin;}
			
			//sets
			virtual void setPin(byte pin);
		
	};
}

#endif