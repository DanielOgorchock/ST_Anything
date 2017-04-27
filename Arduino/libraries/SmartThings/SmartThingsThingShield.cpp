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
//*******************************************************************************
#include "SmartThingsThingShield.h"

namespace st
{
	//*****************************************************************************
	void SmartThingsThingShield::debugPrintBuffer(String prefix, uint8_t * pBuf, uint_fast8_t nBuf)
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
	bool SmartThingsThingShield::isRxLine(uint8_t * pLine)
	{
		// line starts with "T00000000:RX"
		return ((pLine[0] == 'T') && (pLine[9] == ':') && (pLine[10] == 'R') && (pLine[11] == 'X'));
	}

	//*******************************************************************************
	bool SmartThingsThingShield::isAsciiHex(uint8_t ascii)
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
	uint8_t SmartThingsThingShield::asciiToHexU8(uint8_t pAscii[2])
	{
		uint8_t hex;
		hex = ((pAscii[0] - (((pAscii[0] >> 6) & 0x1) * 0x37)) & 0xF);
		hex <<= 4;
		hex |= ((pAscii[1] - (((pAscii[1] >> 6) & 0x1) * 0x37)) & 0xF);
		return hex;
	}

	//*****************************************************************************
	uint_fast8_t SmartThingsThingShield::translatePayload(uint8_t *pBuf, uint_fast8_t nBuf)
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
	void SmartThingsThingShield::_process(void)
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
	}

	//*****************************************************************************
	void SmartThingsThingShield::handleLine(void)
	{
		if (_nBufRX > 0)
		{
			if (isRxLine(_pBufRX))
			{
				debugPrintBuffer("->| ", _pBufRX, _nBufRX);
				{
					uint_fast8_t messageBufLength = translatePayload(_pBufRX, _nBufRX);

					if (messageBufLength > 0)
					{
						debugPrintBuffer("->| payload :: ", (uint8_t *)_pBufRX, messageBufLength);

						_calloutFunction(String((char *)_pBufRX));  // call out to main application
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
	void SmartThingsThingShield::_shieldGetNetworkInfo(void)
	{
		_mySerial->print(F("custom netinfo\n"));

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
	SmartThingsThingShield::SmartThingsThingShield(uint8_t pinRX, uint8_t pinTX, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThings(callout, shieldType, enableDebug, transmitInterval),
		//_SerialPort(SW_SERIAL),
		_lastPingMS(0xFFFFFF00),
		_lastShieldMS(0xFFFFFF00),
		_networkState(STATE_UNKNOWN),
		_nBufRX(0)
	{
		_mySerial = new SoftwareSerial(pinRX, pinTX);
		_mySerial->begin(2400);
	}
#else
	//Hardware Serial Constructor
	SmartThingsThingShield::SmartThingsThingShield(HardwareSerial* hwSerialPort, SmartThingsCallout_t *callout, String shieldType, bool enableDebug, int transmitInterval) :
		SmartThings(callout, shieldType, enableDebug, transmitInterval),
		_mySerial(hwSerialPort),
		_lastPingMS(0xFFFFFF00),
		_lastShieldMS(0xFFFFFF00),
		_networkState(STATE_UNKNOWN),
		_nBufRX(0)
	{
//		_isDebugEnabled = hwSerialPort != Serial ? enableDebug : false;	//Do not allow debug print statements if using Hardware Serial (pins 0,1) for ST Communications
		_mySerial->begin(2400);
	}
#endif
	//*****************************************************************************
	//SmartThings::~SmartThings()
	SmartThingsThingShield::~SmartThingsThingShield()
	{
	}

	//*******************************************************************************
	/// Initialize SmartThings Thingshield Library 
	//*******************************************************************************
	void SmartThingsThingShield::init(void)
	{
		unsigned long tmpTime = millis();
		do
		{
			//updateNetworkState();
			run();
			delay(10);
			updateNetworkState();
		} while ((_networkState != STATE_JOINED) && ((millis() - tmpTime) < 5000));

		if (_isDebugEnabled)
		{
			if (_networkState != STATE_JOINED)
			{
				Serial.println(F("SmartThingsThingShield: Intialization timed out waiting for shield to connect."));
			}
			else
			{
				Serial.println(F("SmartThingsThingShield: Intialization Successful. Shield connected."));
			}
		}
	}


	//*****************************************************************************
	void SmartThingsThingShield::run(void)
	{
		uint8_t readByte;

		while ((_nBufRX < SMARTTHINGS_RX_BUFFER_SIZE) && _mySerial->available())
		{

			readByte = _mySerial->read();

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
	void SmartThingsThingShield::send(String message)
	{
		// e.g. thing.print("raw 0x0 {00 00 0A 0A 62 75 74 74 6f 6e 20 64 6f 77 6e }");
		_mySerial->print(F("raw 0x0 { 00 00 0A 0A "));

		if (_isDebugEnabled)
		{
			Serial.print(F("<-| raw 0x0 { 00 00 0A 0A "));
		}

		for (int i = 0; i < message.length(); i++)
		{
			_mySerial->print(message[i], HEX);
			_mySerial->print(' ');

			if (_isDebugEnabled)
			{
				Serial.print(message[i], HEX);
				Serial.print(' ');
			}
		}

		_mySerial->print(F("}\nsend 0x0 1 1\n"));

		if (_isDebugEnabled)
		{
			Serial.print(F("}\nsend 0x0 1 1\n"));
		}
	}

	//*****************************************************************************
	void SmartThingsThingShield::shieldSetLED(uint8_t red, uint8_t green, uint8_t blue)
	{
		if (red > 9)    red = 9;
		if (green > 9)  green = 9;
		if (blue > 9)   blue = 9;

		_mySerial->print(F("custom rgb "));
		_mySerial->write((red + '0'));
		_mySerial->print(' ');
		_mySerial->write((green + '0'));
		_mySerial->print(' ');
		_mySerial->write((blue + '0'));
		_mySerial->print(F(" \n"));

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
	SmartThingsNetworkState_t SmartThingsThingShield::shieldGetLastNetworkState(void)
	{
		return _networkState;
	}

	//*****************************************************************************
	SmartThingsNetworkState_t SmartThingsThingShield::shieldGetNetworkState(void)
	{
		_shieldGetNetworkInfo();
		return _networkState;
	}

	//*****************************************************************************
	uint16_t SmartThingsThingShield::shieldGetNodeID(void)
	{
		_shieldGetNetworkInfo();
		return _nodeID;
	}

	//*****************************************************************************
	void SmartThingsThingShield::shieldGetEUI64(uint8_t eui[8])
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
	void SmartThingsThingShield::shieldFindNetwork(void)
	{
		_mySerial->print(F("custom find\n"));

		if (_isDebugEnabled)
		{
			Serial.print(F("  |<~ custom find\n"));

		}
	}

	//*****************************************************************************
	void SmartThingsThingShield::shieldLeaveNetwork(void)
	{
		_mySerial->print(F("custom leave\n"));


		if (_isDebugEnabled)
		{
			Serial.print(F("  |<~ custom leave\n"));
		}
	}

	//*******************************************************************************
	// Update the network State/LED
	//*******************************************************************************
	void SmartThingsThingShield::updateNetworkState()		//get the current zigbee network status of the ST Shield
	{
		static SmartThingsNetworkState_t tempState = shieldGetLastNetworkState();

		if (_isDebugEnabled)
		{
			Serial.println(F("Called updateNetworkState()"));
		}
		
		if (tempState != _networkState)
		{
			if (_isDebugEnabled)
			{
				Serial.print(F("Called updateNetworkState() tmpState ="));
				Serial.println(tempState);
			}

			switch (_networkState)
			{
			case STATE_NO_NETWORK:
				if (_isDebugEnabled) Serial.println(F("Everything: NO_NETWORK"));
				shieldSetLED(2, 0, 0); // red
				break;
			case STATE_JOINING:
				if (_isDebugEnabled) Serial.println(F("Everything: JOINING"));
				shieldSetLED(2, 0, 0); // red
				break;
			case STATE_JOINED:
				if (_isDebugEnabled) Serial.println(F("Everything: JOINED"));
				shieldSetLED(0, 0, 0); // off
				break;
			case STATE_JOINED_NOPARENT:
				if (_isDebugEnabled) Serial.println(F("Everything: JOINED_NOPARENT"));
				shieldSetLED(2, 0, 2); // purple
				break;
			case STATE_LEAVING:
				if (_isDebugEnabled) Serial.println(F("Everything: LEAVING"));
				shieldSetLED(2, 0, 0); // red
				break;
			default:
			case STATE_UNKNOWN:
				if (_isDebugEnabled) Serial.println(F("Everything: UNKNOWN"));
				shieldSetLED(0, 2, 0); // green
				break;
			}
			_networkState = tempState;
		}
	}

}