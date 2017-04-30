#include "Constants.h"

#if defined(ST_LINUX)

#include "Com_Restful.h"
#include "Logger.h"
#include <cstring>

namespace st
{
    //private
    void Com_Restful::listenThread(Com_Restful* com)
    {
        socklen_t len = sizeof(com->m_ClientAddress);
        int sock = accept(com->m_Socket, (struct sockaddr*) &com->m_ClientAddress, &len);
        if(sock < 0)
        {
            Logger::logln(F("ERROR: Accept failed"));
        }

        for(unsigned int i = 0; i < 500; ++i)
        {
            com->m_sTmpMessage[i] = 0;
        }
        int size = read(sock, com->m_sTmpMessage, 499);
        if(size < 0)
        {
            Logger::logln(F("ERROR: Failed reading from socket"));
        }

        com->m_bReceived = true; 

        //TODO send a real, valid http response, this isn't adequate
        write(sock, F("HTTP/1.1 200 OK"), 20); 

        shutdown(sock, 2);
    }


    //public
    Com_Restful::Com_Restful(int port):
       m_nPort(port),
       m_Thread(0),
       m_bReceived(false)
    {

    }

    Com_Restful::~Com_Restful()
    {

    }

    void Com_Restful::init()
    {
        m_Socket = socket(AF_INET, SOCK_STREAM, 0);
        if(m_Socket < 0)
        {
            Logger::logln(F("ERROR: Failed to get socket file descriptor")); 
        }

        m_ThisAddress.sin_family = AF_INET;
        m_ThisAddress.sin_port = htons(m_nPort);
        m_ThisAddress.sin_addr.s_addr = INADDR_ANY;

        if(bind(m_Socket, (struct sockaddr*) &m_ThisAddress, sizeof(m_ThisAddress)) < 0) 
        {
            Logger::logln(F("ERROR: Socket failed to bind"));
        }

        listen(m_Socket, 5);

        m_Thread = new std::thread(listenThread, this);
    }

    char* Com_Restful::run()
    {
        if(m_bReceived)
        {
            m_Thread->join();
            m_bReceived = false;
            stringcpy(m_sMessage, strchr(m_sTmpMessage, '{'), 499);
            m_sMessage[499] = 0;

            delete m_Thread;
            m_Thread = new std::thread(listenThread, this);

            return m_sMessage;
        }
        return 0;
    }
    
    void Com_Restful::send(const char* message, uint16_t size)
    {

    } 

}



#endif
