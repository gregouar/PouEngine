#include "PouEngine/utils/SyncedAttribute.h"

#include <iostream>

namespace pou
{

template<typename T>
SyncedAttribute<T>::SyncedAttribute() :
    m_value(T()),
    m_syncValue(T()),
    m_lastUpdateTime(-1),
    m_lastSyncTime(-1),
    m_curLocalTime(0)
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
    if(t.m_lastUpdateTime > m_lastSyncTime)
    {
        if(m_lastSyncTime > m_curLocalTime)
            this->setValue(m_syncValue);

        m_syncValue = t.getValue();
        m_lastSyncTime = t.m_lastUpdateTime;
    }
}

template<typename T>
bool SyncedAttribute<T>::update(const Time &elapsed_time, float curTime)
{
    auto lastTime = curTime - elapsed_time.count();
    if(m_curLocalTime == -1)
        lastTime = -1;

    if(curTime != -1)
        m_curLocalTime = curTime;

    if(m_syncValue == m_value || m_lastSyncTime < curTime)
        return (false);

    if(lastTime == -1)
    {
        this->setValue(m_syncValue,m_lastSyncTime);
        return (true);
    }

    if(curTime > m_lastSyncTime)
        this->setValue(m_syncValue,m_lastSyncTime);

    return (true);
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
float SyncedAttribute<T>::getLastSyncTime() const
{
    return m_lastSyncTime;
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
    if(t.m_lastUpdateTime > this->m_lastSyncTime)
    {
        this->m_syncValue = t.getValue();
        this->m_lastSyncTime = t.m_lastUpdateTime;
    }
}


template<typename T>
bool LinSyncedAttribute<T>::update(const Time &elapsed_time, float curTime)
{
    auto lastTime = curTime - elapsed_time.count();

    if(this->m_curLocalTime == -1)
        lastTime = -1;

    if(curTime != -1)
        this->m_curLocalTime = curTime;

    if(this->m_syncValue == this->m_value || this->m_lastSyncTime < lastTime)
        return (false);

    if(lastTime == -1)
    {
        this->setValue(this->m_syncValue,this->m_lastSyncTime);
        return (true);
    }

    float delta = this->m_lastSyncTime - lastTime;
    float ratio = elapsed_time.count()/delta;

    if(ratio > 1 || ratio < 0)
        this->setValue(this->m_syncValue,lastTime);
    else
        this->setValue(this->m_value + ratio * (this->computeWantedValue() - this->m_value),lastTime);

    this->m_lastUpdateTime = this->m_lastSyncTime;

    return (true);
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
