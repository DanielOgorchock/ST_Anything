//******************************************************************************************
//  File: Constants.h
//  Authors: Dan G Ogorchock & Daniel J Ogorchock (Father and Son)
//
//  Summary:  This file contains a number of user-definable settings that affect the 
//			  ST_Anything library.  This is the main file where user specific settings
//			  should be made to adjust the behavior of SmartThings Shield communcations.
//
//	Notes:
//		-The SmartThings Shield uses Pins 0,1 or Pins 2,3 for serial communications based
//		 on the user selectable switch on the shield.
//		-The SmartThings Shield reserves Pin 6 as well.  Best to avoid using it.
//		-The Arduino UNO R3 defaults to using the SoftwareSerial library since the UNO has
//		 only one Hardware UART port ("Serial") which is used by the USB port for 
//		 programming and debugging typically.  
//		-You can use Hardware Serial with an UNO or Leonardo if desired, but you must suppress 
//		 all "Serial.Print()" calls in your sketch, and make sure your Serial.begin() call 
//		 is suppressed or set to use 2400 baud.
//		-Arduino MEGA defaults to using the MEGA's "Serial3" Hardware UART serial port.
//		 This port uses pins 14(Tx) and 15(Rx).  Wire Pin14 to Pin2 and Pin15 to Pin3. 
//		 Be certain to not use Pins 2 & 3 in your Arduino sketch since they are still
//		 electrically connected to the ThingShield.
//		-The SoftwareSerial library has the following known limitations: 
//			-If using multiple software serial ports, only one can receive data at a time.
//			- Not all pins on the Mega and Mega 2560 support change interrupts, so only 
//			  the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, 
//			  A8(62), A9(63), A10(64), A11(65), A12(66), A13(67), A14(68), A15(69).
//			- Not all pins on the Leonardo and Micro support change interrupts, so only 
//			  the following can be used for RX : 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
//    
//  Change History:
//
//    Date        Who            What
//    ----        ---            ----
//    2015-01-03  Dan & Daniel   Original Creation
//
//
//******************************************************************************************

#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include "Arduino.h"
#include "SmartThings.h"

//#define ENABLE_SERIAL			//If uncommented, will allow you to type in commands via the Arduino Serial Console Window (useful for debugging)
//#define DISABLE_SMARTTHINGS	//If uncommented, will disable all ST Shield Library calls (e.g. you want to use this library without SmartThings for a different application)

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) 
#define BOARD_UNO
#elif defined(__AVR_ATmega32U4__)
#define BOARD_LEONARDO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BOARD_MEGA
#else	
#define BOARD_UNO	//assume user is using an UNO for the unknown case
#endif

namespace st
{
	class Constants
	{
		public:
			//Serial debug console baud rate
			static const unsigned int SERIAL_BAUDRATE=9600;			//Uncomment If NOT using pins 0,1 for ST Shield communications (default)
			//static const unsigned int SERIAL_BAUDRATE=2400;		//Uncomment if using Pins 0,1 for ST Shield Communications
			//Maximum number of SENSOR objects
			static const byte MAX_SENSOR_COUNT=10;					//Used to limit the number of sensor devices allowed.  Be careful on Arduino UNO due to 2K SRAM limitation 
			//Maximum number of EXECUTOR objects
			static const byte MAX_EXECUTOR_COUNT=10;				//Used to limit the number of executor devices allowed.  Be careful on Arduino UNO due to 2K SRAM limitation 
			//Size of reserved return string
			static const byte RETURN_STRING_RESERVE = (MAX_SENSOR_COUNT + MAX_EXECUTOR_COUNT) * 5;	//Do not make too large due to UNO's 2K SRAM limitation
			//Interval on which Device's refresh methods are called (in milliseconds) - most useful for Executors and InterruptSensors
			static const long DEV_REFRESH_INTERVAL=60;				//seconds - Used to make sure the ST Cloud is kept current with device status (in case of missed updates to the ST Cloud) - primarily for Executors and InterruptSensors
			
			// ------------------------------------------------------------------------------- 
			// --- SmartThings specific items 
			// -------------------------------------------------------------------------------

			//--- if true, the sketch will stall until the ST ThingShield has joined the ST Hub (set to false for debugging without a ST ThingShield, but with SmartThings code still enabled)
			static const bool WAIT_FOR_JOIN_AT_START = true;
			
			//Select whether to use Hardware or Software Serial Communications - will result in the correct SmartThings constructor being called in Everything.cpp
			#if defined(BOARD_UNO) || defined(BOARD_LEONARDO)   
				#define ST_SOFTWARE_SERIAL		//Use Software Serial for UNO or LEONARDO by default 
				//#define ST_HARDWARE_SERIAL
			#elif defined(BOARD_MEGA)	
				//#define ST_SOFTWARE_SERIAL
				#define ST_HARDWARE_SERIAL		//Use Hardware Serial for MEGA by default
			#else
				#define ST_SOFTWARE_SERIAL
				//#define ST_HARDWARE_SERIAL
			#endif

			//---
			//--- if using SoftwareSerial 
			//---   -set the following based on the pins you desire 
			//---   -NOTE: you must use the SoftwareSerial version of the SmartThings Constructor			
			//---
			#if defined(BOARD_UNO) 
				static const uint8_t pinRX = 3;		//Rx Pin3 - works for UNO R3, but not for Leonardo or Mega
			#else
				static const uint8_t pinRX = 10;	//Rx Pin10 - works for Leonardo or Mega - You MUST jumper Pin10 to Pin3 - do not use Pin3 or Pin10 in your sketch!!!
			#endif
				
				static const uint8_t pinTX = 2;		//Tx Pin2 - works for UNO R3, Leonardo, and Mega
			
			//---
			//--- if using Hardware Serial 
			//---   -set the following based on the pins you desire (HW_SERIAL, Mega Only(HW_SERIAL1, HW_SERIAL2, HW_SERIAL3)) 
			//---   -NOTE: you must use the HardwareSerial version of the SmartThings Constructor					
			//---
				//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL; //UNO, Leonardo, MEGA - You MUST move ThingShield switch to D0/D1 position after loading program and then reset the Arduino
				//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL1; //Mega - You MUST jumper Pin18 to Pin2 AND Pin19 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!
				//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL2; //Mega - You MUST jumper Pin16 to Pin2 AND Pin17 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!
				static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL3; //Mega - You MUST jumper Pin14 to Pin2 AND Pin15 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!

			// -------------------------------------------------------------------------------
	};
}


#endif