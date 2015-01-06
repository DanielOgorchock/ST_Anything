//*******************************************************************************
/// @file
/// @brief
///   SmartThings Arduino Library 
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
///		-Arduino MEGA should use the Hardware Serial Constructor since it has 4 UARTs.
///		 "Serial3" port uses pins 14(Tx) and 15(Rx).  Wire Pin14 to Pin2 and Pin15 to Pin3. 
///		-Be certain to not use Pins 2 & 3 in your Arduino sketch for I/O since they are 
///		 electrically connected to the ThingShield.
///		-Note - Pin6 is reserved by the ThingShield as well.  Best to avoid using it.
///		-The SoftwareSerial library has the following known limitations: 
///			- If using multiple software serial ports, only one can receive data at a time.
///			- Not all pins on the Mega and Mega 2560 support change interrupts, so only 
///			  the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, 
///			  A8(62), A9(63), A10(64), A11(65), A12(66), A13(67), A14(68), A15(69).
///			- Not all pins on the Leonardo and Micro support change interrupts, so only 
///			  the following can be used for RX : 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
//*******************************************************************************
#ifndef __SMARTTHINGS_H__
#define __SMARTTHINGS_H__

//*******************************************************************************
#define BOARD_TYPE_UNO       0
#define BOARD_TYPE_LEONARDO  1
#define BOARD_TYPE_MEGA		 2
//*******************************************************************************

//#define DISABLE_SOFTWARESERIAL  // uncomment to disable SoftwareSerial to save space if using HW Serial

#include <inttypes.h>
#include <Arduino.h>
#ifndef DISABLE_SOFTWARESERIAL
#include <SoftwareSerial.h>
#endif


//*******************************************************************************
// Set the correct board type automatically 
//*******************************************************************************
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__) 
#define BOARD_TYPE BOARD_TYPE_UNO
#elif defined(__AVR_ATmega32U4__)
#define BOARD_TYPE BOARD_TYPE_LEONARDO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define BOARD_TYPE BOARD_TYPE_MEGA
#else	//assume user is using an UNO for the unknown case
#define BOARD_TYPE BOARD_TYPE_UNO
#endif

//*******************************************************************************

//*******************************************************************************
/// @brief SmartThings Arduino Library Version
//*******************************************************************************
#define SMARTTHINGSLIB_VERSION_MAJOR    0
#define SMARTTHINGSLIB_VERSION_MINOR    0
#define SMARTTHINGSLIB_VERSION_BUILD    6

#define SMARTTHINGSLIB_VERSION \
	(((SMARTTHINGSLIB_VERSION_MAJOR & 0xFF) << 24) | \
	((SMARTTHINGSLIB_VERSION_MINOR & 0xFF) << 16) | \
	(SMARTTHINGSLIB_VERSION_BUILD & 0xFFFF))

//*******************************************************************************
#define SMARTTHINGS_RX_BUFFER_SIZE 256 // if > 255: change _nBufRX to u16 
#define SMARTTHINGS_SHIELDTYPE_SIZE 32 // if > 255: change _shieldTypeLen to u16 

//*******************************************************************************
/// @brief Callout Function Definition for Messages Received over ZigBee Network 
//*******************************************************************************
typedef void SmartThingsCallout_t(String message);

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
/// @brief Serial Communication Type Definition
//*******************************************************************************
typedef enum
{
	SW_SERIAL,
	HW_SERIAL,
	HW_SERIAL1,
	HW_SERIAL2,
	HW_SERIAL3
} SmartThingsSerialType_t;

//*******************************************************************************

class SmartThings
{
private:
#ifndef DISABLE_SOFTWARESERIAL
	SoftwareSerial _mySerial;
#endif
	SmartThingsSerialType_t _SerialPort;
	SmartThingsCallout_t *_calloutFunction;
	bool _isDebugEnabled;
	uint32_t _lastPingMS;
	uint32_t _lastShieldMS;

	SmartThingsNetworkState_t _networkState;
	uint8_t _eui64[8];
	uint16_t _nodeID;

	//Removed for now to save SRAM space - Not sure why ST included this as it is never used in the library
	//uint8_t _shieldTypeBuf[SMARTTHINGS_SHIELDTYPE_SIZE];
	//uint8_t _shieldTypeLen;

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
	void st_begin(long);
	int st_available();
	int st_read();
	long st_print(String);
	long st_print(char);
	long st_print(char, int);
	byte st_write(uint8_t);


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
	SmartThings(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout, String shieldType = "GenericShield", bool enableDebug = false);
#endif
	//*******************************************************************************
	/// @brief  SmartThings HardwareSerial Constructor 
	///   @param[in] hwSerialPort - enum of Hardware Serial Port to the Arduino
	///   @param[in] callout - Set the Callout Function that is called on Msg Reception
	///   @param[in] shieldType (optional) - Set the Reported SheildType to the Server 
	///   @param[in] enableDebug (optional) - Enable internal Library debug
	//*******************************************************************************
	SmartThings(SmartThingsSerialType_t hwSerialPort, SmartThingsCallout_t *callout, String shieldType = "GenericShield", bool enableDebug = false);

	//*******************************************************************************
	/// @brief Descructor 
	//*******************************************************************************
	~SmartThings();

	//*******************************************************************************
	/// @brief  Run SmartThings Library 
	//*******************************************************************************
	void run(void);

	//*******************************************************************************
	/// @brief Send Message out over ZigBee to the Hub 
	/// @param[in] message to send
	//*******************************************************************************
	void send(String message);

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
};

#endif
