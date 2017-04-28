#ifndef ST_ANALOGINPUT_H
#define ST_ANALOGINPUT_H

#include <cstdint>

namespace st
{
    typedef uint16_t analog_output_unit;
    class AnalogInput
    {
        private:
        uint8_t m_nPin;

        public:
        AnalogInput(uint8_t pin);

        void setPin(uint8_t pin);

        analog_output_unit read();
        analog_output_unit operator()();
        
    };
}

#endif
