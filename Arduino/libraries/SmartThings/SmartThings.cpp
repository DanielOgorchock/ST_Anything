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
#include <SmartThings.h>

//*****************************************************************************
void SmartThings::debugPrintBuffer(String prefix, uint8_t * pBuf, uint_fast8_t nBuf)
{
	if (_isDebugEnabled)
	{
		Serial.print(prefix);
		for (uint_fast8_t i = 0; i < nBuf; i++)
		{
			Serial.print(char(pBuf[i]));
		}
		Serial.println();
	}
}

//*****************************************************************************
bool SmartThings::isRxLine(uint8_t * pLine)
{
	// line starts with "T00000000:RX"
	//int validRxLineLength = 12; // TODO: What is a real value for this length?

	//  return line.length > validRxLineLength && line[0] == 'T' && line[9] = ':' && line[10] == 'R' && line[11] == 'X';
	return ((pLine[0] == 'T') && (pLine[9] == ':') && (pLine[10] == 'R') && (pLine[11] == 'X'));
}

//*******************************************************************************
bool SmartThings::isAsciiHex(uint8_t ascii)
{
	bool retVal = false;
	if (
		((ascii >= 'A') && (ascii <= 'F')) ||
		((ascii >= 'a') && (ascii <= 'f')) ||
		((ascii >= '0') && (ascii <= '9'))
		)
	{
		retVal = true;
	}
	return retVal;
}

//*******************************************************************************
/// @note this function doesn't check for hex validity before converting
//*******************************************************************************
uint8_t SmartThings::asciiToHexU8(uint8_t pAscii[2])
{
	uint8_t hex;
	hex = ((pAscii[0] - (((pAscii[0] >> 6) & 0x1) * 0x37)) & 0xF);
	hex <<= 4;
	hex |= ((pAscii[1] - (((pAscii[1] >> 6) & 0x1) * 0x37)) & 0xF);
	return hex;
}

//*****************************************************************************
uint_fast8_t SmartThings::translatePayload(uint8_t *pBuf, uint_fast8_t nBuf)
{
	uint_fast8_t payloadLength = 0; // return value
	uint_fast8_t payloadStart = 0;
	uint_fast8_t payloadEnd = 0;

	uint_fast8_t i;

	// find [ ] message from the back of the message
	for (i = nBuf - 1; i > 0; i--)
	{
		if (pBuf[i] == ']')
		{
			payloadEnd = i;
		}
		else if (pBuf[i] == '[')
		{
			payloadStart = i;
			break;
		}
	}


	if (_isDebugEnabled)
	{
		Serial.print(F("payload start: "));
		Serial.print(payloadStart);
		Serial.print(F(" end: "));
		Serial.print(payloadEnd);
		Serial.print(F(" : "));
		for (i = payloadStart + 1; i < payloadEnd; i++)
		{
			Serial.print(pBuf[i]);
			Serial.print(' ');
		}
		Serial.println();
	}

	//  int expectedPayloadLength = (payloadEnd - payloadStart) / 3; // TODO: Verify this, but 2 chars for byte and 1 for space char
	//  char payloadString[expectedPayloadLength];
	if ((payloadStart != 0) && (payloadEnd != 0) && (payloadEnd - payloadStart > 4) && (pBuf[payloadStart + 1] == '0') && (pBuf[payloadStart + 2] == 'A'))
	{ // if valid message then parse
		i = payloadStart + 4; // start+3 should be ' '  
		while (i < payloadEnd)
		{
			if (pBuf[i] != ' ')
			{
				if (isAsciiHex(pBuf[i]) && isAsciiHex(pBuf[i + 1]))
				{
					pBuf[payloadLength++] = asciiToHexU8(&(pBuf[i++]));	
				}
			}
			i++;
		}
	}

	pBuf[payloadLength] = 0x0; // null-terminate the string
	return payloadLength;
}

//*****************************************************************************
void SmartThings::_process(void)
{
	uint32_t nowMilliseconds = millis();

	if ((nowMilliseconds < _lastShieldMS) || ((nowMilliseconds - _lastShieldMS) > 5000))
	{
		_shieldGetNetworkInfo();
		_lastShieldMS = nowMilliseconds;
	}
	else if ((_networkState == STATE_JOINED) &&
		((nowMilliseconds < _lastPingMS) || ((nowMilliseconds - _lastPingMS) > 60000)))
	{ // ping every minutes or on rollover
		send("ping");
		_lastPingMS = nowMilliseconds;
	}

	// check for return character
}

