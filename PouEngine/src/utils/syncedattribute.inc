#include "PouEngine/utils/SyncedAttribute.h"

#include "PouEngine/net/NetEngine.h"

#include <iostream>

namespace pou
{

template<typename T>
SyncedAttribute<T>::SyncedAttribute() :
    m_value(T()),
    m_syncValue(T()),
    m_firstSync(true),
    m_curLocalTime(0),
    m_lastUpdateTime(-1),
    //m_lastSyncValue(T()),
    m_syncTime(-1),
    m_lastSyncTime(-1),
    m_maxRewindAmount(NetEngine::getMaxRewindAmount()),
    m_syncDelay(0),
    m_syncPrecision(T())
{
}

template<typename T>
SyncedAttribute<T>::SyncedAttribute(const T &t, float localTime) :
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
    if(t.m_lastUpdateTime > m_syncTime)
    {
        //if(m_syncTime > m_curLocalTime)
        //if(m_curLocalTime > m_syncTime)
          //  this->setValue(m_syncValue);

        if(m_lastSyncTime != m_syncTime && m_syncTime != -1)
            this->setValue(m_syncValue, m_syncTime);

        m_syncValue = t.getValue();
        m_syncTime  = t.m_lastUpdateTime - pou::NetEngine::getSyncDelay();
    }
}

template<typename T>
bool SyncedAttribute<T>::update(const Time &elapsed_time, float curTime)
{

    if(curTime != -1)
        m_curLocalTime = curTime;

    if(m_syncTime < 0)
        return (false);

    //if(m_syncValue == m_value /*|| m_syncTime < curTime*/)
      //  return (false);

    if(m_firstSync)
    {
        m_firstSync = false;
        m_lastSyncTime = m_syncTime;
        this->setValue(m_syncValue);
        return (true);
    }

    if(curTime >= m_syncTime && m_syncTime > m_lastSyncTime)
    {
        this->setValue(m_syncValue);
        m_lastSyncTime = m_syncTime;
        return (true);
    }

    return (false);
}

template<typename T>
void SyncedAttribute<T>::setValue(const T &t)
{
    this->setValue(t, m_curLocalTime);
}

template<typename T>
void SyncedAttribute<T>::setValue(const T &t, float curLocalTime)
{
    if(m_value == t && m_lastUpdateTime != -1)
        return;

    m_value = t;
    if(curLocalTime > m_lastUpdateTime)
        m_lastUpdateTime = curLocalTime;

    if(m_maxRewindAmount > 0)
    {
        m_rewindValues.insert({curLocalTime, t});
        if(m_rewindValues.size() >= m_maxRewindAmount)
            m_rewindValues.erase(m_rewindValues.begin());
    }
}

template<typename T>
bool SyncedAttribute<T>::rewind(float time)
{
    if(m_rewindValues.empty())
        return (false);

    bool r = true;
    auto it = m_rewindValues.upper_bound(time);
    if(it == m_rewindValues.end())
        return (false);
    /*if(it == m_rewindValues.end())
    {
        it = m_rewindValues.begin();
        r = false;
    }*/

    m_lastUpdateTime = it->first;
    this->setValue(it->second);
    m_rewindValues.erase(++it, m_rewindValues.end());

    return (r);
}

template<typename T>
void SyncedAttribute<T>::setSyncDelay(float syncDelay)
{
    m_syncDelay = syncDelay;
}

template<typename T>
void SyncedAttribute<T>::setSyncPrecision(T precision)
{
    m_syncPrecision = precision;
}

template<typename T>
const T &SyncedAttribute<T>::getValue() const
{
    return m_value;
}

template<typename T>
float SyncedAttribute<T>::getLastUpdateTime() const
{
    return m_lastUpdateTime;
}

template<typename T>
float SyncedAttribute<T>::getSyncTime() const
{
    return m_syncTime;
}

///LinSyncedAttribute

