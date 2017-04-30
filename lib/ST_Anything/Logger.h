#ifndef ST_LOGGER_H
#define ST_LOGGER_H

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
    };
}


#endif
