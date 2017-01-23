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
 *  Summary:  EX_Chime is a class which implements the SmartThings "Tone" device capability,
 *            and produces a door chime melody, or an arbitrary user defined melody.
 */
#include "EX_Chime.h"

#include "Constants.h"
#include "Everything.h"

namespace st
{
    static const unsigned int EX_Chime::DEFAULT_NOTES[] = { NOTE_FS7, NOTE_D7 };
    static const unsigned int EX_Chime::DEFAULT_DURATIONS[] = { 3, 2 };

    /*
     * Constructs the EX_Chime instance.
     *    name - name of the device
     *    pin - pin number of the output pin that has a piezzo buzzer, or amplified speaker connected
     *    size - size of the note and duration arrays
     *    notes - an array of melody notes (optional, defaults to { NOTE_FS7, NOTE_D7 })
     *    durations - an array of note durations in note lengths (optional, defaults to { 3, 2 })
     *          e.g. 1 = full note, 2 = half note 4 = quarter note, 8 = eighth note
     */
    EX_Chime::EX_Chime(const __FlashStringHelper *name, byte pin, unsigned int size, unsigned int notes[], unsigned int durations[]) :
            EX_Tone(name, pin), m_size(size), m_notes(notes)
    {
    }

    /*
     * Generate the note melody.
     */
    void EX_Chime::beep() const
    {
        // iterate over the notes of the melody:
        for (unsigned int index = 0; index < m_size; index++)
        {
            // calculate the note duration
            unsigned int duration = MILLIS_PER_SECOND / (m_durations[index] * TEMPO_BPS);
            tone(m_notes[index], duration);

            // pause between notes
            delay(duration);
        }
    }
}
