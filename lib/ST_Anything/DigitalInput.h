#ifndef ST_DIGITALINPUT_H
#define ST_DIGITALINPUT_H

#include "Constants.h"
#include <stdint.h>

namespace st
{
    class DigitalInput
    {
        private:
            pin_unit m_nPin; 
            bool m_bInvertLogic, m_bPullup;

        public:
            DigitalInput(pin_unit pin, bool pullup=false, bool invertLogic=false);

            void setPin(pin_unit pin);
            void setPullup(bool pullup);
            void setInvertLogic(bool invertLogic);

            bool operator() ();
            bool read();
            bool isTrue();
            bool isFalse();
    };
}

#endif
