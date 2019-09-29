/*!
 *  @file Adafruit_VEML7700.cpp
 *
 *  @mainpage Adafruit VEML7700 I2C Lux Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the VEML7700 I2C Lux sensor
 *
 * 	This is a library for the Adafruit VEML7700 breakout:
 * 	http://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_VEML7700.h"

/*!
 *    @brief  Instantiates a new VEML7700 class
 */
Adafruit_VEML7700::Adafruit_VEML7700(void) {}

/*!
 *    @brief  Setups the HW
 *    @param  addr
 *    @return True if initialization was successful, otherwise false.
 */
boolean Adafruit_VEML7700::begin(TwoWire *theWire) {
  i2c_dev = new Adafruit_I2CDevice(VEML7700_I2CADDR_DEFAULT);

  if (!i2c_dev->begin()) {
    return false;
  }

  ALS_Config = new Adafruit_I2CRegister(i2c_dev, VEML7700_ALS_CONFIG, 2, LSBFIRST);
  ALS_HighThreshold = new Adafruit_I2CRegister(i2c_dev, VEML7700_ALS_THREHOLD_HIGH, 2, LSBFIRST);
  ALS_LowThreshold = new Adafruit_I2CRegister(i2c_dev, VEML7700_ALS_THREHOLD_LOW, 2, LSBFIRST);
  Power_Saving =  new Adafruit_I2CRegister(i2c_dev, VEML7700_ALS_POWER_SAVE, 2, LSBFIRST);
  ALS_Data = new Adafruit_I2CRegister(i2c_dev, VEML7700_ALS_DATA, 2, LSBFIRST);
  White_Data = new Adafruit_I2CRegister(i2c_dev, VEML7700_WHITE_DATA, 2, LSBFIRST);
  Interrupt_Status = new Adafruit_I2CRegister(i2c_dev, VEML7700_INTERRUPTSTATUS, 2, LSBFIRST);

  ALS_Shutdown = new Adafruit_I2CRegisterBits(ALS_Config, 1, 0);  // # bits, bit_shift
  ALS_Interrupt_Enable = new Adafruit_I2CRegisterBits(ALS_Config, 1, 1); 
  ALS_Persistence = new Adafruit_I2CRegisterBits(ALS_Config, 2, 4); 
  ALS_Integration_Time = new Adafruit_I2CRegisterBits(ALS_Config, 4, 6); 
  ALS_Gain = new Adafruit_I2CRegisterBits(ALS_Config, 2, 11); 
  PowerSave_Enable = new Adafruit_I2CRegisterBits(Power_Saving, 1, 0);
  PowerSave_Mode = new Adafruit_I2CRegisterBits(Power_Saving, 2, 1);

  enable(false);
  interruptEnable(false);
  setPersistence(VEML7700_PERS_1);
  setGain(VEML7700_GAIN_1);
  setIntegrationTime(VEML7700_IT_100MS);
  powerSaveEnable(false);
  enable(true);

  return true;
}


float Adafruit_VEML7700::normalize(float value) {
  // adjust for gain (1x is normalized)
  switch (getGain()) {
  case VEML7700_GAIN_2:  
    value /= 2.0;  break;
  case VEML7700_GAIN_1_4: 
    value *= 4; break;
  case VEML7700_GAIN_1_8:
    value *= 8; break;
  }
  
  // adjust for integrationtime (100ms is normalized)
  switch (getIntegrationTime()) {
  case VEML7700_IT_25MS:
    value *= 4; break;
  case VEML7700_IT_50MS:
    value *= 2; break;
  case VEML7700_IT_200MS:
    value /= 2.0; break;
  case VEML7700_IT_400MS:
    value /= 4.0; break;
  case VEML7700_IT_800MS:
    value /= 8.0; break;
  }

  return value;
}

float Adafruit_VEML7700::readLux() {
  return normalize(ALS_Data->read()) * 0.0576;  // see app note lux table on page 5
}

uint16_t Adafruit_VEML7700::readALS() {
  return ALS_Data->read();
}


float Adafruit_VEML7700::readWhite() {
  return normalize(White_Data->read()) * 0.0576; // Unclear if this is the right multiplier
}


void Adafruit_VEML7700::enable(bool enable) {
  ALS_Shutdown->write(!enable);
}

bool Adafruit_VEML7700::enabled(void) {
  return !ALS_Shutdown->read();
}

void Adafruit_VEML7700::interruptEnable(bool enable) {
  ALS_Interrupt_Enable->write(enable);
}

bool Adafruit_VEML7700::interruptEnabled(void) {
  return ALS_Interrupt_Enable->read();
}

void Adafruit_VEML7700::setPersistence(uint8_t pers) {
  ALS_Persistence->write(pers);
}

uint8_t Adafruit_VEML7700::getPersistence(void) {
  return ALS_Persistence->read();
}

void Adafruit_VEML7700::setIntegrationTime(uint8_t it) {
  ALS_Integration_Time->write(it);
}

uint8_t Adafruit_VEML7700::getIntegrationTime(void) {
  return ALS_Integration_Time->read();
}

void Adafruit_VEML7700::setGain(uint8_t gain) {
  ALS_Gain->write(gain);
}

uint8_t Adafruit_VEML7700::getGain(void) {
  return ALS_Gain->read();
}

void Adafruit_VEML7700::powerSaveEnable(bool enable) {
  PowerSave_Enable->write(enable);
}

bool Adafruit_VEML7700::powerSaveEnabled(void) {
  return PowerSave_Enable->read();
}

void Adafruit_VEML7700::setPowerSaveMode(uint8_t mode) {
  PowerSave_Mode->write(mode);
}

uint8_t Adafruit_VEML7700::getPowerSaveMode(void) {
  return PowerSave_Mode->read();
}

void Adafruit_VEML7700::setLowThreshold(uint16_t value) {
  ALS_LowThreshold->write(value);
}

uint16_t Adafruit_VEML7700::getLowThreshold(void) {
  return ALS_LowThreshold->read();
}


void Adafruit_VEML7700::setHighThreshold(uint16_t value) {
  ALS_HighThreshold->write(value);
}

uint16_t Adafruit_VEML7700::getHighThreshold(void) {
  return ALS_HighThreshold->read();
}

uint16_t Adafruit_VEML7700::interruptStatus(void) {
  return Interrupt_Status->read();
}
