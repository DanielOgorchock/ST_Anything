#ifndef ST_DEVICE_H
#define ST_DEVICE_H

#include "Constants.h"
#include <stdint.h>

namespace st
{
    typedef uint8_t dev_uid_unit;
    class Device
    {
        private:
            dev_uid_unit m_nUid; 

        public:
            Device(dev_uid_unit uid);
            
            virtual ~Device();

            virtual void init()=0;

            virtual void beSmart(const char* msg)=0;

            virtual void update()=0;

            virtual void refresh()=0;

            dev_uid_unit getUid();
    };
}


#endif
