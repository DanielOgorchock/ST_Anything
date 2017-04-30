#ifndef ST_INTERVALDEVICE_H
#define ST_INTERVALDEVICE_H

#include "Device.h"
#include "Constants.h"
#include "Timer.h"

namespace st
{
    class IntervalDevice :public Device
    {
        private:
            Timer m_Timer; 
            time_unit m_nInterval;
            time_unit m_nOffset;

        public:
            IntervalDevice(dev_uid_unit uid, time_unit interval_ms, time_unit offset_ms=0);

            virtual ~IntervalDevice();

            virtual void init();

            virtual void beSmart(const char* msg, const jsmntok_t* t)=0;

            virtual void update();

            virtual void refresh();

            virtual void takeAction()=0;
    };
}


#endif
