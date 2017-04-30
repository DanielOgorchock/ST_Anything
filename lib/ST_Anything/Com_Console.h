#ifndef ST_COM_CONSOLE_H
#define ST_COM_CONSOLE_H

#include "Constants.h"

#if defined(ST_LINUX)

#include "Communicator.h"
#include <thread>


namespace st
{
    class Com_Console :public Communicator
    {
        private:
            static void inputThread(Com_Console* console);
            std::string tmpMsg;
            char m_sMessage[200];
            std::thread* m_Thread;
            bool m_bReceived;


        public:
            Com_Console();
            
            virtual ~Com_Console();

            virtual void init();
            
            virtual char* run();

            virtual void send(const char* message, uint16_t size);


    };
}



#endif
#endif
