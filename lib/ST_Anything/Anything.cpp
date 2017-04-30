#include "Anything.h"

#if defined(ST_ARDUINO)
    #include <Arduino.h>
#endif


//include all devices:
#include "Switch.h"

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
        if(stringcmp(updatesBuffer,(const char*) F("{\"type\":\"update\", \"updates\":[")))
        {
            updatesBuffer[updatesIndex-1] = ']';
            updatesBuffer[updatesIndex] = '}';
            communicator->send(updatesBuffer, updatesIndex);
            stringcpy(updatesBuffer, (const char*)F("{\"type\":\"update\", \"updates\":["), UPDATES_BUFFER_SIZE);
            updatesIndex = strlen(updatesBuffer);
            updatesBuffer[updatesIndex] = 0;
        }
    }

    void Anything::handleMessage(const char* msg)
    {
        jsmn_init(&jParser);

        Logger::debug(F("Received message: "));
        Logger::debugln(msg);

        jsmntok_t tokens[JSMN_TOKENS];
        int res = jsmn_parse(&jParser, msg, strlen(msg), tokens, JSMN_TOKENS);

        char tmp[20];
        jsmntok_t* t = &tokens[0];
        
        // {"type":"str", ...}"
        if(res > 0  &&  t->type == JSMN_OBJECT)
        {
            ++t;
            if (t->type == JSMN_STRING)
            {
                if(!getJsonString(*t, msg, tmp, 20))
                    return;
                if(!stringcmp(tmp,(const char*) F("type")))
                {
                    ++t;
                    if(t->type == JSMN_STRING)
                    {
                        if(!getJsonString(*t, msg, tmp, 20))
                            return;
                        Logger::debug(F("type: "));
                        Logger::debugln(tmp);

                        //handle message types:
                        // {"type":"refresh"}
                        if(!stringcmp(tmp,(const char*) F("refresh")))
                        {
                            refreshDevices();
                        }
                        // {"type":"add" , "cmd":{"dtype":"str", "parms":{construction params}}}
                        else if(!stringcmp(tmp,(const char*) F("add")))
                        {
                            ++t;
                            if(t->type == JSMN_STRING && (++t)->type == JSMN_OBJECT)
                            {
                                createDevice(msg, t);
                            }

                        }
                        // {"type":"del", "uid":num}
                        else if(!stringcmp(tmp,(const char*) F("del")))
                        {
                            ++t;
                            if(t->type == JSMN_STRING)
                            {
                                if(!getJsonString(*t, msg, tmp, 20))
                                    return;
                                if((++t)->type == JSMN_PRIMITIVE && !stringcmp(tmp, (const char*) F("uid")))
                                {
                                    int32_t id;
                                    if(!getJsonInt(*t, msg, &id))
                                       return;

                                    Device* d = getDevice(id);
                                    if(!d)
                                    {
                                        Logger::debugln(F("ERROR: UID not found"));
                                        return;
                                    }

                                    Device* tmp[MAX_DEVICES];
                                    bool found = false;
                                    for(uint8_t i = 0; i < numDevices; ++i)
                                    {
                                        if(devices[i] == d)
                                        {
                                            devices[i] = 0;
                                            found = true;
                                            continue;
                                        }

                                        tmp[found?(i-1):i] = devices[i];
                                        devices[i] = 0;
                                    }

                                    --numDevices;
                                    
                                    for(uint8_t i = 0; i < numDevices; ++i)
                                    {
                                        devices[i] = tmp[i];
                                    }

                                    delete d;
                                }
                            }                            
                        }
                        // {"type":"update" , {"uid":num, "parms":{object for the Dev.}}}
                        else if(!stringcmp(tmp,(const char*) F("update")))
                        {
                            ++t;
                            if(t->type == JSMN_OBJECT && (++t)->type == JSMN_STRING)
                            {
                                if(!getJsonString(*t, msg, tmp, 20))
                                    return;
                                if(!stringcmp(tmp, (const char*)F("uid")) && (++t)->type == JSMN_PRIMITIVE)
                                {
                                    int32_t uid = 0;
                                    if(!getJsonInt(*t, msg, &uid))
                                        return;

                                    Device* d = getDevice(uid);
                                    if(!d)
                                    {
                                        Logger::debugln(F("ERROR: UID not found"));
                                        return;
                                    }

                                    if((++t)->type == JSMN_STRING && (++t)->type == JSMN_OBJECT)
                                    {
                                        d->beSmart(msg, t);
                                    }
                                     
                                }
                            }         
                        }
                        else
                        {
                            Logger::debugln(F("Unknown message type, ignoring..."));
                        }
                    }
                }
            }

        }
    }

    void Anything::createDevice(const char* msg, const jsmntok_t* t)
    {
        char tmp[20];
        if((++t)->type != JSMN_STRING || !getJsonString(*t, msg, tmp, 20))
            return;
        if(!stringcmp(tmp, (const char*) F("dtype")) && (++t)->type == JSMN_STRING)
        {
            if(!getJsonString(*t, msg, tmp, 20))
                return;
            ++t;
            if(t->type != JSMN_STRING || (++t)->type != JSMN_OBJECT)
                return;

            Device* d;
            //check which device it is...
            if(!stringcmp(tmp, (const char*) F("switch")))
                d = Switch::createNew(msg, t); 
            else
            {
                Logger::debugln(F("ERROR: unknown type to create"));
            }


            if(!d)
            {
                Logger::debugln(F("ERROR: device creation failed"));
                return;
            }

            //make sure this is a unique id
            for(uint8_t i = 0; i < numDevices; ++i)
            {
                if(devices[i]->getUid() == d->getUid())
                {
                    Logger::debugln(F("ERROR: Another device already has this UID"));
                    delete d;
                    return;
                }
            }

            if(!addDevice(d))
            {
                Logger::debugln(F("ERROR: Maximum device count reached"));
                delete d;
                return;
            }
            d->init();
        }
    }

    //public:
    void Anything::init(Communicator* com)
    {
    #if defined(ST_ARDUINO)
        Serial.begin(SERIAL_BAUDRATE);
    #endif

        refreshTimer.start(REFRESH_INTERVAL);
        sendUpdatesTimer.start(SEND_UPDATES_INTERVAL);

        jsmn_init(&jParser);

        //Communicators:
        communicator = com;

        stringcpy(updatesBuffer, (const char*)F("{\"type\":\"update\", \"updates\":["), UPDATES_BUFFER_SIZE);
        updatesIndex = strlen(updatesBuffer);
        updatesBuffer[updatesIndex] = 0;

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
        if(len + updatesIndex + 10 >= UPDATES_BUFFER_SIZE)
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
        strncpy(buffer, &msg[t.start], len-1);
        buffer[len-1] = 0;

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

        strncpy(tmp, &msg[t.start], len-1);
        tmp[len-1] = 0;
        return sscanf(tmp, "%i", res) == 1;
    }
}















