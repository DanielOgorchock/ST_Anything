/*

  This is a library for the BH1750FVI Digital Light Sensor breakout board.

  The BH1750 board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The BH1750 library doesn't do this automatically.

  Written by Christopher Laws, March, 2013.

  Modified by Dan Ogorchock on 2018-07-04 for ST_Anything compatability

*/

#include "BH1750.h"

// Define milliseconds delay for ESP8266 platform
#if defined(ESP8266)

  #include <pgmspace.h>
  #define _delay_ms(ms) delayMicroseconds((ms) * 1000)

// Use _delay_ms from utils for AVR-based platforms
#elif defined(__avr__)
  #include <util/delay.h>

// Use Wiring's delay for compability with another platforms
#else
  #define _delay_ms(ms) delay(ms)
#endif


// Legacy Wire.write() function fix
#if (ARDUINO >= 100)
  #define __wire_write(d) Wire.write(d)
#else
  #define __wire_write(d) Wire.send(d)
#endif


// Legacy Wire.read() function fix
#if (ARDUINO >= 100)
  #define __wire_read() Wire.read()
#else
  #define __wire_read() Wire.receive()
#endif


/**
 * Constructor
 * @params addr Sensor address (0x23 or 0x5C, see datasheet)
 *
 * Default for BH1750 is 0x23
 */
BH1750::BH1750(byte addr) {

  BH1750_I2CADDR = addr;

}


/**
 * Configure sensor
 * @param mode Measurement mode
 */
bool BH1750::begin(Mode mode) {

  // I2C is expected to be initialized outside this library
	//DGO Revision for ST_Anything compatability - all other I2C sensor initialize I2C.  
	//    This one was the exception, but I have modified it to be like the others.
  Wire.begin();
  // Configure sensor in specified mode
  return configure(mode);

}


/**
 * Configure BH1750 with specified mode
 * @param mode Measurement mode
 */
bool BH1750::configure(Mode mode) {

  // default transmission result to a value out of normal range
  byte ack = 5;

  // Check measurement mode is valid
  switch (mode) {

    case BH1750::CONTINUOUS_HIGH_RES_MODE:
    case BH1750::CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750::CONTINUOUS_LOW_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE_2:
    case BH1750::ONE_TIME_LOW_RES_MODE:

      // Send mode to sensor
      Wire.beginTransmission(BH1750_I2CADDR);
      __wire_write((uint8_t)BH1750_MODE);
      ack = Wire.endTransmission();

      // Wait a few moments to wake up
      _delay_ms(10);
      break;

    default:
      // Invalid measurement mode
      Serial.println(F("[BH1750] ERROR: Invalid mode"));
      break;

  }

  // Check result code
  switch (ack) {
    case 0:
      BH1750_MODE = mode;
      return true;
    case 1: // too long for transmit buffer
      Serial.println(F("[BH1750] ERROR: too long for transmit buffer"));
    case 2: // received NACK on transmit of address
      Serial.println(F("[BH1750] ERROR: received NACK on transmit of address"));
    case 3: // received NACK on transmit of data
      Serial.println(F("[BH1750] ERROR: received NACK on transmit of data"));
    case 4: // other error
      Serial.println(F("[BH1750] ERROR: other error"));
    default:
      Serial.println(F("[BH1750] ERROR: undefined error"));
      break;
  }

  return false;

}


/**
 * Read light level from sensor
 * @return Light level in lux (0 ~ 65535)
 */
uint16_t BH1750::readLightLevel(bool maxWait) {

  if (BH1750_MODE == UNCONFIGURED) {
    Serial.println(F("[BH1750] Device is not configured!"));
    return 0;
  }

  // Measurement result will be stored here
  uint16_t level=65535;

  // Send mode to sensor
  Wire.beginTransmission(BH1750_I2CADDR);
  __wire_write((uint8_t)BH1750_MODE);
  Wire.endTransmission();

  // Wait for measurement to be taken.
  // Measurements have a maximum measurement time and a typical measurement
  // time. The maxWait argument determines which measurement wait time is
  // used when a one-time mode is being used. The typical (shorter)
  // measurement time is used by default and if maxWait is set to True then
  // the maximum measurement time will be used. See data sheet pages 2, 5
  // and 7 for more details.
  // A continuous mode measurement can be read immediately after re-sending
  // the mode command.

  switch (BH1750_MODE) {

    case BH1750::ONE_TIME_LOW_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE_2:

      if (BH1750_MODE == BH1750::ONE_TIME_LOW_RES_MODE) {
        maxWait ? _delay_ms(24) : _delay_ms(16);
      }
      else {
        maxWait ? _delay_ms(180) :_delay_ms(120);
      }
      break;
    default: break;
  }

  // Read two bytes from the sensor, which are low and high parts of the sensor
  // value
  Wire.requestFrom(BH1750_I2CADDR, 2);
  if (Wire.available() == 2) {
    level = __wire_read();
    level <<= 8;
    level |= __wire_read();
  }

  // Print raw value if debug enabled
  #ifdef BH1750_DEBUG
  Serial.print(F("[BH1750] Raw value: "));
  Serial.println(level);
  #endif

  // Convert raw value to lux
  level /= 1.2;

  // Print converted value if debug enabled
  #ifdef BH1750_DEBUG
  Serial.print(F("[BH1750] Converted value: "));
  Serial.println(level);
  #endif

  return level;

}
