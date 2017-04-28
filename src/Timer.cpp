#include "Timer.h"

#include <chrono>
#include <thread>

namespace st
{
    Timer::Timer()
    {

    }

    Timer::Timer(time_unit duration)
    {
        start(duration);
    }

    time_unit Timer::getMillis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
    }

    void Timer::wait(time_unit duration)
    {
       std::this_thread::sleep_for(std::chrono::duration<time_unit, std::milli>(duration));
    }

    bool Timer::ready()
    {
        time_unit now = getMillis();
        if(m_nStartTime + m_nDuration < m_nStartTime)
        {
            return (now < m_nStartTime) && (now >= m_nStartTime + m_nDuration);
        }
        else
        {
            return now >= m_nStartTime + m_nDuration;
        }
    }

    void Timer::start(time_unit duration)
    {
        m_nStartTime = getMillis();
        m_nDuration = duration;
    }

    bool Timer::operator()()
    {
        return ready();
    }
}