template<typename T>
LinSyncedAttribute<T>::LinSyncedAttribute() :
    SyncedAttribute<T>(),
    m_useModulo(false)
{

}

template<typename T>
LinSyncedAttribute<T>::LinSyncedAttribute(const T &t, float localTime) :
    SyncedAttribute<T>(t, localTime),
    m_useModulo(false)
{
}

template<typename T>
void LinSyncedAttribute<T>::syncFrom(const LinSyncedAttribute<T> &t)
{
    if(t.m_lastUpdateTime > this->m_syncTime)
    {
        /*this->m_lastSyncTime = this->m_syncTime;
        this->m_lastSyncValue = this->m_syncValue;*/

        this->m_lastSyncTime    = t.m_lastUpdateTime - pou::NetEngine::getSyncDelay();
        this->m_lastSyncValue   = this->m_value;

        this->m_syncValue   = t.getValue();
        this->m_syncTime    = t.m_lastUpdateTime;

        /*this->m_syncTime    = t.m_lastUpdateTime;
        this->m_syncValue   = t.getValue();
        m_syncTimesAndValuesList.push_back({t.m_lastUpdateTime, t.getValue()});*/
    }
}


template<typename T>
bool LinSyncedAttribute<T>::update(const Time &elapsed_time, float curTime)
{
    auto lastTime = curTime - elapsed_time.count();
    if(curTime != -1)
        this->m_curLocalTime = curTime;

    if(this->m_syncTime == -1)
        return (false);

    if(this->m_firstSync)
    {
        this->setValue(this->m_syncValue);
        this->m_firstSync = false;
        return (true);
    }

    if(this->m_syncValue == this->m_value || this->m_syncTime < lastTime)
        return (false);

    if(curTime > this->m_lastSyncTime && curTime < this->m_syncTime)
    {
        float ratio = (curTime - this->m_lastSyncTime)/(this->m_syncTime - this->m_lastSyncTime);

        T wantedNewValue = this->m_lastSyncValue * (1-ratio) + this->m_syncValue*ratio;

        if(this->m_syncDelay == 0)
        {
            this->setValue(wantedNewValue);
            return (true);
        }
        else
        {
            auto isSynced = glm::greaterThan(glm::abs(this->m_value - wantedNewValue), this->m_syncPrecision);
            if(T(isSynced) == T())
                this->m_syncTimer.reset(this->m_syncDelay);
            else if(this->m_syncTimer.update(elapsed_time))
            {
                this->setValue(wantedNewValue);
                return (true);
            } else if(!this->m_syncTimer.isActive())
                this->m_syncTimer.reset(this->m_syncDelay);
         }
    }

    return (false);
}

template<typename T>
T LinSyncedAttribute<T>::computeWantedValue()
{
    if(!m_useModulo)
        return this->m_syncValue;

    T deltaModulo = m_maxModuloValue - m_minModuloValue;
    T wantedValue = this->m_syncValue;


    T testValue = glm::abs(wantedValue - this->m_value) - glm::abs(wantedValue - this->m_value + deltaModulo);
    wantedValue += deltaModulo * T(glm::greaterThan(testValue, T(0)));

    testValue = glm::abs(wantedValue - this->m_value) - glm::abs(wantedValue - this->m_value - deltaModulo);
    wantedValue -= deltaModulo * T(glm::greaterThan(testValue, T(0)));

    /*if(glm::abs(wantedValue - this->m_value) > glm::abs(wantedValue - this->m_value + deltaModulo))
        wantedValue += deltaModulo;

    if(glm::abs(wantedValue - this->m_value) > glm::abs(wantedValue - this->m_value - deltaModulo))
        wantedValue -= deltaModulo;*/

    return wantedValue;
}


template<typename T>
void LinSyncedAttribute<T>::setModuloRange(const T& min, const T& max)
{
    m_useModulo = true;
    m_minModuloValue = min;
    m_maxModuloValue = max;
}

}
