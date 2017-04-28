#include "AnalogInput.h"
#include "Constants.h"

#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif

namespace st
{
    //private
    

    //public
    AnalogInput::AnalogInput(uint8_t pin)
    {
        setPin(pin);
    }

    void AnalogInput::setPin(uint8_t pin)
    {
        m_nPin = pin;
    }

    analog_input_unit AnalogInput::read()
    {
        analog_input_unit res = 0;
    #if defined(ST_ARDUINO)
        res = analogRead(m_nPin);
    #endif
        return res;
    }

    analog_input_unit AnalogInput::operator()()
    {
        return read();
    }
}
