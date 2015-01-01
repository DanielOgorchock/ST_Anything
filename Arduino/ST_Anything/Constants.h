//		-Arduino UNO defaults to using the SoftwareSerial library since the UNO has
//		 only one Hardware UART poart ("Serial") which is used by the USB port for 
//		 programming and debugging typically.
//		-Arduino MEGA defaults to using the MEGA's "Serial3" Hardware UART serial port.
//		 This port uses pins 14(Tx) and 15(Rx).  Wire Pin14 to Pin2 and Pin15 to Pin3. 
//		 Be certain to not use Pins 2 & 3 in your Arduino sketch since they are still
//		 electrically connected to the ThingShield.
//		-Note - Pin6 is reserved by the ThingShield as well.  Best to avoid using it.
//		-The SoftwareSerial library has the following known limitations: 
//			-If using multiple software serial ports, only one can receive data at a time.
//			- Not all pins on the Mega and Mega 2560 support change interrupts, so only 
//			  the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, 
//			  A8(62), A9(63), A10(64), A11(65), A12(66), A13(67), A14(68), A15(69).
//			- Not all pins on the Leonardo and Micro support change interrupts, so only 
//			  the following can be used for RX : 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).


#ifndef ST_CONSTANTS_H
#define ST_CONSTANTS_H

#include "Arduino.h"
#include "SmartThings.h"

//#define ENABLE_SERIAL
//#define DISABLE_SMARTTHINGS

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) 
#define BOARD_UNO
#elif defined(__AVR_ATmega32U4__)
#define BOARD_LEONARDO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BOARD_MEGA
#else	//assume user is using an UNO for the unknown case
#define BOARD_UNO
#endif

namespace st
{
	class Constants
	{
		public:
			//Serial debug console baud rate
			static const unsigned int SERIAL_BAUDRATE=9600;
			//Maximum number of SENSOR objects
			static const byte MAX_SENSOR_COUNT=10;
			//Maximum number of EXECUTOR objects
			static const byte MAX_EXECUTOR_COUNT=10;
			//Size of reserved return string
			static const byte RETURN_STRING_RESERVE = (MAX_SENSOR_COUNT + MAX_EXECUTOR_COUNT) * 5;
			//Interval on which executors' refresh methods are called (in milliseconds)
			static const long EX_REFRESH_INTERVAL=30000;
			
			// ------------------------------------------------------------------------------- 
			// --- SmartThings specific items 
			// -------------------------------------------------------------------------------

			//--- if true, the sketch will stall until the ST ThingShield has joined the ST Hub (set to false for debugging without a ST ThingShield)
			static const bool WAIT_FOR_JOIN_AT_START = true;

			//Select whether to use Hardware or Software Serial Communications
			#if defined(BOARD_UNO) || defined(BOARD_LEONARDO)   //use Software Serial for UNO or LEONARDO
				#define ST_SOFTWARE_SERIAL
				//#define ST_HARDWARE_SERIAL
			#elif defined(BOARD_MEGA)	//use Hardware Serial for MEGA
				//#define ST_SOFTWARE_SERIAL
				#define ST_HARDWARE_SERIAL
			#else
				#define ST_SOFTWARE_SERIAL
				//#define ST_HARDWARE_SERIAL
			#endif

			//--- if using SoftwareSerial - set the following based on the pins you desire - you must use the SoftwareSerial SmartThings Constructor			
			#if defined(BOARD_UNO) 
				static const uint8_t pinRX = 3;		//Rx Pin3 - works for UNO R3, but not for Leonardo or Mega
			#else
				static const uint8_t pinRX = 10;	//Rx Pin10 - works for Leonardo or Mega - You MUST jumper Pin10 to Pin3 - do not use Pin3 in your sketch!!!
			#endif
				
			static const uint8_t pinTX = 2;		//Tx Pin2 - works for UNO R3, Leonardo, and Mega

			//--- if using HardwareSerial - set the following based on the pins you desire (HW_SERIAL, Mega Only(HW_SERIAL1, HW_SERIAL2, HW_SERIAL3)) - you must use the HardwareSerial SmartThings Constructor					
			//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL; //Uno, (Leonardo?), Mega - You MUST move ThingShield switch to D0/D1 position after loading program and then reset the Arduino
			//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL1; //Mega - You MUST jumper Pin18 to Pin2 AND Pin19 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!
			//static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL2; //Mega - You MUST jumper Pin16 to Pin2 AND Pin17 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!
			static const SmartThingsSerialType_t SERIAL_TYPE = HW_SERIAL3; //Mega - You MUST jumper Pin14 to Pin2 AND Pin15 to Pin3 - do not use Pin2 or Pin3 in your sketch!!!

			// -------------------------------------------------------------------------------
	};
}


#endif