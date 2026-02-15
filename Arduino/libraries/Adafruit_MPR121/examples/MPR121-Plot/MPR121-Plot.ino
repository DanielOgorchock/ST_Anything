/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required
to interface



based on MPR121test
modified & extended by Stefan Krüger


**********************************************************/
#include <Adafruit_MPR121.h>
#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();

const uint8_t channelFirst = 0;
const uint8_t channelLast = 5; // max 13
uint16_t currTouched;

void plotValues() {
  currTouched = cap.touched();

  for (int chan = channelFirst; chan <= channelLast; chan++) {
    if (currTouched & _BV(chan)) {
      Serial.print("550,");
    } else {
      Serial.print("0,");
    }
    Serial.print(cap.filteredData(chan));
    Serial.print(",");
    Serial.print(cap.filteredData(chan));
    Serial.print(",  ");

    // uint8_t reg = cap.readRegister8(0x5F + chan);
    // if (reg < 10)
    //   Serial.print(" ");
    // Serial.print(reg);
  }
  Serial.println();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("MPR121 Autoconfiguration Test. (MPR121-AutoConfig.ino)");
  Wire.begin();
  if (!cap.begin(0x5A, &Wire)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  cap.setAutoconfig(true);
}

void loop() {
  plotValues();
  delay(10);
}