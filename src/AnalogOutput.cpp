#include "AnalogOutput.h"
#include "Constants.h"

#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif

namespace st
{
    //private


    //public
    AnalogOutput::AnalogOutput(uint8_t pin, analog_output_unit initial)
    {
        setPin(pin);
        write(initial);
    }

    void AnalogOutput::setPin(uint8_t pin)
    {
        m_nPin = pin;
    }

    void AnalogOutput::write(analog_output_unit val)
    {
    #if defined(ST_ARDUINO)
        analogWrite(m_nPin, val);
    #endif
    }

    void AnalogOutput::operator()(analog_output_unit val)
    {
        write(val);
    }
}
