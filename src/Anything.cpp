#include "Anything.h"


namespace st
{
    //static initializations
    uint8_t Anything::numDevices = 0;
    uint8_t Anything::updatesIndex = 0;
    Timer Anything::refreshTimer();
    Timer Anything::sendUpdatesTimer();
    Communicator* Anything::communicator = 0;

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
        for(uint8_t i = 0; i < numdevices; ++i)
        {
            devices[i]->init();
        }
    }

    void Anything::updateDevices()
    {
        for(uint8_t i = 0; i < numdevices; ++i)
        {
            devices[i]->update();
        }
    }

    void Anything::sendUpdates()
    {
        communicator->send(updatesBuffer, updatesIndex);
    }

    void Anything::handleMessage(const char* msg)
    {
    }

    //public:
    void Anything::init()
    {
        refreshTimer.start(REFRESH_INTERVAL);
        sendUpdatesTimer.start(SEND_UPDATES_INTERVAL);

        //TODO: assign communicator


        updatesBuffer[0] = 0;

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
         
         
    }

}





































