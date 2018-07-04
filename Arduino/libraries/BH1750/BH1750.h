/*

  This is a library for the BH1750FVI Digital Light Sensor
  breakout board.

  The BH1750 board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The BH1750 library doesn't do this automatically.

  Datasheet: http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1750fvi-e.pdf

  Written by Christopher Laws, March, 2013.

  Modified by Dan Ogorchock on 2018-07-04 for ST_Anything compatability
*/

#ifndef BH1750_h
#define BH1750_h

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include "Wire.h"

// Uncomment, to enable debug messages
// #define BH1750_DEBUG

// No active state
#define BH1750_POWER_DOWN 0x00

// Wating for measurement command
#define BH1750_POWER_ON 0x01

// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_RESET 0x07

//I2C Addresses
#define BH1750_ADDR_LOW  0x23
#define BH1750_ADDR_HIGH 0x5C

class BH1750 {

  public:

    enum Mode
    {
      UNCONFIGURED = 0,
      // Measurement at 1lx resolution. Measurement time is approx 120ms.
      CONTINUOUS_HIGH_RES_MODE  = 0x10,
      // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
      CONTINUOUS_HIGH_RES_MODE_2 = 0x11,
      // Measurement at 4lx resolution. Measurement time is approx 16ms.
      CONTINUOUS_LOW_RES_MODE = 0x13,
      // Measurement at 1lx resolution. Measurement time is approx 120ms.
      ONE_TIME_HIGH_RES_MODE = 0x20,
      // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
      ONE_TIME_HIGH_RES_MODE_2 = 0x21,
      // Measurement at 1lx resolution. Measurement time is approx 120ms.
      ONE_TIME_LOW_RES_MODE = 0x23
    };

    BH1750(byte addr = BH1750_ADDR_LOW);
    bool begin(Mode mode = CONTINUOUS_HIGH_RES_MODE);
    bool configure(Mode mode);
    uint16_t readLightLevel(bool maxWait = false);

  private:
    int BH1750_I2CADDR;
    Mode BH1750_MODE = UNCONFIGURED;

};

#endif
