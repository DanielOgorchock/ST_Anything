#ifndef ST_ANALOGOUTPUT_H
#define ST_ANALOGOUTPUT_H

#include "Constants.h"
#include <cstdint>

namespace st
{
    typedef uint8_t analog_output_unit;
    class AnalogOutput
    {
        private:
            uint8_t m_nPin;

        public:
            AnalogOutput(uint8_t pin, analog_output_unit initial=0);

            void setPin(uint8_t pin);
            void write(analog_output_unit val);
            void operator()(analog_output_unit val);

    };
}

#endif
