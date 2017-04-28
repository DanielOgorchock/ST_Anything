#include "Logger.h"

#include "Constants.h"

#if defined(ST_LINUX)
    #include <iostream>
    #include <fstream>
#endif

#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif

namespace st
{
    void Logger::log(const char* msg)
    {
    #if defined(ST_LINUX)
        std::cout << msg;
    #endif

    #if defined(ST_ARDUINO)
        Serial.print(msg);
    #endif    
    }

    void Logger::logln(const char* msg)
    {
        log(msg);
        log("\n");
    }
}
