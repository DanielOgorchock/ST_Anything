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
#ifndef ST_EX_Chime_H
#define ST_EX_Chime_H

#include "Executor.h"
#include "Pitches.h"
#include "EX_Tone.h"

#define TEMPO_BPM 60
#define TEMPO_BPS TEMPO_BPM / 60
#define MILLIS_PER_SECOND 1000


namespace st
{
	class EX_Chime : public EX_Tone
	{

	private:
	    unsigned int m_size;
	    unsigned int* m_notes;
	    unsigned int* m_durations;
	    static const unsigned int DEFAULT_NOTES[];
        static const unsigned int DEFAULT_DURATIONS[];

	public:
		//constructor
		EX_Chime(const __FlashStringHelper *name, byte pin, unsigned int size = 2, unsigned int notes[] = DEFAULT_NOTES, unsigned int durations[] = DEFAULT_DURATIONS);

		// virtuals
        virtual void beep() const override;
	};
}
#endif