//*****************************************************************************
void SmartThings::handleLine(void)
{
	if (_nBufRX > 0)
	{
		if (isRxLine(_pBufRX))
		{
			debugPrintBuffer("->| ", _pBufRX, _nBufRX);
			{
				//char messageBuf[255]; // TODO: Figure this out
				uint_fast8_t messageBufLength = translatePayload(_pBufRX, _nBufRX);

				if (messageBufLength > 0)
				{
					debugPrintBuffer("->| payload :: ", (uint8_t *)_pBufRX, messageBufLength);

					_calloutFunction(String((char *)_pBufRX));  // call out to main application
					// that.handleSmartThingMessage(message);
				}
				else
				{
					debugPrintBuffer("->| no payload from :: ", _pBufRX, _nBufRX);
				}
			}
		}
		else
		{ //XXX Totally slapped together since this is temp-- will go away with command set change 
			uint_fast8_t i = 0;
			bool found = false;
			if (_nBufRX >= 32) //netinfo:0022A3000000B675,E30E,02 
			{
				while (i < _nBufRX)
				{
					if (
						(_pBufRX[i] == 'n') &&
						(_pBufRX[i + 1] == 'e') &&
						(_pBufRX[i + 2] == 't') &&
						(_pBufRX[i + 3] == 'i') &&
						(_pBufRX[i + 4] == 'n') &&
						(_pBufRX[i + 5] == 'f') &&
						(_pBufRX[i + 6] == 'o') &&
						(_pBufRX[i + 7] == ':') &&
						(_pBufRX[i + 24] == ',') &&
						(_pBufRX[i + 29] == ',')
						)
					{
						// parse off EUI 
						if (
							isAsciiHex(_pBufRX[i + 8]) &&
							isAsciiHex(_pBufRX[i + 9]) &&
							isAsciiHex(_pBufRX[i + 10]) &&
							isAsciiHex(_pBufRX[i + 11]) &&
							isAsciiHex(_pBufRX[i + 12]) &&
							isAsciiHex(_pBufRX[i + 13]) &&
							isAsciiHex(_pBufRX[i + 14]) &&
							isAsciiHex(_pBufRX[i + 15]) &&
							isAsciiHex(_pBufRX[i + 16]) &&
							isAsciiHex(_pBufRX[i + 17]) &&
							isAsciiHex(_pBufRX[i + 18]) &&
							isAsciiHex(_pBufRX[i + 19]) &&
							isAsciiHex(_pBufRX[i + 20]) &&
							isAsciiHex(_pBufRX[i + 21]) &&
							isAsciiHex(_pBufRX[i + 22]) &&
							isAsciiHex(_pBufRX[i + 23]) &&

							isAsciiHex(_pBufRX[i + 25]) &&
							isAsciiHex(_pBufRX[i + 26]) &&
							isAsciiHex(_pBufRX[i + 27]) &&
							isAsciiHex(_pBufRX[i + 28]) &&

							isAsciiHex(_pBufRX[i + 30]) &&
							isAsciiHex(_pBufRX[i + 31])
							)
						{
							uint8_t tempNetStat = asciiToHexU8(&(_pBufRX[i + 30]));
							if (tempNetStat <= STATE_LEAVING) // make sure it maps to the enum
							{
								_networkState = (SmartThingsNetworkState_t)tempNetStat;

								_nodeID = asciiToHexU8(&(_pBufRX[i + 25]));
								_nodeID <<= 8;
								_nodeID |= asciiToHexU8(&(_pBufRX[i + 27]));

								_eui64[7] = asciiToHexU8(&(_pBufRX[i + 8]));
								_eui64[6] = asciiToHexU8(&(_pBufRX[i + 10]));
								_eui64[5] = asciiToHexU8(&(_pBufRX[i + 12]));
								_eui64[4] = asciiToHexU8(&(_pBufRX[i + 14]));
								_eui64[3] = asciiToHexU8(&(_pBufRX[i + 16]));
								_eui64[2] = asciiToHexU8(&(_pBufRX[i + 18]));
								_eui64[1] = asciiToHexU8(&(_pBufRX[i + 20]));
								_eui64[0] = asciiToHexU8(&(_pBufRX[i + 22]));

								debugPrintBuffer("  |~> ", &(_pBufRX[i]), 32);
								found = true;
							}
						}
					}
					i++;
				}
			}
			if (found == false)
				debugPrintBuffer("->| IGNORING :: ", _pBufRX, _nBufRX);
		}
		_nBufRX = 0;
	}
}
//*****************************************************************************
void SmartThings::_shieldGetNetworkInfo(void)
{
	st_print(F("custom netinfo\n"));

	if (_isDebugEnabled)
	{
		Serial.print(F("  |<~ custom netinfo\n"));
	}
}

