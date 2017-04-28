#ifndef ST_TIMER_H
#define ST_TIMER_H

#include "Constants.h"

#include <cstdint>

namespace st
{
#if defined(ST_LINUX)
    typedef uint64_t time_unit;
#elif defined(ST_ARDUINO)
    typedef uint32_t time_unit;
#endif
    class Timer
    {
        private:
            time_unit m_nStartTime, m_nDuration;

        public:
            Timer();
            Timer(time_unit duration);

            static time_unit getMillis();
            static void wait(time_unit duration);
            bool ready();
            void start(time_unit duration);
            bool operator()();
    };    
}



#endif
