#include "IntervalDevice.h"


namespace st
{
    //private
    

    //public
    IntervalDevice::IntervalDevice(dev_uid_unit uid, time_unit interval_ms, time_unit offset_ms):
        Device(uid),
        m_Timer(),
        m_nInterval(interval_ms),
        m_nOffset(offset_ms)
    {

    }

    IntervalDevice::~IntervalDevice()
    {

    }

    void IntervalDevice::init()
    {
        m_Timer.start(m_nInterval + m_nOffset); //offset only needs to be applied the first time the timer is run.
    }
        
    void IntervalDevice::update()
    {
        if(m_Timer())
        {
            m_Timer.start(m_nInterval);
            takeAction();
        }
    }
   
}
