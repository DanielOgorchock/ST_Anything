/*
 *  Copyright 2017 Imprimis LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 *  Authors: Jiri Culik
 *  Summary:  EX_Tone is a class which implements the SmartThings "Tone" device capability.
 */
#include "EX_Tone.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
    /*
     * Constructs the EX_Tone instance.
     *    name - name of the device
     *    pin - pin number of the output pin that has a piezzo buzzer, or amplified speaker connected
     *    frequency - the tone frequency in Hz (optional, defaults to 400Hz)
     *    duration - the tone duration in milliseconds (optional, defaults to 1000ms)
     */
    EX_Tone::EX_Tone(const __FlashStringHelper *name, byte pin, unsigned int frequency, unsigned long duration) :
            Executor(name), m_pin(pin), m_frequency(frequency), m_duration(duration)
    {
    }

    /*
     * Initialize the hardware
     */
    void EX_Tone::init()
    {
        pinMode(m_pin, OUTPUT);
    }

    /*
     * Parse the command messages from the SmartThings shield.
     */
    void EX_Tone::beSmart(const String &str)
    {
        String s = str.substring(str.indexOf(' ') + 1);
        if (debug)
        {
            Serial.print(F("EX_Tone::beSmart s = "));
            Serial.println(s);
        }
        if ((s == F("beep")))
        {
            beep();
        }
    }

    /*
     * Generates a beep tone.
     */
    void EX_Tone::beep() const
    {
        tone(m_pin, m_frequency, m_duration);
    }
}
