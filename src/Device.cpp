#include "Device.h"
#include "Constants.h"

namespace st
{
    //private:
    

    //public:
    Device::Device(dev_uid_unit uid):
        m_nUid(uid)
    {

    }

    Device::~Device()
    {

    }

    dev_uid_unit Device::getUid()
    {
        return m_nUid;
    }
}









