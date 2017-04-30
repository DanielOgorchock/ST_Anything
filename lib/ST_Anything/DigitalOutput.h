#ifndef ST_DIGITALOUTPUT_H
#define ST_DIGITALOUTPUT_H

#include "Constants.h"
#include <stdint.h>

namespace st
{
    class DigitalOutput
    {
        private:
            pin_unit m_nPin;
            bool m_bInvertLogic;
        public:
            DigitalOutput(pin_unit pin, bool invertLogic=false, bool initialOutput=false); 

            void setPin(pin_unit pin);
            void write(bool output);
            void operator()(bool output);
            void toggle();

            bool read();
            bool operator()();
            bool readRaw(); //direct pin reading, ignore any inverted logic

    };
}

#endif
