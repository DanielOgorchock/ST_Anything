// ST_Anything 3.0
#include "Timer.h"
#include "Logger.h"
#include <chrono>
#include <iostream>
#include <sstream>

using namespace st;

int main(int argc, char* argv[])
{
    time_unit start = Timer::getMillis();

    Timer timer;

    std::stringstream ss;

    if(argc > 1)
        ss<<argv[1];
    else
        ss<<1000;

    time_unit dur;
    ss>>dur;

    std::stringstream ss2;

    if(argc > 2)
        ss2<<argv[2];
    else
        ss2<<1;

    time_unit sleepDur;
    ss2>>sleepDur;

    timer.start(dur);

    std::cout<<sleepDur<<std::endl;
    while(!timer())
    {
        Timer::wait(sleepDur);
    }
    std::cout<<(Timer::getMillis()-start)<<std::endl;

    Logger::logln("Testing logging class 123.");
    return 0;
}
