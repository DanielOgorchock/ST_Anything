/*!
 *  @file Adafruit_VEML7700.h
 *
 * 	I2C Driver for VEML7700 Lux sensor
 *
 * 	This is a library for the Adafruit VEML7700 breakout:
 * 	http://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_VEML7700_H
#define _ADAFRUIT_VEML7700_H

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>

#define VEML7700_I2CADDR_DEFAULT 0x10 ///< I2C address

#define VEML7700_ALS_CONFIG          0x00
#define VEML7700_ALS_THREHOLD_HIGH   0x01
#define VEML7700_ALS_THREHOLD_LOW    0x02
#define VEML7700_ALS_POWER_SAVE      0x03
#define VEML7700_ALS_DATA            0x04
#define VEML7700_WHITE_DATA          0x05
#define VEML7700_INTERRUPTSTATUS     0x06

#define VEML7700_INTERRUPT_HIGH     0x4000
#define VEML7700_INTERRUPT_LOW      0x8000

#define VEML7700_GAIN_1             0x00
#define VEML7700_GAIN_2             0x01
#define VEML7700_GAIN_1_8           0x02
#define VEML7700_GAIN_1_4           0x03

#define VEML7700_IT_100MS           0x00
#define VEML7700_IT_200MS           0x01
#define VEML7700_IT_400MS           0x02
#define VEML7700_IT_800MS           0x03
#define VEML7700_IT_50MS            0x08
#define VEML7700_IT_25MS            0x0C

#define VEML7700_PERS_1             0x00
#define VEML7700_PERS_2             0x01
#define VEML7700_PERS_4             0x02
#define VEML7700_PERS_8             0x03

#define VEML7700_POWERSAVE_MODE1    0x00
#define VEML7700_POWERSAVE_MODE2    0x01
#define VEML7700_POWERSAVE_MODE3    0x02
#define VEML7700_POWERSAVE_MODE4    0x03


/*!
 *    @brief  Class that stores state and functions for interacting with
 *            VEML7700 Temp Sensor
 */
class Adafruit_VEML7700 {
public:
  Adafruit_VEML7700();
  boolean begin(TwoWire *theWire = &Wire);

  void enable(bool enable);
  bool enabled(void);

  void interruptEnable(bool enable);
  bool interruptEnabled(void);
  void setPersistence(uint8_t pers);
  uint8_t getPersistence(void);
  void setIntegrationTime(uint8_t it);
  uint8_t getIntegrationTime(void);
  void setGain(uint8_t gain);
  uint8_t getGain(void);
  void powerSaveEnable(bool enable);
  bool powerSaveEnabled(void);
  void setPowerSaveMode(uint8_t mode);
  uint8_t getPowerSaveMode(void);
  
  void setLowThreshold(uint16_t value);
  uint16_t getLowThreshold(void);
  void setHighThreshold(uint16_t value);
  uint16_t getHighThreshold(void);
  uint16_t interruptStatus(void);


  float readLux();
  uint16_t readALS();
  float readWhite();

  Adafruit_I2CRegister *ALS_Config, *ALS_Data, *White_Data, 
    *ALS_HighThreshold, *ALS_LowThreshold, *Power_Saving, *Interrupt_Status;
  Adafruit_I2CRegisterBits *ALS_Shutdown, *ALS_Interrupt_Enable, 
    *ALS_Persistence, *ALS_Integration_Time, *ALS_Gain,
    *PowerSave_Enable, *PowerSave_Mode;


private:
  float normalize(float value);

  Adafruit_I2CDevice *i2c_dev;

};

#endif
