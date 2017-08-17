//*******************************************************************************
/// @file
/// @brief
///   SmartThingsThingShield Arduino Library 
/// @section License
///   (C) Copyright 2013 Physical Graph
///
/// Updates by Daniel Ogorchock 12/30/2014 - Arduino Mega 2560 HW Serial support
///		-Numerous performance and size optimizations (helpful on UNO with only 2K SRAM)
///		-Arduino UNO should use the SoftwareSerial library Constructor since the UNO has
///		 only one Hardware UART port ("Serial") which is used by the USB port for 
///		 programming and debugging typically.  UNO can use the Hardware "Serial"
///		 if desired, but USB programming and debug will be troublesome.
///		 Leonardo and Mega can use SoftwareSerial BUT cannot use Pin3 for Rx - use
///		 Pin10 for Rx and add jumper from Pin10 to Pin3.
///		-Arduino LEONARDO should use the Hardware Serial Constructor since it has 1 UART
///		 separate from the USB port.  "Serial1" port uses pins 0(Rx) and 1(Tx).
///		-Arduino MEGA should use the Hardware Serial Constructor since it has 4 UARTs.
///		 "Serial3" port uses pins 14(Tx) and 15(Rx).  Wire Pin14 to Pin2 and Pin15 to Pin3. 
///		-Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are 
///		 electrically connected to the ThingShield if set to D2/D3.
///		-Note - Pin6 is reserved by the ThingShield as well.  Best to avoid using it.
///		-The SoftwareSerial library has the following known limitations: 
///			- If using multiple software serial ports, only one can receive data at a time.
///			- Not all pins on the Mega and Mega 2560 support change interrupts, so only 
///			  the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, 
///			  A8(62), A9(63), A10(64), A11(65), A12(66), A13(67), A14(68), A15(69).
///			- Not all pins on the Leonardo and Micro support change interrupts, so only 
///			  the following can be used for RX : 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
/// 	-2016-06-04  Dan Ogorchock  Added improved support for Arduino Leonardo
///		-2017-02-04  Dan Ogorchock  Modified to be a subclass of new SmartThings base class
///		-2017-02-08  Dan Ogorchock  Cleaned up.  Now uses HardwareSerial* objects directly.
///     -2017-08-14  Dan Ogorchock  Disabled SoftwareSerial support if compiling ESP32 board
//*******************************************************************************
#ifndef __SMARTTHINGS_THINGSHIELD_H__
#define __SMARTTHINGS_THINGSHIELD_H__

#include "SmartThings.h"

//*******************************************************************************
#define BOARD_TYPE_UNO       0
#define BOARD_TYPE_LEONARDO  1
#define BOARD_TYPE_MEGA		 2
//*******************************************************************************

#include <inttypes.h>

//*******************************************************************************
// Set the correct board type automatically 
//*******************************************************************************
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) 
#define BOARD_TYPE BOARD_TYPE_UNO
//#define DISABLE_SOFTWARESERIAL  // uncomment to disable SoftwareSerial to save some program space if neccessary while using HW Serial
#elif defined(__AVR_ATmega32U4__)
#define BOARD_TYPE BOARD_TYPE_LEONARDO
#define DISABLE_SOFTWARESERIAL  //Assume HW Serial is being used.  Saves some program space while using HW Serial
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BOARD_TYPE BOARD_TYPE_MEGA
#define DISABLE_SOFTWARESERIAL  //Assume HW Serial is being used.  Saves some program space while using HW Serial
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
#define DISABLE_SOFTWARESERIAL  //The SAMD chip (e.g. MKR1000) and ESP32 do not support SoftwareSerial
#else	//assume user is using an UNO for the unknown case
#define BOARD_TYPE BOARD_TYPE_UNO
//#define DISABLE_SOFTWARESERIAL  // uncomment to disable SoftwareSerial to save some program space if neccessary while using HW Serial
#endif

#ifndef DISABLE_SOFTWARESERIAL
#include <SoftwareSerial.h>
#endif

//*******************************************************************************
#define SMARTTHINGS_RX_BUFFER_SIZE 256 // if > 255: change _nBufRX to u16 
#define SMARTTHINGS_SHIELDTYPE_SIZE 32 // if > 255: change _shieldTypeLen to u16 

namespace st
{
	//*******************************************************************************
	/// @brief ZigBee Network State Definition
	//*******************************************************************************
	typedef enum
	{
		STATE_NO_NETWORK,
		STATE_JOINING,
		STATE_JOINED,
		STATE_JOINED_NOPARENT,
		STATE_LEAVING,
		STATE_UNKNOWN
	} SmartThingsNetworkState_t;

