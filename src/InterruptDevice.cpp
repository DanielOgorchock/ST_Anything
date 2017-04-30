#include "InterruptDevice.h"

namespace st
{
    //private
    void InterruptDevice::checkIfTriggered()
    {
        if(m_Input() && !m_bStatus) //new interrupt
        {
            m_nCurrentDownCount = 0;
            m_nCurrentUpCount++;
            if(m_nCurrentUpCount >= m_nRequiredCounts)
            {
                m_bStatus = true;
                triggered();
            }
        }
        else if(!m_Input() && m_bStatus)
        {
            m_nCurrentUpCount = 0;
            m_nCurrentDownCount++;
            if(m_nCurrentDownCount >= m_nRequiredCounts)
            {
                m_bStatus = false;
                triggerEnded();
            }
        }
    }


    //public
    InterruptDevice::InterruptDevice(dev_uid_unit uid, pin_unit pin, bool risingEdge, bool pullup, uint16_t numReqCounts):
        Device(uid),
        m_Input(pin, pullup, risingEdge),
        m_bStatus(false),
        m_nRequiredCounts(numReqCounts),
        m_nCurrentUpCount(0),
        m_nCurrentDownCount(0)
    {

    }

    InterruptDevice::~InterruptDevice()
    {

    }

    void InterruptDevice::init()
    {
        m_bStatus = m_Input();
    }

    void InterruptDevice::update()
    {
        checkIfTriggered();
    }

    void InterruptDevice::triggerEnded()
    {

    }





}





