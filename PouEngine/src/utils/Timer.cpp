#include "PouEngine/utils/Timer.h"

namespace pou
{


Timer::Timer()
{
    m_maxTime       = 0.0f;
    m_elapsedTime   = 0.0f;
    m_isLooping     = false;
}

Timer::~Timer()
{
    //dtor
}


void Timer::reset(float time, bool looping)
{
    m_elapsedTime   = 0;
    m_maxTime       = time;
    m_isLooping     = looping;
}

int Timer::update(const pou::Time &elapsedTime)
{
    return (this->update(elapsedTime.count()));
}

int Timer::update(float elapsedTime)
{
    if(elapsedTime < 0)
        return (0);

    if(m_maxTime == 0)
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

bool Timer::isActive()
{
    return (m_maxTime > 0);
}

}
