#include "Anything.h"


namespace st
{
    //static initializations
    uint8_t Anything::numDevices = 0;
    Device* Anything::devices[];
    uint8_t Anything::updatesIndex = 0;
    char Anything::updatesBuffer[];
    Timer Anything::refreshTimer;
    Timer Anything::sendUpdatesTimer;
    Communicator* Anything::communicator = 0;
    jsmn_parser Anything::jParser;
    uint8_t Anything::timersPending;

    //private:
    void Anything::refreshDevices()
    {
        for(uint8_t i = 0; i < numDevices; ++i)
        {
            devices[i]->refresh();
        }
    }

    void Anything::initDevices()
    {
        for(uint8_t i = 0; i < numDevices; ++i)
        {
            devices[i]->init();
        }
    }

    void Anything::updateDevices()
    {
        for(uint8_t i = 0; i < numDevices; ++i)
        {
            devices[i]->update();
        }
    }

    void Anything::sendUpdates()
    {
        updatesBuffer[updatesIndex] = ']';
        communicator->send(updatesBuffer, updatesIndex);
        updatesIndex = 1;
        updatesBuffer[0] = '[';
        updatesBuffer[1] = 0;
    }

    void Anything::handleMessage(const char* msg)
    {
        Logger::debug(F("Received message: "));
        Logger::debugln(msg);

        jsmntok_t tokens[JSMN_TOKENS];
        int res = jsmn_parse(&jParser, msg, strlen(msg), tokens, JSMN_TOKENS);

        char tmp[20];
        jsmntok_t* t = &tokens[0];

        if(res > 0  &&  t->type == JSMN_OBJECT)
        {
            ++t;
            if (t->type == JSMN_STRING)
            {
                getJsonString(*t, msg, tmp, 20);
                if(!strcmp(tmp, F("type")))
                {
                    ++t;
                    if(t->type == JSMN_STRING)
                    {
                        getJsonString(*t, msg, tmp, 20);
                        Logger::debug(F("type: "));
                        Logger::debugln(tmp);

                        //later add checking for different message types
                    }
                }
            }

        }
        
    }

    //public:
    void Anything::init()
    {
        refreshTimer.start(REFRESH_INTERVAL);
        sendUpdatesTimer.start(SEND_UPDATES_INTERVAL);

        jsmn_init(&jParser);

        //TODO: assign communicator


        updatesBuffer[0] = '[';
        updatesBuffer[1] = 0;
        updatesIndex = 1;

        communicator->init();

        initDevices();
    }    

    void Anything::run()
    {
        updateDevices();
        
        sendUpdates();

        char* msg = communicator->run();
        if(msg)
        {
            handleMessage(msg);    
        }

        if(refreshTimer())
        {
            refreshTimer.start(REFRESH_INTERVAL);
            refreshDevices();
        }
         
        sendUpdates();  //sending before and after potentially receiving a message from ST seems like a good idea

    }

    void Anything::sendUpdate(const char* msg)
    {
        int len = strlen(msg);
        if(len + updatesIndex + 1 >= UPDATES_BUFFER_SIZE)
        {
            sendUpdates();
        } 

        strncpy(&updatesBuffer[updatesIndex], msg, UPDATES_BUFFER_SIZE - updatesIndex - 1);
        updatesIndex += len;
        updatesBuffer[updatesIndex] = ',';
        updatesIndex++;
        updatesBuffer[updatesIndex] = 0;
    }

    void Anything::sendUpdateNow(const char* msg)
    {
        sendUpdate(msg);
        sendUpdates();
    }

    Device* Anything::getDevice(dev_uid_unit uid)
    {
        for(uint8_t i = 0; i < numDevices; ++i)
        {
            if(devices[i]->getUid() == uid)
            {
                return devices[i];
            }
        }

        return 0;
    }

    bool Anything::addDevice(Device* dev)
    {
        if(numDevices < MAX_DEVICES)
        {
            devices[numDevices++] = dev;
            return true;
        }

        return false;
    } 



    //json helpers
    bool Anything::getJsonString(const jsmntok_t& t, const char* msg, char* buffer, uint8_t size)
    {
        int len = t.end - t.start + 1;
        if(len+1 > size)
        {
            return false;
        }
        strncpy(buffer, &msg[t.start], len);

        return true;
    }

    bool Anything::getJsonBool(const jsmntok_t& t, const char* msg, bool* res)
    {
        char c = msg[t.start];

        if(c == 't' || c == 'T')
        {
            *res = true;
            return true;
        }
        if(c == 'f' || c == 'F')
        {
            *res = false;
            return true;
        }

        return false;
    }

    bool Anything::getJsonInt(const jsmntok_t& t, const char* msg, int32_t* res)
    {
        char tmp[12];
        int len = t.end - t.start + 1;
        if(len+1 > 12)
            return false;

        strncpy(tmp, &msg[t.start], len);
        return sscanf(tmp, F("%i"), res) == 1;
    }
}















