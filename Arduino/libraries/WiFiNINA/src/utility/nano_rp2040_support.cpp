/*
  This file is part of the WiFiNINA library.
  Copyright (c) 2021 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef ARDUINO_NANO_RP2040_CONNECT

/******************************************************************************
 * INCLUDE
 ******************************************************************************/

#include "nina_pins.h" /* variants/NANO_RP2040_CONNECT/ninaPins.h */
#include "wifi_drv.h"

/******************************************************************************
 * FUNCTION DEFINITION
 ******************************************************************************/

#ifdef NINA_PINS_AS_CLASS
#define VAL(x) x.get()
#else
#define VAL(x) static_cast<uint8_t>(x)
#endif

uint8_t toAnalogPin(NinaPin pin)
{
  if      (pin == A4) return 6; /* ADC1 - CH6 */
  else if (pin == A5) return 3; /* ADC1 - CH3 */
  else if (pin == A6) return 0; /* ADC1 - CH0 */
  else if (pin == A7) return 7; /* ADC1 - CH7 */
  else                return 0xFF;
}

void pinMode(NinaPin pin, PinMode mode)
{
  WiFiDrv::pinMode(VAL(pin), static_cast<uint8_t>(mode));
}

PinStatus digitalRead(NinaPin pin)
{
  return WiFiDrv::digitalRead(VAL(pin));
}

void digitalWrite(NinaPin pin, PinStatus value)
{
  if (value == LOW)
    WiFiDrv::digitalWrite(VAL(pin), 1);
  else
    WiFiDrv::digitalWrite(VAL(pin), 0);
}

int analogRead(NinaPin pin)
{
  uint8_t const adc_channel = toAnalogPin(pin);

  if (adc_channel == 0xFF)
    return 0;
  else
#ifdef NINA_PINS_AS_CLASS
    return WiFiDrv::analogRead(adc_channel) >> (12 - pin.analogReadResolution());
#else
    return WiFiDrv::analogRead(adc_channel);
#endif
}

void analogWrite(NinaPin pin, int value)
{
  WiFiDrv::analogWrite(VAL(pin), static_cast<uint8_t>(value));
}

#endif /* ARDUINO_NANO_RP2040_CONNECT */
