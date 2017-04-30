#include "Constants.h"

#if defined(ST_LINUX)

#include "Com_Console.h"
#include <iostream>
#include <string>
namespace st
{
    //private
    void Com_Console::inputThread(Com_Console* console)
    {
        std::getline(std::cin, console->tmpMsg);
        console->m_bReceived = true;
    } 



    //public
    Com_Console::Com_Console():
        m_Thread(0),
        m_bReceived(false)
    {

    }

    Com_Console::~Com_Console()
    {
        delete m_Thread;
    }

    void Com_Console::init()
    {
        m_Thread = new std::thread(inputThread, this);
    }

    char* Com_Console::run()
    {
        if(m_bReceived)
        {
            m_Thread->join();
            m_bReceived = false;
            delete m_Thread;
            tmpMsg.copy(m_sMessage, 199, 0);
            m_sMessage[tmpMsg.length()] = 0;
            m_Thread = new std::thread(inputThread, this);
            return m_sMessage;
        }
        else
            return 0;
    }

    void Com_Console::send(const char* message, uint16_t size)
    {
        std::cout << message << std::endl;
    }
}

#endif
