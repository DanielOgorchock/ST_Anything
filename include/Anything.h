#ifndef ST_ANYTHING_H
#define ST_ANYTHING_H

#include "Constants.h"
#include "Device.h"
#include "Communicator.h"
#include "Timer.h"
#include <stdint.h>
#include <string.h>

namespace st
{
    class Anything
    {
        private:
            static uint8_t numDevices;
            static Device* devices[MAX_DEVICES]; 
            static uint8_t updatesIndex;
            static char updatesBuffer[UPDATES_BUFFER_SIZE];
            static Timer refreshTimer;
            static Timer sendUpdatesTimer;
            static Communicator* communicator;


            static void refreshDevices(); 
            static void initDevices();
            static void updateDevices();

            static void sendUpdates();
            static void handleMessage(const char* msg);

        public:
            static void init();
            static void run();

            static void sendUpdate(const char* msg);
            static void sendUpdateNow(const char* msg);

            static Device* getDevice(dev_uid_unit uid);

            static bool addDevice(Device* dev);


            static uint8_t timersPending;

    };
}



#endif
