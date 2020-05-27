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
        m_syncValue = t.getValue();
        m_lastSyncTime = t.m_lastUpdateTime;
    }
}

template<typename T>
bool SyncedAttribute<T>::update(const Time &elapsed_time, float curTime)
{
    if(curTime != -1)
        m_curLocalTime = curTime + elapsed_time.count();

    if(m_syncValue == m_value || m_lastSyncTime < curTime)
        return (false);

    if(curTime == -1)
    {
        this->setValue(m_syncValue,m_lastSyncTime);
        return (true);
    }

    float delta = m_lastSyncTime - curTime;
    float ratio = elapsed_time.count()/delta;

    if(ratio > 1 || ratio < 0)
        this->setValue(m_syncValue,curTime);
    else
        this->setValue(m_value + ratio * (m_syncValue - m_value),curTime);

    m_lastUpdateTime = m_lastSyncTime;

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

}
