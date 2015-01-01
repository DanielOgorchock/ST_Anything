#ifndef ST_PS_ALARM_H
#define ST_PS_ALARM_H

#include "PollingSensor.h"

namespace st
{
	class PS_Alarm: public PollingSensor
	{

	private:
		
		bool m_bCurrentState; //HIGH or LOW
		bool m_bInvertLogic;
		byte m_nPin;


		void writeStateToPin();

	public:
		//constructor
		PS_Alarm(const String &name, long interval, long offset, byte Pin, bool startingState = LOW, bool invertLogic = false);

		//destructor
		virtual ~PS_Alarm();

		virtual void init();

		virtual void getData();

		virtual void beSmart(const String &str);

		//gets
		virtual byte getPin() const { return m_nPin; }

		//sets
		virtual void setPin(byte pin);

	
	};
}



#endif