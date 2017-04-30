#include "Switch.h"

namespace st
{
    //private:
    void Switch::sendCurrentStatus()
    {
        char buffer[80];
        stringprintf(buffer, (const char*)F("{\"uid\":%u,\"update\":{\"state\":%u}}"), getUid(), m_Output());
        Anything::sendUpdate(buffer);
    }


    //public:
    Switch::Switch(dev_uid_unit uid, pin_unit pin, bool invertLogic, bool initialOutput):
        Device(uid),
        m_Output(pin, invertLogic, initialOutput)
    {

    }

    Switch::~Switch()
    {

    }

    void Switch::init()
    {
        sendCurrentStatus();
    }

    // {"state":true/false}
    void Switch::beSmart(const char* msg, const jsmntok_t* t)
    {
        char buf[50];
        t+=2;
        bool b;
        if(!Anything::getJsonBool(*t, msg, &b))
            Logger::debugln(F("ERROR: Couldn't interpret Switch command string"));

        m_Output(b);

        Logger::debug(getUid());
        Logger::debug(F(": Switch turned "));
        Logger::debug(F(m_Output()?"on (output ":"off (output "));
        Logger::debugln(F(m_Output.readRaw()?"1)":"0)"));
    }

    void Switch::update()
    {

    }

    void Switch::refresh()
    {
        sendCurrentStatus();
    }

}