//*****************************************************************************
// Thing API  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//            V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V
//*****************************************************************************

// SoftwareSerial Constructor
#ifndef DISABLE_SOFTWARESERIAL
SmartThings::SmartThings(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout, String shieldType, bool enableDebug) :
	_mySerial(pinRX, pinTX),
	_SerialPort(SW_SERIAL),
	_calloutFunction(callout),
	_isDebugEnabled(enableDebug),
	_lastPingMS(0xFFFFFF00),
	_lastShieldMS(0xFFFFFF00),
	_networkState(STATE_UNKNOWN),
	_nBufRX(0)
{
	//Removed for now to save SRAM space - Not sure why ST included this as it is never used in the library
	//uint_fast8_t i = shieldType.length();
	//if (i > 32)
	//	i = 32;
	//_shieldTypeLen = i;

	//while (i--)
	//{
	//	_shieldTypeBuf[i] = (uint8_t)shieldType[i];
	//}

	st_begin(2400);
}
#endif
//Hardware Serial Constructor
SmartThings::SmartThings(SmartThingsSerialType_t hwSerialPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug) :
#ifndef DISABLE_SOFTWARESERIAL
	_mySerial(254, 255),		//Needed unless you uncomment the "#define DISABLE_SOFTWARESERIAL" to prevent compiler error (SmartThings.h)
#endif
	_SerialPort(hwSerialPort),
	_calloutFunction(callout),
	_isDebugEnabled(hwSerialPort != HW_SERIAL ? enableDebug : false),	//Do not allow debug print statements if using Hardware Serial (pins 0,1) for ST Communications
	_lastPingMS(0xFFFFFF00),
	_lastShieldMS(0xFFFFFF00),
	_networkState(STATE_UNKNOWN),
	_nBufRX(0)
{
	//Removed for now to save SRAM space - Not sure why ST included this as it is never used in the library
	//uint_fast8_t i = shieldType.length();
	//if (i > 32)
	//	i = 32;
	//_shieldTypeLen = i;

	//while (i--)
	//{
	//	_shieldTypeBuf[i] = (uint8_t)shieldType[i];
	//}

	st_begin(2400);
}

//*****************************************************************************
//SmartThings::~SmartThings()
SmartThings::~SmartThings()
{
}

//*****************************************************************************
void SmartThings::run(void)
{
	uint8_t readByte;

	while ((_nBufRX < SMARTTHINGS_RX_BUFFER_SIZE) && st_available())
	{

		readByte = st_read();

		if ((readByte == 0x0D) || (readByte == 0x0A))
		{ // handle data from SmartThing Hub line-by-line, execute user's callback function for each line
			handleLine();
		}
		else
		{
			// keep track of everything that comes in until we reach a newline
			// TODO(cwvh): validate bufferlength 1988-10-19
			//if (_nBufRX > 200)
			//  panic("too many characters!"); 
			_pBufRX[_nBufRX++] = readByte;
		}

	}
	_process();
}

//*****************************************************************************
void SmartThings::send(String message)
{
	// e.g. thing.print("raw 0x0 {00 00 0A 0A 62 75 74 74 6f 6e 20 64 6f 77 6e }");
	st_print(F("raw 0x0 { 00 00 0A 0A "));

	if (_isDebugEnabled)
	{
		Serial.print(F("<-| raw 0x0 { 00 00 0A 0A "));
	}

	for (int i = 0; i < message.length(); i++)
	{
		//char c = message[i];

		st_print(message[i], HEX);
		st_print(' ');

		if (_isDebugEnabled)
		{
			Serial.print(message[i], HEX);
			Serial.print(' ');
		}
	}

	st_print(F("}\nsend 0x0 1 1\n"));

	if (_isDebugEnabled)
	{
		Serial.print(F("}\nsend 0x0 1 1\n"));
	}
}

//*****************************************************************************
void SmartThings::shieldSetLED(uint8_t red, uint8_t green, uint8_t blue)
{
	if (red > 9)    red = 9;
	if (green > 9)  green = 9;
	if (blue > 9)   blue = 9;

	st_print(F("custom rgb "));
	st_write((red + '0'));
	st_print(' ');
	st_write((green + '0'));
	st_print(' ');
	st_write((blue + '0'));
	st_print(F(" \n"));

	if (_isDebugEnabled)
	{
		Serial.print(F("  |<~ custom rgb "));
		Serial.write(red + '0');
		Serial.print(' ');
		Serial.write(green + '0');
		Serial.print(' ');
		Serial.write(blue + '0');
		Serial.print(F(" \n"));
	}
}


