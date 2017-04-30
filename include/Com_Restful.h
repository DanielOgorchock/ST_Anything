#ifndef ST_COM_RESTFUL_H
#define ST_COM_RESTFUL_H
#include "Constants.h"

#if defined(ST_LINUX)
#include "Communicator.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

namespace st
{
    class Com_Restful :public Communicator
    {
        private:
            int m_Socket;
            int m_nPort;
            struct sockaddr_in m_ThisAddress;
            struct sockaddr_in m_ClientAddress;
            struct sockaddr_in m_RemoteAddress;
            char m_sTmpMessage[500];
            char m_sMessage[500];
            std::thread *m_Thread;
            bool m_bReceived; 
            

            static void listenThread(Com_Restful* com);

        public:
            Com_Restful(int port);
            
            virtual ~Com_Restful();

            virtual void init();
            
            virtual char* run();

            virtual void send(const char* message, uint16_t size);
    };
}

#endif
#endif
