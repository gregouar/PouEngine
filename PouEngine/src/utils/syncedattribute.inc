#include "PouEngine/utils/SyncedAttribute.h"

#include "PouEngine/net/NetEngine.h"

#include <iostream>

namespace pou
{

template<typename T>
SyncedAttribute<T>::SyncedAttribute() :
    m_value(T()),
    m_syncValue(T()),
    //m_lastSyncValue(T()),
    m_lastUpdateTime(-1),
    m_syncTime(-1),
    m_lastSyncTime(-1),
    m_curLocalTime(0),
    m_firstSync(true)
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
        m_syncTime  = t.m_lastUpdateTime;
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
    if(m_value == t)
        return;

    m_value = t;
    if(curLocalTime > m_lastUpdateTime)
        m_lastUpdateTime = curLocalTime;
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
    /*if(curTime != -1)
        this->m_curLocalTime = curTime;

    if(m_syncTimesAndValuesList.empty())
        return (false);

    if(this->m_curLocalTime == 0)
    {
        this->setValue(m_syncTimesAndValuesList.begin()->second);
        return (true);
    }

    while( m_syncTimesAndValuesList.size() > 1 && (++m_syncTimesAndValuesList.begin())->first <= curTime)
    {
        m_syncTimesAndValuesList.pop_front();
    }

    if(m_syncTimesAndValuesList.size() <= 1)
        return (false);

    float firstTime = (m_syncTimesAndValuesList.begin())->first;
    float secondTime = (++m_syncTimesAndValuesList.begin())->first;
    if(this->m_curLocalTime > firstTime && this->m_curLocalTime < secondTime)
    {
        T &firstValue = (m_syncTimesAndValuesList.begin())->second;
        T &secondValue = (++m_syncTimesAndValuesList.begin())->second;

        float ratio = (this->m_curLocalTime - firstTime)/(secondTime - firstTime);
        this->setValue(firstValue*(1-ratio) + secondValue*ratio);
        std::cout<<"Ratio:"<<ratio<<std::endl;
        return (true);
    }

    return (false);*/

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
        this->setValue(this->m_lastSyncValue * (1-ratio) + this->m_syncValue*ratio);
        return (true);
    }

    return (false);






    /**auto lastTime = curTime - elapsed_time.count();

    if(this->m_curLocalTime == 0)
        lastTime = 0;

    if(curTime != -1)
        this->m_curLocalTime = curTime;

    if(this->m_syncValue == this->m_value || this->m_syncTime < lastTime)
        return (false);

    if(lastTime == 0)
    {
        this->setValue(this->m_syncValue,this->m_syncTime);
        return (true);
    }

    float delta = this->m_syncTime - lastTime;
    float ratio = elapsed_time.count()/delta;

    if(ratio > 1 || ratio < 0)
        this->setValue(this->m_syncValue,curTime);
    else
        this->setValue(this->m_value + ratio * (this->computeWantedValue() - this->m_value),curTime);

    return (true);**/
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