//*****************************************************************************
SmartThingsNetworkState_t SmartThings::shieldGetLastNetworkState(void)
{
	return _networkState;
}

//*****************************************************************************
SmartThingsNetworkState_t SmartThings::shieldGetNetworkState(void)
{
	_shieldGetNetworkInfo();
	return _networkState;
}

//*****************************************************************************
uint16_t SmartThings::shieldGetNodeID(void)
{
	_shieldGetNetworkInfo();
	return _nodeID;
}

//*****************************************************************************
void SmartThings::shieldGetEUI64(uint8_t eui[8])
{
	_shieldGetNetworkInfo();
	{
		uint_fast8_t i = 7;
		do
		{
			eui[i] = _eui64[i];
		} while (i--);
	}
}

//*****************************************************************************
void SmartThings::shieldFindNetwork(void)
{
	st_print(F("custom find\n"));

	if (_isDebugEnabled)
	{
		Serial.print(F("  |<~ custom find\n"));

	}
}

//*****************************************************************************
void SmartThings::shieldLeaveNetwork(void)
{
	st_print(F("custom leave\n"));


	if (_isDebugEnabled)
	{
		Serial.print(F("  |<~ custom leave\n"));
	}
}

//*****************************************************************************
void SmartThings::st_begin(long baudRate)
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		_mySerial.begin(baudRate);
#endif
	case HW_SERIAL:
		Serial.end();
		Serial.begin(baudRate);
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		Serial1.end();
		Serial1.begin(baudRate);
		break;
	case HW_SERIAL2:
		Serial2.end();
		Serial2.begin(baudRate);
		break;
	case HW_SERIAL3:
		Serial3.end();
		Serial3.begin(baudRate);
		break;
#endif
	}
}
//*****************************************************************************
int SmartThings::st_available()
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.available();
#endif
	case HW_SERIAL:
		return Serial.available();
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.available();
		break;
	case HW_SERIAL2:
		return Serial2.available();
		break;
	case HW_SERIAL3:
		return Serial3.available();
		break;
#endif
	default:
		return 0;
	}
}
//*****************************************************************************
int SmartThings::st_read()
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.read();
#endif
	case HW_SERIAL:
		return Serial.read();
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.read();
		break;
	case HW_SERIAL2:
		return Serial2.read();
		break;
	case HW_SERIAL3:
		return Serial3.read();
		break;
#endif
	default:
		return 0;
	}
}
//*****************************************************************************
long SmartThings::st_print(String str)
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.print(str);
#endif
	case HW_SERIAL:
		return Serial.print(str);
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.print(str);
		break;
	case HW_SERIAL2:
		return Serial2.print(str);
		break;
	case HW_SERIAL3:
		return Serial3.print(str);
		break;
#endif
	default:
		return 0;
	}
}

//*****************************************************************************
long SmartThings::st_print(char c)
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.print(c);
#endif
	case HW_SERIAL:
		return Serial.print(c);
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.print(c);
		break;
	case HW_SERIAL2:
		return Serial2.print(c);
		break;
	case HW_SERIAL3:
		return Serial3.print(c);
		break;
#endif
	default:
		return 0;

	}
}

//*****************************************************************************
long SmartThings::st_print(char c, int i)
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.print(c, i);
#endif
	case HW_SERIAL:
		return Serial.print(c, i);
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.print(c, i);
		break;
	case HW_SERIAL2:
		return Serial2.print(c, i);
		break;
	case HW_SERIAL3:
		return Serial3.print(c, i);
		break;
#endif
	default:
		return 0;
	}
}

//*****************************************************************************
byte SmartThings::st_write(uint8_t i)
{
	switch (_SerialPort) {
#ifndef DISABLE_SOFTWARESERIAL
	case SW_SERIAL:
		return _mySerial.write(i);
#endif
	case HW_SERIAL:
		return Serial.write(i);
		break;
#if BOARD_TYPE == BOARD_TYPE_MEGA
	case HW_SERIAL1:
		return Serial1.write(i);
		break;
	case HW_SERIAL2:
		return Serial2.write(i);
		break;
	case HW_SERIAL3:
		return Serial3.write(i);
		break;
#endif
	default:
		return 0;
	}
}

