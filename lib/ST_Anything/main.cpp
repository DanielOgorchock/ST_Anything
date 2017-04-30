// ST_Anything 3.0
#include "Constants.h"

#if defined(ST_LINUX)

#include "Anything.h"
#include "Com_Console.h"
#include "Com_Restful.h"
#include "Switch.h"
#include "Timer.h"

using namespace st;

int main(int argc, char* argv[])
{
    //Anything::addDevice(new Switch(1, 1));
    //Anything::init(new Com_Restful(44555));
    Anything::init(new Com_Console());
    
    Timer t(1);

    while(true)
    {
        if(t())
        {
            t.start(1);
            Anything::run();
        }
    }

    return 0;
}

#endif
