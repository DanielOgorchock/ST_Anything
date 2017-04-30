#ifndef ST_LOGGER_H
#define ST_LOGGER_H

#include "Constants.h"
#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif

namespace st
{
    class Logger
    {
        private:

        public:
           static void log(const char* msg); 
           static void logln(const char* msg);
           static void debug(const char* msg);
           static void debugln(const char* msg);
        #if defined(ST_ARDUINO)
           static void log(const __FlashStringHelper* msg); 
           static void logln(const __FlashStringHelper* msg);
           static void debug(const __FlashStringHelper* msg);
           static void debugln(const __FlashStringHelper* msg);
        #endif
    };
}


#endif
