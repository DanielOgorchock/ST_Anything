/***************************************************
  This is a library for the SHT31 Digital Humidity & Temp Sensor

  Designed specifically to work with the SHT31 Digital sensor from Adafruit
  ----> https://www.adafruit.com/products/2857

  These sensors use I2C to communicate, 2 pins are required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef ADAFRUIT_SHT31_H
#define ADAFRUIT_SHT31_H

#include "Arduino.h"
#include "Wire.h"

#define SHT31_DEFAULT_ADDR         0x44
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066

/**
 * Driver for the Adafruit SHT31-D Temperature and Humidity breakout board.
 */
class Adafruit_SHT31 {
    public:
        /**
         *  Constructor.
         */
        Adafruit_SHT31();

        /**
         * Initialises the I2C bus, and assigns the I2C address to us.
         *
         * @param i2caddr   The I2C address to use for the sensor.
         *
         * @return True if initialisation was successful, otherwise False.
         */
        boolean begin(uint8_t i2caddr = SHT31_DEFAULT_ADDR);

        /**
         * Gets a single temperature reading from the sensor.
         *
         * @return A float value indicating the temperature.
         */
        float readTemperature(void);

        /**
         * Gets a single relative humidity reading from the sensor.
         *
         * @return A float value representing relative humidity.
         */
        float readHumidity(void);

        /**
         * Gets the current status register contents.
         *
         * @return The 16-bit status register.
         */
        uint16_t readStatus(void);

        /**
         * Performs a reset of the sensor to put it into a known state.
         */
        void reset(void);

        /**
         * Enables or disabled the heating element.
         *
         * @param h True to enable the heater, False to disable it.
         */
        void heater(boolean h);

        /**
         * Performs a CRC8 calculation on the supplied values.
         *
         * @param data  Pointer to the data to use when calculating the CRC8.
         * @param len   The number of bytes in 'data'.
         *
         * @return The computed CRC8 value.
         */
        uint8_t crc8(const uint8_t *data, int len);

    private:
        /**
         * Placeholder to track the I2C address.
         */
        uint8_t _i2caddr;

        /**
         * Placeholder to track humidity internally.
         */
        float humidity;

        /**
         * Placeholder to track temperature internally.
         */
        float temp;

        /**
         * Internal function to perform a temp + humidity read.
         *
         * @return True if successful, otherwise false.
         */
        boolean  readTempHum(void);

        /**
         * Internal function to perform and I2C write.
         *
         * @param cmd   The 16-bit command ID to send.
         */
        void     writeCommand(uint16_t cmd);

        /**
         * Internal function to read data over the I2C bus.
         *
         * @return True if successful, otherwise False.
         */
        boolean  readData(void);
};

#endif
