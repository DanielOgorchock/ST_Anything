#ifndef ST_ANALOGOUTPUT_H
#define ST_ANALOGOUTPUT_H

#include "Constants.h"
#include <stdint.h>

namespace st
{
    typedef uint8_t analog_output_unit;
    class AnalogOutput
    {
        private:
            pin_unit m_nPin;

        public:
            AnalogOutput(pin_unit pin, analog_output_unit initial=0);

            void setPin(pin_unit pin);
            void write(analog_output_unit val);
            void operator()(analog_output_unit val);

    };
}

#endif
