#include "Logger.h"

#include "Constants.h"

#if defined(ST_LINUX)
    #include <iostream>
    #include <fstream>
    #include <cstring>
    #include <stdio.h>
#endif

#if defined(ST_ARDUINO)
    #include <Arduino.h>
    #include <string.h>
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

    void Logger::log(long long n)
    {
        char buf[15];
        sprintf(buf, "%d", n);
        log(buf); 
    }

    void Logger::logln(const char* msg)
    {
        log(msg);
        log("\n");
    }

    void Logger::debug(const char* msg)
    {
    #if defined(ST_DEBUG)
        log(msg);
    #endif
    }

    void Logger::debug(long long n)
    {
    #if defined(ST_DEBUG)
        log(n);
    #endif
    }

    void Logger::debugln(const char* msg)
    {
    #if defined(ST_DEBUG)
        logln(msg);
    #endif
    }

#if defined(ST_ARDUINO)
    void Logger::log(const __FlashStringHelper* msg)
    {
        Serial.print(msg);
    }

    void Logger::logln(const __FlashStringHelper* msg)
    {
        log(msg);
        log("\n");
    }

    void Logger::debug(const __FlashStringHelper* msg)
    {
    #if defined(ST_DEBUG)
        log(msg);
    #endif
    }

    void Logger::debugln(const __FlashStringHelper* msg)
    {
    #if defined(ST_DEBUG)
        logln(msg);
    #endif
    }
#endif
}
