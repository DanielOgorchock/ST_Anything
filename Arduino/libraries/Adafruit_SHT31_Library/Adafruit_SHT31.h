/*!
 *  @file Adafruit_SHT31.h
 *
 *  This is a library for the SHT31 Digital Humidity & Temp Sensor
 *
 *  Designed specifically to work with the  Digital Humidity & Temp Sensor
 *  -----> https://www.adafruit.com/product/2857
 *
 *  These sensors use I2C to communicate, 2 pins are required to interface
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_SHT31_H
#define ADAFRUIT_SHT31_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#define SHT31_DEFAULT_ADDR 0x44 /**< SHT31 Default Address */
#define SHT31_MEAS_HIGHREP_STRETCH                                             \
  0x2C06 /**< Measurement High Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_MEDREP_STRETCH                                              \
  0x2C0D /**< Measurement Medium Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_LOWREP_STRETCH                                              \
  0x2C10 /**< Measurement Low Repeatability with Clock Stretch Enabled*/
#define SHT31_MEAS_HIGHREP                                                     \
  0x2400 /**< Measurement High Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_MEDREP                                                      \
  0x240B /**< Measurement Medium Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_LOWREP                                                      \
  0x2416 /**< Measurement Low Repeatability with Clock Stretch Disabled */
#define SHT31_READSTATUS 0xF32D   /**< Read Out of Status Register */
#define SHT31_CLEARSTATUS 0x3041  /**< Clear Status */
#define SHT31_SOFTRESET 0x30A2    /**< Soft Reset */
#define SHT31_HEATEREN 0x306D     /**< Heater Enable */
#define SHT31_HEATERDIS 0x3066    /**< Heater Disable */
#define SHT31_REG_HEATER_BIT 0x0d /**< Status Register Heater Bit */

extern TwoWire Wire; /**< Forward declarations of Wire for board/variant
                        combinations that don't have a default 'Wire' */

/**
 * Driver for the Adafruit SHT31-D Temperature and Humidity breakout board.
 */
class Adafruit_SHT31 {
public:
  Adafruit_SHT31(TwoWire *theWire = &Wire);
  ~Adafruit_SHT31();

  bool begin(uint8_t i2caddr = SHT31_DEFAULT_ADDR);
  float readTemperature(void);
  float readHumidity(void);
  bool readBoth(float *temperature_out, float *humidity_out);
  uint16_t readStatus(void);
  void reset(void);
  void heater(bool h);
  bool isHeaterEnabled();

private:
  /**
   * Placeholder to track humidity internally.
   */
  float humidity;

  /**
   * Placeholder to track temperature internally.
   */
  float temp;

  bool readTempHum(void);
  bool writeCommand(uint16_t cmd);

  TwoWire *_wire;                     /**< Wire object */
  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#endif
