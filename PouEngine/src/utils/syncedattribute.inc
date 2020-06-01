#include "PouEngine/utils/SyncedAttribute.h"

#include "PouEngine/net/NetEngine.h"



bool uint32less(uint32_t lhs, uint32_t rhs);
bool uint32leq(uint32_t lhs, uint32_t rhs);

#include <iostream>

namespace pou
{

template<typename T>
SyncedAttribute<T>::SyncedAttribute() :
    m_value(T()),
    m_firstSync(true),
    m_curLocalTime(0),
    m_lastUpdateTime(-1),
    m_syncTime(-1),
    m_lastSyncTime(-1),
    m_maxRewindAmount(NetEngine::getMaxRewindAmount()),
    m_syncDelay(0),
    m_syncPrecision(T()),
    m_timeBeforeRewind(-1)
{
}

template<typename T>
SyncedAttribute<T>::SyncedAttribute(const T &t, uint32_t localTime) :
    SyncedAttribute()
{
    this->setValue(t, localTime);
}

template<typename T>
SyncedAttribute<T>::~SyncedAttribute()
{

}

template<typename T>
void SyncedAttribute<T>::syncFrom(const SyncedAttribute<T> &t)
{
    if(uint32less(m_syncTime, t.m_lastUpdateTime))
    {
        m_syncValues.insert_or_assign(t.m_lastUpdateTime, t.getValue());
        m_syncTime  = t.m_lastUpdateTime;
    }
}

template<typename T>
bool SyncedAttribute<T>::update(const Time &elapsed_time, uint32_t curTime)
{
    if(curTime != (uint32_t)(-1))
        m_curLocalTime = curTime;

   // if(m_timeBeforeRewind != (uint32_t)(-1) && m_curLocalTime > m_timeBeforeRewind)
    //if(m_curLocalTime != (uint32_t)(-1) && m_curLocalTime > m_timeBeforeRewind)
    if(m_timeBeforeRewind != (uint32_t)(-1) && uint32less(m_timeBeforeRewind, m_curLocalTime))
    {
        if((m_lastUpdateTime < m_curLocalTime || m_lastUpdateTime == (uint32_t)(-1))
        && !(m_value == m_valueBeforeRewind))
            m_lastUpdateTime = m_curLocalTime;
        m_timeBeforeRewind = (uint32_t)(-1);
    }

    if(m_syncTime == (uint32_t)(-1) || m_syncValues.empty())
        return (false);

    auto itSyncValue = m_syncValues.lower_bound(m_curLocalTime);

    /*if(itSyncValue->first != this->m_curLocalTime)
    {
        if(itSyncValue == this->m_syncValues.begin())
            return (false);
        else
            (--itSyncValue);
    }*/


    if(itSyncValue != m_syncValues.begin()) //Don't need the condition though
    {
        m_syncValues.erase(m_syncValues.begin(), itSyncValue);
        itSyncValue = m_syncValues.begin();
    }

    if(m_firstSync)
    {
        m_firstSync = false;
        m_lastSyncTime = itSyncValue->first;
        this->setValue(itSyncValue->second);
        return (true);
    }

    //if(this->m_syncDelay == 0)
    {
        if(uint32less(m_lastSyncTime, itSyncValue->first))
        {
            this->setValue(itSyncValue->second);
            m_lastSyncTime = itSyncValue->first;
            return (true);
        }
    }
    /*else
    {
        if(this->m_value == itSyncValue->second)
        {
            this->m_syncTimer.reset(this->m_syncDelay);
        }
        else if(this->m_syncTimer.update(elapsed_time))
        {
            this->setValue(itSyncValue->second);
            m_lastSyncTime = m_syncTime;
            return (true);
        }
        else if(!this->m_syncTimer.isActive())
            this->m_syncTimer.reset(this->m_syncDelay);
     }*/


    return (false);
}

template<typename T>
void SyncedAttribute<T>::setValue(const T &t, bool forceUpdate)
{
    this->setValue(t, m_curLocalTime, forceUpdate);
}

template<typename T>
void SyncedAttribute<T>::setValue(const T &t, uint32_t curLocalTime, bool forceUpdate)
{
    if(m_value == t && m_lastUpdateTime != (uint32_t)(-1) && !forceUpdate)
        return;

    m_value = t;
    if(uint32less(m_lastUpdateTime, curLocalTime))
    //if(curLocalTime > m_lastUpdateTime || m_lastUpdateTime == (uint32_t)(-1))
        m_lastUpdateTime = curLocalTime;

    if(m_maxRewindAmount > 0 && curLocalTime  != (uint32_t)(-1))
    {
        m_rewindValues.insert_or_assign(curLocalTime, t);
        if(m_rewindValues.size() >= m_maxRewindAmount)
            m_rewindValues.erase(m_rewindValues.begin());
    }
}

template<typename T>
bool SyncedAttribute<T>::rewind(uint32_t time)
{
    if(m_rewindValues.empty())
        return (false);

    bool r = true;
    auto it = m_rewindValues.upper_bound(time);

    if(it != m_rewindValues.begin())
        --it;

    m_timeBeforeRewind = m_curLocalTime;
    m_valueBeforeRewind = m_value;

    m_curLocalTime = it->first;
    m_lastUpdateTime = it->first;
    this->setValue(it->second);
    m_rewindValues.erase(++it, m_rewindValues.end());

    return (r);
}

template<typename T>
void SyncedAttribute<T>::setSyncDelay(uint32_t syncDelay)
{
    m_syncDelay = syncDelay;
}

template<typename T>
void SyncedAttribute<T>::setSyncPrecision(T precision)
{
    m_syncPrecision = precision;
}

template<typename T>
void SyncedAttribute<T>::setMaxRewindAmount(size_t maxRewind)
{
    m_maxRewindAmount = maxRewind;
}

template<typename T>
const T &SyncedAttribute<T>::getValue(bool useRewind) const
{
    if(!useRewind || m_rewindValues.empty() || m_syncDelay == 0)
        return m_value;

    auto it = m_rewindValues.begin();
    if((int64_t)(m_curLocalTime - m_syncDelay) >= 0)
        it = m_rewindValues.upper_bound(m_curLocalTime - m_syncDelay);

    if(it != m_rewindValues.begin())
        --it;

    return it->second;
}

template<typename T>
uint32_t SyncedAttribute<T>::getLastUpdateTime(bool useRewind) const
{
    if(useRewind)
        return (m_lastUpdateTime + m_syncDelay);
    return m_lastUpdateTime;
}

template<typename T>
uint32_t SyncedAttribute<T>::getSyncTime() const
{
    return m_syncTime;
}

///LinSyncedAttribute

template<typename T>
LinSyncedAttribute<T>::LinSyncedAttribute() :
    SyncedAttribute<T>(),
    m_useModulo(false),
    m_interpolationDelay(0)
{

}

template<typename T>
LinSyncedAttribute<T>::LinSyncedAttribute(const T &t, uint32_t localTime) :
    SyncedAttribute<T>(t, localTime),
    m_useModulo(false),
    m_interpolationDelay(0)
{
}

/*template<typename T>
void LinSyncedAttribute<T>::syncFrom(const LinSyncedAttribute<T> &t)
{
    //if(t.m_lastUpdateTime > this->m_syncTime)
    if(uint32less(this->m_syncTime,t.m_lastUpdateTime))
    {
        this->m_syncValues.insert_or_assign(t.m_lastUpdateTime, t.getValue());
        this->m_syncTime    = t.m_lastUpdateTime;
    }
}*/


template<typename T>
bool LinSyncedAttribute<T>::update(const Time &elapsed_time, uint32_t curTime)
{
    if(curTime != (uint32_t)(-1))
        this->m_curLocalTime = curTime;

    //if(this->m_timeBeforeRewind != (uint32_t)(-1) && this->m_curLocalTime > this->m_timeBeforeRewind)
    if(this->m_timeBeforeRewind != (uint32_t)(-1) && uint32less(this->m_timeBeforeRewind, this->m_curLocalTime))
    {
        //if((this->m_lastUpdateTime < this->m_curLocalTime || this->m_lastUpdateTime == (uint32_t)(-1))
        //&& !(this->m_value == this->m_valueBeforeRewind))
        if(uint32less(this->m_lastUpdateTime, this->m_curLocalTime) && !(this->m_value == this->m_valueBeforeRewind))
            this->m_lastUpdateTime = this->m_curLocalTime;
        this->m_timeBeforeRewind = (uint32_t)(-1);
    }

    if(this->m_syncTime == (uint32_t)(-1) || this->m_syncValues.empty())
        return (false);

    /*if(this->m_syncValues.size() == 1
    && uint32leq(this->m_syncValues.begin()->first, this->m_lastSyncTime)) ///TEST
        return (false);*/

    uint32_t interpTime = this->m_curLocalTime - m_interpolationDelay;

    auto itSyncValue = this->m_syncValues.lower_bound(interpTime);

    if(itSyncValue->first != interpTime)
    {
        if(itSyncValue == this->m_syncValues.begin())
            return (false);
        else
            (--itSyncValue);
    }

    if(itSyncValue != this->m_syncValues.begin())
    {
        this->m_syncValues.erase(this->m_syncValues.begin(), itSyncValue);
        itSyncValue = this->m_syncValues.begin();
    }

    if(this->m_firstSync)
    {
        //std::cout<<"FirstSync:"<<itSyncValue->second.x<<std::endl;
        this->m_lastSyncTime = interpTime;
        this->setValue(itSyncValue->second);
        this->m_firstSync = false;
        return (true);
    }

    //if(!(evalValue == itSyncValue->second))
    {
        float ratio = 0;
        auto nextIt = itSyncValue;

        if(itSyncValue != --this->m_syncValues.end())
        {
            nextIt = std::next(itSyncValue);
            ratio = (float)(interpTime - itSyncValue->first)/(float)(nextIt->first - itSyncValue->first);
        } else {
            interpTime = itSyncValue->first;
        }

        if(ratio > 1)
        {
            ratio = 1;
        }


        T wantedNewValue = itSyncValue->second * (1-ratio) + this->computeWantedValue(nextIt->second)*ratio;

        if(this->m_syncDelay == 0)
        {
            this->m_lastSyncTime = interpTime;
            this->setValue(wantedNewValue);
            return (true);
        }
        else
        {
            if(this->m_rewindValues.empty())
                return (false);

            T evalValue = this->m_value;
            auto evalIt = --this->m_rewindValues.end();
            if(!this->m_rewindValues.empty())
            {
                evalIt = this->m_rewindValues.lower_bound(interpTime - this->m_syncDelay);
                if(evalIt != this->m_rewindValues.begin())
                    --evalIt;
            }
            evalValue = evalIt->second;

            T deltaMove = wantedNewValue - evalValue;
            auto isSynced = glm::greaterThan(glm::abs(deltaMove), this->m_syncPrecision);

            if(T(isSynced) != T())
            {
                if(!this->m_desyncTimer.isActive())
                    this->m_desyncTimer.reset(.5f);
                else if(this->m_desyncTimer.update(elapsed_time.count()))
                {
                    this->m_lastSyncTime = interpTime;
                    while(evalIt != this->m_rewindValues.end())
                    {
                        evalIt->second += deltaMove;
                        ++evalIt;
                    }
                    this->setValue(this->m_value+deltaMove);
                    return (true);
                }
            } else
                this->m_desyncTimer.reset(.5f);


            /*else {
                deltaMove *= (float)elapsed_time.count();
                while(evalIt != this->m_rewindValues.end())
                {
                    evalIt->second += deltaMove;
                    ++evalIt;
                }
                this->setValue(this->m_value + (deltaMove));
                return (true);
            }*/
        }
    }

    return (false);
}

template<typename T>
T LinSyncedAttribute<T>::computeWantedValue(T &value)
{
    if(!m_useModulo)
        return value;

    T deltaModulo = m_maxModuloValue - m_minModuloValue;
    T wantedValue = value;

    T testValue = glm::abs(wantedValue - this->m_value) - glm::abs(wantedValue - this->m_value + deltaModulo);
    wantedValue += deltaModulo * T(glm::greaterThan(testValue, T(0)));

    testValue = glm::abs(wantedValue - this->m_value) - glm::abs(wantedValue - this->m_value - deltaModulo);
    wantedValue -= deltaModulo * T(glm::greaterThan(testValue, T(0)));

    return wantedValue;
}


template<typename T>
void LinSyncedAttribute<T>::setModuloRange(const T& min, const T& max)
{
    m_useModulo = true;
    m_minModuloValue = min;
    m_maxModuloValue = max;
}

template<typename T>
void LinSyncedAttribute<T>::setInterpolationDelay(uint32_t delay)
{
    m_interpolationDelay = delay;
}

}