	//*******************************************************************************

	class SmartThingsThingShield: public SmartThings
	{
	private:
#ifndef DISABLE_SOFTWARESERIAL
		SoftwareSerial* _mySerial;
#else
		HardwareSerial* _mySerial;
#endif
		//SmartThingsSerialType_t _SerialPort;

		uint32_t _lastPingMS;
		uint32_t _lastShieldMS;
		SmartThingsNetworkState_t _networkState;
		uint8_t _eui64[8];
		uint16_t _nodeID;

		uint8_t _pBufRX[SMARTTHINGS_RX_BUFFER_SIZE];
		uint_fast8_t _nBufRX;

		void _shieldGetNetworkInfo(void);
		void _process(void);

		void debugPrintBuffer(String prefix, uint8_t * pBuf, uint_fast8_t nBuf);
		bool isRxLine(uint8_t * pLine);
		bool isAsciiHex(uint8_t ascii);
		uint8_t asciiToHexU8(uint8_t pAscii[2]);
		uint_fast8_t translatePayload(uint8_t *pBuf, uint_fast8_t nBuf);
		void handleLine(void);

	public:
		//*******************************************************************************
		/// @brief  SmartThings SoftwareSerial Constructor 
		///   @param[in] pinRX - Receive Pin for the SoftwareSerial Port to the Arduino
		///   @param[in] pinTX - Transmit Pin for the SoftwareSerial Port to the Arduino
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
#ifndef DISABLE_SOFTWARESERIAL
		SmartThingsThingShield(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout, String shieldType = "ThingShield", bool enableDebug = false, int transmitInterval = 1000);
#else
		//*******************************************************************************
		/// @brief  SmartThings HardwareSerial Constructor 
		///   @param[in] hwSerialPort - HardwareSerial Port of the Arduino (i.e. &Serial, &Serial1, &Serial2, &Serial3)
		///   @param[in] callout - Set the Callout Function that is called on Msg Reception
		///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
		///   @param[in] enableDebug (optional) - Enable internal Library debug
		//*******************************************************************************
		SmartThingsThingShield(HardwareSerial* hwSerialPort, SmartThingsCallout_t *callout, String shieldType = "ThingShield", bool enableDebug = false, int transmitInterval = 1000);
#endif
		//*******************************************************************************
		/// @brief Destructor 
		//*******************************************************************************
		virtual ~SmartThingsThingShield();

		//*******************************************************************************
		/// @brief  Initialize SmartThings Library 
		//*******************************************************************************
		virtual void init(void);

		//*******************************************************************************
		/// @brief  Run SmartThings Library 
		//*******************************************************************************
		virtual void run(void);

		//*******************************************************************************
		/// @brief Send Message out over ZigBee to the Hub 
		/// @param[in] message to send
		//*******************************************************************************
		virtual void send(String message);

		//*******************************************************************************
		/// @brief  Set SmartThings Shield MultiColor LED
		/// @param[in]  red: intensity {0=off to 9=max}
		/// @param[in]  green: intensity {0=off to 9=max}
		/// @param[in]  blue: intensity {0=off to 9=max}
		//*******************************************************************************
		void shieldSetLED(uint8_t red, uint8_t green, uint8_t blue);

		//*******************************************************************************
		/// @brief  Get Last Read Shield State
		/// @return Last Read Network State
		//*******************************************************************************
		SmartThingsNetworkState_t shieldGetLastNetworkState(void);

		//*******************************************************************************
		/// @brief  Get Last Read Shield State and Trigger Refresh of Network Info 
		/// @return Last Read Network State
		//*******************************************************************************
		SmartThingsNetworkState_t shieldGetNetworkState(void);

		//*******************************************************************************
		/// @brief  Get Last Read NodeID and Trigger Refresh of Network Info
		/// @return Last Read NodeID 
		//*******************************************************************************
		uint16_t shieldGetNodeID(void);

		//*******************************************************************************
		/// @brief  Get Last Read EUI64 and Trigger Refresh of Network Info
		/// @return Last Read EUI64 
		//*******************************************************************************
		void shieldGetEUI64(uint8_t eui[8]);

		//*******************************************************************************
		/// @brief Find and Join a Network 
		//*******************************************************************************
		void shieldFindNetwork(void);

		//*******************************************************************************
		/// @brief Leave the Current ZigBee Network 
		//*******************************************************************************
		void shieldLeaveNetwork(void);

		//*******************************************************************************
		// Update the network State/LED
		//*******************************************************************************
		void updateNetworkState(void);		//get the current zigbee network status of the ST Shield

	};
}
#endif
