/*!
 * @file Adafruit_MAX31855.h
 *
 * This is the documentation for Adafruit's MAX31855 thermocouple breakout
 * driver for the Arduino platform.  It is designed specifically to work with
 * the Adafruit MAX31855 breakout: https://www.adafruit.com/products/269
 *
 * These sensors use SPI to communicate, 3 pins are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * BSD license, all text above must be included in any redistribution.
 *
 */

#ifndef ADAFRUIT_MAX31855_H
#define ADAFRUIT_MAX31855_H

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_SPIDevice.h>

#define MAX31855_FAULT_NONE (0x00)      ///< Disable all fault checks
#define MAX31855_FAULT_OPEN (0x01)      ///< Enable open circuit fault check
#define MAX31855_FAULT_SHORT_GND (0x02) ///< Enable short to GND fault check
#define MAX31855_FAULT_SHORT_VCC (0x04) ///< Enable short to VCC fault check
#define MAX31855_FAULT_ALL (0x07)       ///< Enable all fault checks

/**************************************************************************/
/*!
    @brief  Sensor driver for the Adafruit MAX31855 thermocouple breakout.
*/
/**************************************************************************/
class Adafruit_MAX31855 {
public:
  Adafruit_MAX31855(int8_t _sclk, int8_t _cs, int8_t _miso);
  Adafruit_MAX31855(int8_t _cs, SPIClass *_spi = &SPI);

  bool begin(void);
  double readInternal(void);
  double readCelsius(void);
  double readFahrenheit(void);
  uint8_t readError();
  void setFaultChecks(uint8_t faults);

private:
  Adafruit_SPIDevice spi_dev;
  bool initialized = false;
  uint8_t faultMask = MAX31855_FAULT_ALL;
  uint32_t spiread32(void);
};

#endif
