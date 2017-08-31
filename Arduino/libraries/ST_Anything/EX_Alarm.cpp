//******************************************************************************************
//  File: EX_Alarm.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  EX_Alarm is a class which implements the SmartThings "Alarm" device capability.
//			  It inherits from the st::PollingSensor class.  The current version implements the Siren as
//			  a digital output (uses a relay to switch the Siren on or off)
//
//			  TODO:  Possibly add Strobe capability in the future
//
//			  Create an instance of this class in your sketch's global variable section
//			  For Example:  st::EX_Alarm executor2("alarm1", PIN_ALARM, LOW, true, PIN_STROBE);
//
//			  st::EX_Alarm() constructor requires the following arguments
//				- String &name - REQUIRED - the name of the object - must match the Groovy ST_Anything DeviceType tile name
//				- byte pin - REQUIRED - the Arduino Pin to be used as a digital output
//				- bool startingState - OPTIONAL - the value desired for the initial state of the switch.  LOW = "off", HIGH = "on"
//				- bool invertLogic - OPTIONAL - determines whether the Arduino Digital Output should use inverted logic
//				- byte pinStrobe - OPTOINAL - If supplied, will allow separate SIREN and STROBE outputs
//
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//	  2017-04-20  Dan Ogorchock	 Add optional Strobe functionality
//    2017-04-26  Dan Ogorchock  Improved Logic if Strobe pin not defined 
//    2018-08-30  Dan Ogorchock  Modified comment section above to comply with new Parent/Child Device Handler requirements
//
//
//******************************************************************************************

#include "EX_Alarm.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
	//private
	void EX_Alarm::writeStateToPin()
	{
		if (m_nCurrentAlarmState == both) {
			digitalWrite(m_nPin, m_bInvertLogic ? LOW : HIGH);
			if (m_bUseStrobe) { digitalWrite(m_nPinStrobe, m_bInvertLogic ? LOW : HIGH); }
		}
		else if(m_nCurrentAlarmState == siren) {
			digitalWrite(m_nPin, m_bInvertLogic ? LOW : HIGH);
			if (m_bUseStrobe) { digitalWrite(m_nPinStrobe, m_bInvertLogic ? HIGH : LOW); }
		}
		else if(m_nCurrentAlarmState == strobe) {
			digitalWrite(m_nPin, m_bInvertLogic ? HIGH : LOW);
			if (m_bUseStrobe) { digitalWrite(m_nPinStrobe, m_bInvertLogic ? LOW : HIGH); }
		}
		else if(m_nCurrentAlarmState == off) {
			digitalWrite(m_nPin, m_bInvertLogic ? HIGH : LOW);
			if (m_bUseStrobe) { digitalWrite(m_nPinStrobe, m_bInvertLogic ? HIGH : LOW); }
		}

	}

	//public
	//constructor
	EX_Alarm::EX_Alarm(const __FlashStringHelper *name, byte pin, bool startingState, bool invertLogic, byte pinStrobe) :
		Executor(name),
		m_nPin(pin),
		m_bInvertLogic(invertLogic),
		m_nPinStrobe(pinStrobe)
	{
		m_nCurrentAlarmState = off;
		m_bUseStrobe = false;

		setPin(pin);
		//If user has supplied a pin for Strobe Light
		if (pinStrobe < 255) {
			m_bUseStrobe = true;
			setPin(pinStrobe);
		}
		//else
		//{
		//	m_bUseStrobe = false;
		//}


	}

	//destructor
	EX_Alarm::~EX_Alarm()
	{

	}

	//initialization routine
	void EX_Alarm::init()
	{
		refresh();	//update ST Cloud with initial data
	}

	//called periodically to ensure state of the Alarm Siren is up to date in the SmartThings Cloud (in case an event is missed)
	void EX_Alarm::refresh()
	{
		//if (debug) {
		//	Serial.print(F("EX_Alarm:: UseStrobe = "));
		//	Serial.println(m_bUseStrobe);
		//}

		if (m_nCurrentAlarmState == both) {
			Everything::sendSmartString(getName() + F(" both"));
		}
		else if(m_nCurrentAlarmState == siren) {
			Everything::sendSmartString(getName() + F(" siren"));
		}
		else if(m_nCurrentAlarmState == strobe) {
			Everything::sendSmartString(getName() + F(" strobe"));
		}
		else if(m_nCurrentAlarmState == off) {
		//else {
			Everything::sendSmartString(getName() + F(" off"));
		}
	}

	//SmartThings Shield data handler (receives command to turn "both" or "off" the Alarm (digital output)
	void EX_Alarm::beSmart(const String &str)
	{
		String s = str.substring(str.indexOf(' ') + 1);

		if (debug) {
			Serial.print(F("EX_Alarm::beSmart s = "));
			Serial.println(s);
		}

		//if (m_bUseStrobe) {
			if (s == F("both")) {
				if (m_bUseStrobe) {
					m_nCurrentAlarmState = both;
				}
				else {
					m_nCurrentAlarmState = siren;
					if (debug) {
						Serial.println(F("EX_Alarm::beSmart - Strobe Pin not defined. Defaulting to Siren!"));
					}

				}
			}
			else if(s == F("siren")) {
				m_nCurrentAlarmState = siren;
			}
			else if(s == F("strobe")) {
				if (m_bUseStrobe) {
					m_nCurrentAlarmState = strobe;
				}
				else {
					m_nCurrentAlarmState = siren;
					if (debug) {
						Serial.println(F("EX_Alarm::beSmart - Strobe Pin not defined. Defaulting to Siren!"));
					}

				}
			}
			else if(s == F("off")) {
				m_nCurrentAlarmState = off;
			}
		//}
		//else {
		//	if (s == F("both") {
		//		m_nCurrentAlarmState = both;
		//	}
		//	else if(s == F("siren")) {
		//		m_nCurrentAlarmState = siren;
		//	}
		//	else if(s == F("strobe")) {
		//		m_nCurrentAlarmState = siren;
		//	}
		//	else if(s == F("off")) {
		//		m_nCurrentAlarmState = off;
		//	}
		//}

		writeStateToPin();

		refresh();
	}

	void EX_Alarm::setPin(byte pin)
	{
		//m_nPin = pin;
		digitalWrite(pin, m_bInvertLogic ? HIGH : LOW);
		pinMode(pin, OUTPUT);
		writeStateToPin();
	}
}