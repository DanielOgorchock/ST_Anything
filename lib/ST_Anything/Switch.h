#ifndef ST_SWITCH_H
#define ST_SWITCH_H

#include "Anything.h"
#include "Constants.h"
#include "Device.h"
#include "DigitalOutput.h"

namespace st
{
    class Switch :public Device
    {
        private:
            DigitalOutput m_Output;
            
            void sendCurrentStatus();

        public:

            Switch(dev_uid_unit uid, pin_unit pin, bool invertLogic=false, bool initialOutput=false);
            
            virtual ~Switch();

            virtual void init();

            virtual void beSmart(const char* msg, const jsmntok_t* t);

            virtual void update();

            virtual void refresh();


            static Switch* createNew(const char* msg, const jsmntok_t* t);
    };
}



#endif
