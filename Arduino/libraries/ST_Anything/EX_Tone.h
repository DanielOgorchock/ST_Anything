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
#ifndef ST_EX_Tone_H
#define ST_EX_Tone_H

#include "Executor.h"

namespace st
{
    class EX_Tone: public Executor
    {
        private:
            byte m_pin;
            unsigned int m_frequency;
            unsigned long m_duration;

        public:
            // constructor
            EX_Tone(const __FlashStringHelper *name, byte pin, unsigned int frequency = 400, unsigned long duration = 1000);

            // virtuals
            virtual void init();
            virtual void beSmart(const String &str);
            virtual void beep() const;

            // getters
            inline byte getPin() const { return m_pin; }
            inline unsigned int getFrequency() const { return m_frequency; }
            inline unsigned long getDuration() const { return m_duration; }
    };
}
#endif
