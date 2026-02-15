#include <Wire.h>
#include "Adafruit_TCS34725.h"

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

void setup(void) {
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Now we're ready to get readings!
}

void loop(void) {
  uint16_t r, g, b, c;

  tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_614MS);
  delay(154); // Delay for one old integ. time period (to finish old reading)
  delay(615); // Delay for one new integ. time period (to allow new reading)
  tcs.getRawData(&r, &g, &b, &c);
  Serial.print("Integ. time: 614.4ms ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");

  tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_300MS);
  delay(615); // Delay for one old integ. time period (to finish old reading)
  delay(300); // Delay for one new integ. time period (to allow new reading)
  tcs.getRawData(&r, &g, &b, &c);
  Serial.print("Integ. time: 300.0ms ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");

  tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_154MS);
  delay(300); // Delay for one old integ. time period (to finish old reading)
  delay(154); // Delay for one new integ. time period (to allow new reading)
  tcs.getRawData(&r, &g, &b, &c);
  Serial.print("Integ. time: 153.6ms ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
}
