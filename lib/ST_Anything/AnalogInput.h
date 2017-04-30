#ifndef ST_ANALOGINPUT_H
#define ST_ANALOGINPUT_H

#include "Constants.h"
#include <stdint.h>

namespace st
{
    typedef uint16_t analog_input_unit;
    class AnalogInput
    {
        private:
        pin_unit m_nPin;

        public:
        AnalogInput(pin_unit pin);

        void setPin(pin_unit pin);

        analog_input_unit read();
        analog_input_unit operator()();
        
    };
}

#endif
