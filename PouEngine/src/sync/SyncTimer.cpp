#include "PouEngine/sync/SyncTimer.h"

namespace pou
{

SyncTimer::SyncTimer() :
    m_maxTime(Time(0), 0),
    m_elapsedTime(Time(0), 0),
    m_isLooping(false)
{
}

SyncTimer::~SyncTimer()
{
    //dtor
}


void SyncTimer::reset(float time, bool looping)
{
    this->reset(Time(time),looping);
}


void SyncTimer::reset(const Time &time, bool looping)
{
    m_elapsedTime.setValue(Time(0), 0, false);
    m_maxTime.setValue(time);
    m_isLooping = looping;
}

int SyncTimer::update(float elapsedTime, uint32_t localTime)
{
    return this->update(Time(elapsedTime), localTime);
}

int SyncTimer::update(const Time &elapsedTime, uint32_t localTime)
{
    m_maxTime.update(elapsedTime, localTime);
    m_elapsedTime.update(elapsedTime, localTime);

    if(elapsedTime < Time(0))
        return (0);

    if(m_maxTime.getValue() == Time(0))
        return (0);

    m_elapsedTime.setValue(m_elapsedTime.getValue() + elapsedTime);
    if(m_elapsedTime.getValue() >= m_maxTime.getValue())
    {
        if(m_isLooping)
        {
            this->reset(m_maxTime.getValue());
            return (1+this->update(elapsedTime - m_maxTime.getValue(), localTime));
        }
        this->reset(0);
        return (1);
    }
    return (0);
}

bool SyncTimer::isActive()
{
    return (m_maxTime.getValue() > Time(0));
}


/**void SyncTimer::rewind(uint32_t time)
{
    m_maxTime.rewind(time);
    m_elapsedTime.rewind(time);
}**/

}
