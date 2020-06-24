#include "PouEngine/system/Timer.h"

namespace pou
{


Timer::Timer()
{
    m_maxTime       = Time(0);
    m_elapsedTime   = Time(0);
    m_isLooping     = false;
}

Timer::~Timer()
{
    //dtor
}


void Timer::reset(float time, bool looping)
{
    this->reset(Time(time),looping);
}


void Timer::reset(const Time &time, bool looping)
{
    m_elapsedTime   = Time(0);
    m_maxTime       = time;
    m_isLooping     = looping;
}

int Timer::update(float elapsedTime)
{
    return this->update(Time(elapsedTime));
}

int Timer::update(const Time &elapsedTime)
{
    if(elapsedTime < Time(0))
        return (0);

    if(m_maxTime == Time(0))
        return (0);

    m_elapsedTime += elapsedTime;
    if(m_elapsedTime >= m_maxTime)
    {
        if(m_isLooping)
        {
            this->reset(m_maxTime);
            return (1+this->update(elapsedTime - m_maxTime));
        }
        this->reset(0);
        return (1);
    }
    return (0);
}

Time Timer::remainingTime()
{
    return m_maxTime-m_elapsedTime;
}

bool Timer::isActive()
{
    return (m_maxTime > Time(0));
}


}
