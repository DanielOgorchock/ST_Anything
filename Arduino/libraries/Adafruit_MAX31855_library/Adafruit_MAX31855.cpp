/*!
 * @file Adafruit_MAX31855.cpp
 *
 * @mainpage Adafruit MAX31855 Thermocouple Breakout Driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's MAX31855 thermocouple breakout
 * driver for the Arduino platform.  It is designed specifically to work with
 * the Adafruit MAX31855 breakout: https://www.adafruit.com/products/269
 *
 * This breakout uses SPI to communicate, 3 pins are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit_BusIO"> Adafruit_BusIO</a> being
 * present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#include "Adafruit_MAX31855.h"
#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include <stdlib.h>

/**************************************************************************/
/*!
    @brief  Instantiates a new Adafruit_MAX31855 class using software SPI.

    @param _sclk The pin to use for SPI Serial Clock.
    @param _cs The pin to use for SPI Chip Select.
    @param _miso The pin to use for SPI Master In Slave Out.
*/
/**************************************************************************/
Adafruit_MAX31855::Adafruit_MAX31855(int8_t _sclk, int8_t _cs, int8_t _miso)
    : spi_dev(_cs, _sclk, _miso, -1, 1000000) {}

/**************************************************************************/
/*!
    @brief  Instantiates a new Adafruit_MAX31855 class using hardware SPI.

    @param _cs The pin to use for SPI Chip Select.
    @param _spi which spi buss to use.
*/
/**************************************************************************/
Adafruit_MAX31855::Adafruit_MAX31855(int8_t _cs, SPIClass *_spi)
    : spi_dev(_cs, 1000000, SPI_BITORDER_MSBFIRST, SPI_MODE0, _spi) {}

/**************************************************************************/
/*!
    @brief  Setup the HW

    @return True if the device was successfully initialized, otherwise false.
*/
/**************************************************************************/
bool Adafruit_MAX31855::begin(void) {
  initialized = spi_dev.begin();

  return initialized;
}

/**************************************************************************/
/*!
    @brief  Read the internal temperature.

    @return The internal temperature in degrees Celsius.
*/
/**************************************************************************/
double Adafruit_MAX31855::readInternal(void) {
  uint32_t v;

  v = spiread32();

  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  // Serial.print("\tInternal Temp: "); Serial.println(internal);
  return internal;
}

/**************************************************************************/
/*!
    @brief  Read the thermocouple temperature.

    @return The thermocouple temperature in degrees Celsius.
*/
/**************************************************************************/
double Adafruit_MAX31855::readCelsius(void) {

  int32_t v;

  v = spiread32();

  // Serial.print("0x"); Serial.println(v, HEX);

  /*
  float internal = (v >> 4) & 0x7FF;
  internal *= 0.0625;
  if ((v >> 4) & 0x800)
    internal *= -1;
  Serial.print("\tInternal Temp: "); Serial.println(internal);
  */

  if (v & faultMask) {
    // uh oh, a serious problem!
    return NAN;
  }

  if (v & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFF);
  } else {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }
  // Serial.println(v, HEX);

  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  return centigrade;
}

/**************************************************************************/
/*!
    @brief  Read the error state.

    @return The error state.
*/
/**************************************************************************/
uint8_t Adafruit_MAX31855::readError() { return spiread32() & 0x7; }

/**************************************************************************/
/*!
    @brief  Read the thermocouple temperature.

    @return The thermocouple temperature in degrees Fahrenheit.
*/
/**************************************************************************/
double Adafruit_MAX31855::readFahrenheit(void) {
  float f = readCelsius();
  f *= 9.0;
  f /= 5.0;
  f += 32;
  return f;
}

/**************************************************************************/
/*!
    @brief  Set the faults to check when reading temperature. If any set
    faults occur, temperature reading will return NAN.

    @param faults Faults to check. Use logical OR combinations of preset
    fault masks: MAX31855_FAULT_OPEN, MAX31855_FAULT_SHORT_GND,
    MAX31855_FAULT_SHORT_VCC. Can also enable/disable all fault checks
    using: MAX31855_FAULT_ALL or MAX31855_FAULT_NONE.
*/
/**************************************************************************/
void Adafruit_MAX31855::setFaultChecks(uint8_t faults) {
  faultMask = faults & 0x07;
}

/**************************************************************************/
/*!
    @brief  Read 4 bytes (32 bits) from breakout over SPI.

    @return The raw 32 bit value read.
*/
/**************************************************************************/
uint32_t Adafruit_MAX31855::spiread32(void) {
  uint32_t d = 0;
  uint8_t buf[4];

  // backcompatibility!
  if (!initialized) {
    begin();
  }

  spi_dev.read(buf, 4);

  d = buf[0];
  d <<= 8;
  d |= buf[1];
  d <<= 8;
  d |= buf[2];
  d <<= 8;
  d |= buf[3];

  // Serial.println(d, HEX);

  return d;
}
