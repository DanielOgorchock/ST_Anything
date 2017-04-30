#ifndef ST_DIGITALINPUT_H
#define ST_DIGITALINPUT_H

#include "Constants.h"
#include <cstdint>

namespace st
{
    class DigitalInput
    {
        private:
            uint8_t m_nPin; 
            bool m_bInvertLogic, m_bPullup;

        public:
            DigitalInput(uint8_t pin, bool pullup=false, bool invertLogic=false);

            void setPin(uint8_t pin);
            void setPullup(bool pullup);
            void setInvertLogic(bool invertLogic);

            bool operator() ();
            bool read();
            bool isTrue();
            bool isFalse();
    };
}

#endif
