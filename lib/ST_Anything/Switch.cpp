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

    Switch* Switch::createNew(const char* msg, const jsmntok_t* t)
    {
        int32_t uid;
        int32_t pin;
        bool invertLogic = false;
        bool initialOutput = false;

        if(t->size < 4)
        {
            Logger::debugln(F("Switch construction object doesn't contain all fields"));
            return 0;
        }

        t+=2;
        if(!Anything::getJsonInt(*t, msg, &uid))
            return 0;
        t+=2;
        if(!Anything::getJsonInt(*t, msg, &pin))
            return 0;
        t+=2;
        if(!Anything::getJsonBool(*t, msg, &invertLogic))
            return 0;
        t+=2;
        if(!Anything::getJsonBool(*t, msg, &initialOutput))
            return 0;

        return new Switch(uid, pin, invertLogic, initialOutput);
    }
}





