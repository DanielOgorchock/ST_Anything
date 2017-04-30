// ST_Anything 3.0
#include "Anything.h"
#include "Com_Console.h"
#include "Com_Restful.h"

using namespace st;

int main(int argc, char* argv[])
{
    Anything::init(new Com_Restful(44555));

    while(true)
    {
        Anything::run();
    }

    return 0;
}
