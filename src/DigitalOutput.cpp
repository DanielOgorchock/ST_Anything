#include "DigitalOutput.h"
#include "Constants.h"

#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif

namespace st
{
    //private:
    

    //public:
    DigitalOutput::DigitalOutput(uint8_t pin, bool invertLogic, bool initalOutput):
        m_bInvertLogic(invertLogic)
    {
        setPin(pin);
        write(initalOutput);
    }

    void DigitalOutput::setPin(uint8_t pin)
    {
        m_nPin = pin;

    #if defined(ST_ARDUINO)
        pinMode(pin, OUTPUT);
    #endif
    }

    void DigitalOutput::write(bool output)
    {
        bool toWrite = output;
        if(m_bInvertLogic)
            toWrite = !toWrite;

    #if defined(ST_ARDUINO)
        digitalWrite(m_nPin, toWrite);
    #endif
    }

    void DigitalOutput::operator()(bool output)
    {
        write(output);
    }

    void DigitalOutput::toggle()
    {
        write(!read());
    }

    bool DigitalOutput::read()
    {
        bool res = false;
    #if defined(ST_ARDUINO)
        res = digitalRead(m_nPin);
    #endif
        return res;
    }

    bool DigitalOutput::operator()()
    {
        return read();
    }

}






