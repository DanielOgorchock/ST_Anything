#ifndef ST_DIGITALOUTPUT_H
#define ST_DIGITALOUTPUT_H

#include <cstdint>

namespace st
{
    class DigitalOutput
    {
        private:
            uint8_t m_nPin;
            bool m_bInvertLogic;
        public:
            DigitalOutput(uint8_t pin, bool invertLogic=false, bool initialOutput=false); 

            void setPin(uint8_t pin);
            void write(bool output);
            void operator()(bool output);
            void toggle();

            bool read();
            bool operator()();

    };
}

#endif
