#ifndef ST_ANYTHING_H
#define ST_ANYTHING_H

#include "Constants.h"
#include "Device.h"
#include "Communicator.h"
#include "Timer.h"
#include "Logger.h"
#include "jsmn.h"
#include <stdint.h>
#include <stdio.h>

#if defined(ST_LINUX)
    #include <cstring>
#elif defined(ST_ARDUINO)
    #include <string.h>
#endif

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

            static jsmn_parser jParser; //for json parsing

            static void refreshDevices(); 
            static void initDevices();
            static void updateDevices();

            static void sendUpdates();
            static void handleMessage(const char* msg);

        public:
            static void init(Communicator* com);
            static void run();

            static void sendUpdate(const char* msg);
            static void sendUpdateNow(const char* msg);

            static Device* getDevice(dev_uid_unit uid);

            static bool addDevice(Device* dev);


            static uint8_t timersPending;


            //json parsing helpers
            static bool getJsonString(const jsmntok_t& t, const char* msg, char* buffer, uint8_t size);
            static bool getJsonBool(const jsmntok_t& t, const char* msg, bool* res);
            static bool getJsonInt(const jsmntok_t& t, const char* msg, int32_t* res);
    };
}



#endif
