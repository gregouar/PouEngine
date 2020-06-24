#include "PouEngine/system/Clock.h"

namespace pou
{

Time TimeZero()
{
    return std::chrono::duration<double>::zero();
}

Clock::Clock()
{
    this->restart();
}

Clock::~Clock()
{
    //dtor
}

Time Clock::restart()
{
    Time r = this->elapsedTime();
    m_lastTime = std::chrono::high_resolution_clock::now();
    return r;
}

Time Clock::elapsedTime()
{
    return std::chrono::high_resolution_clock::now() - m_lastTime;
}

}
