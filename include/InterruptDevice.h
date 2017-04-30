#ifndef ST_INTERRUPTDEVICE_H
#define ST_INTERRUPTDEVICE_H

#include "Constants.h"
#include "DigitalInput.h"
#include "Device.h"

#include <stdint.h>

namespace st
{
    class InterruptDevice :public Device
    {
        private:
            DigitalInput m_Input;
            bool m_bRisingEdge;
            bool m_bStatus;
            uint16_t m_nRequiredCounts;
            uint16_t m_nCurrentUpCount;
            uint16_t m_nCurrentDownCount;

            void checkIfTriggered();

        public:
            InterruptDevice(dev_uid_unit uid, pin_unit pin, bool risingEdge = true, bool pullup = false, uint16_t numReqCounts = 0);

            virtual ~InterruptDevice();
            
            virtual void init();

            virtual void beSmart(const char* msg, const jsmntok_t* t)=0;

            virtual void update();

            virtual void refresh()=0;

            virtual void triggered()=0;

            virtual void triggerEnded(); //optionally, can take action on the reverse edge


    };
}


#endif
