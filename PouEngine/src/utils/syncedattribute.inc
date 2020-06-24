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
    m_reconciliationDelay_client(0),
    m_reconciliationDelay_server(0),
    m_reconciliationPrecision(T())
    ///m_timeBeforeRewind(-1)
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
    //if(uint32less(m_syncTime, t.m_lastUpdateTime))
    {
        m_syncValues.insert_or_assign(t.m_lastUpdateTime, t.getValue());

        if(uint32less(m_syncTime, t.m_lastUpdateTime))
            m_syncTime  = t.m_lastUpdateTime;
    }
}

template<typename T>
bool SyncedAttribute<T>::update(const Time &elapsed_time, uint32_t curTime)
{
    if(curTime != (uint32_t)(-1))
        m_curLocalTime = curTime;

    /**if(m_timeBeforeRewind != (uint32_t)(-1) && uint32less(m_timeBeforeRewind, m_curLocalTime))
    {
        if((m_lastUpdateTime < m_curLocalTime || m_lastUpdateTime == (uint32_t)(-1))
        && !(m_value == m_valueBeforeRewind))
            m_lastUpdateTime = m_curLocalTime;
        m_timeBeforeRewind = (uint32_t)(-1);
    }**/

    if(m_syncTime == (uint32_t)(-1) || m_syncValues.empty())
        return (false);

    uint32_t serverLocalTime = 0;
    if(this->m_reconciliationDelay_server < m_curLocalTime)
        serverLocalTime = m_curLocalTime - this->m_reconciliationDelay_server;

    auto itSyncValue = m_syncValues.upper_bound(serverLocalTime);

    //if(itSyncValue == this->m_syncValues.end() || itSyncValue->first != serverLocalTime)
    {
        if(itSyncValue == this->m_syncValues.begin())
            return (false);
        else
            (--itSyncValue);
    }

    m_syncValues.erase(m_syncValues.begin(), itSyncValue);

    /*if(itSyncValue != m_syncValues.begin()) //Don't need the condition though
    {
        m_syncValues.erase(m_syncValues.begin(), itSyncValue);
        itSyncValue = m_syncValues.begin();
    }*/

    if(m_firstSync)
    {
        m_firstSync = false;
        this->m_rewindValues.clear();
        m_lastSyncTime = itSyncValue->first;
        this->setValue(itSyncValue->second, true);
        return (true);
    }

    if(this->m_reconciliationDelay_client == 0)
    {
        if(uint32less(m_lastSyncTime, itSyncValue->first))
        {
            this->setValue(itSyncValue->second);
            m_lastSyncTime = itSyncValue->first;
            return (true);
        }
    }
    else
    {
        if(this->m_rewindValues.empty())
            return (false);

        auto evalIt = --this->m_rewindValues.end();
        if(!this->m_rewindValues.empty())
        {
            uint32_t localClientTime = 0;
            if(this->m_reconciliationDelay_client < this->m_curLocalTime)
                localClientTime = this->m_curLocalTime - this->m_reconciliationDelay_client;

            evalIt = this->m_rewindValues.upper_bound(localClientTime);
            if(evalIt != this->m_rewindValues.begin())
                --evalIt;
        }
        T evalValue = evalIt->second;

        /*if(!(evalValue == itSyncValue->second))
        {
            this->m_lastSyncTime = itSyncValue->first;
            this->setValue(itSyncValue->second);
            return (true);
        }*/

        if(!(evalValue == itSyncValue->second))
        {
            if(!this->m_desyncTimer.isActive())
                this->m_desyncTimer.reset(.2f);
            else if(this->m_desyncTimer.update(elapsed_time.count()))
            {
                this->m_lastSyncTime = itSyncValue->first;
                this->setValue(itSyncValue->second);
                this->m_desyncTimer.reset();
                return (true);
            }
        } else {
            if(this->m_desyncTimer.isActive())
            this->m_desyncTimer.reset();
        }
     }

    return (false);
}

template<typename T>
bool SyncedAttribute<T>::setValue(const T &t, bool forceUpdate)
{
    return this->setValue(t, m_curLocalTime, forceUpdate);
}

template<typename T>
bool SyncedAttribute<T>::setValue(const T &t, uint32_t curLocalTime, bool forceUpdate)
{
    if(m_value == t && m_lastUpdateTime != (uint32_t)(-1) && !forceUpdate)
        return (false);

    m_value = t;
    if(uint32less(m_lastUpdateTime, curLocalTime))
        m_lastUpdateTime = curLocalTime;

    if(m_maxRewindAmount > 0 && curLocalTime  != (uint32_t)(-1))
    {
        m_rewindValues.insert_or_assign(curLocalTime, t);
        if(m_rewindValues.size() >= m_maxRewindAmount)
            m_rewindValues.erase(m_rewindValues.begin());
    }

    return (true);
}

/**template<typename T>
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
}**/

template<typename T>
void SyncedAttribute<T>::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    if(clientDelay == uint32_t(-1))
        clientDelay = serverDelay;

    m_reconciliationDelay_server = serverDelay;
    m_reconciliationDelay_client = clientDelay;
}

template<typename T>
void SyncedAttribute<T>::setReconciliationPrecision(T precision)
{
    m_reconciliationPrecision = precision;
}

template<typename T>
void SyncedAttribute<T>::setMaxRewindAmount(size_t maxRewind)
{
    m_maxRewindAmount = maxRewind;
}


template<typename T>
const T &SyncedAttribute<T>::getValue() const
{
    return m_value;
}

/*template<typename T>
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
}*/

template<typename T>
uint32_t SyncedAttribute<T>::getLastUpdateTime() const
{
    return m_lastUpdateTime;
}

/*template<typename T>
uint32_t SyncedAttribute<T>::getLastUpdateTime(bool useRewind) const
{
    if(useRewind)
        return (m_lastUpdateTime + m_syncDelay);
    return m_lastUpdateTime;
}*/

template<typename T>
uint32_t SyncedAttribute<T>::getSyncTime() const
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
LinSyncedAttribute<T>::LinSyncedAttribute(const T &t, uint32_t localTime) :
    SyncedAttribute<T>(t, localTime),
    m_useModulo(false)
{
}


template<typename T>
bool LinSyncedAttribute<T>::update(const Time &elapsed_time, uint32_t curTime)
{
    if(curTime != (uint32_t)(-1))
        this->m_curLocalTime = curTime;

    /**if(this->m_timeBeforeRewind != (uint32_t)(-1) && uint32less(this->m_timeBeforeRewind, this->m_curLocalTime))
    {
        if(uint32less(this->m_lastUpdateTime, this->m_curLocalTime) && !(this->m_value == this->m_valueBeforeRewind))
            this->m_lastUpdateTime = this->m_curLocalTime;
        this->m_timeBeforeRewind = (uint32_t)(-1);
    }**/

    if(this->m_syncTime == (uint32_t)(-1) || this->m_syncValues.empty())
        return (false);

    uint32_t interpTime = 0;
    if(this->m_curLocalTime > this->m_reconciliationDelay_server)
        interpTime = this->m_curLocalTime - this->m_reconciliationDelay_server;

    uint32_t syncTime = interpTime;

    auto itSyncValue = this->m_syncValues.upper_bound(interpTime);

    //if(itSyncValue == this->m_syncValues.end() || itSyncValue->first != interpTime)
    {
        if(itSyncValue == this->m_syncValues.begin())
            return (false);
        else
            (--itSyncValue);

        this->m_syncValues.erase(this->m_syncValues.begin(), itSyncValue);
    }

    if(this->m_firstSync)
    {
        this->m_lastSyncTime = interpTime;
        this->m_rewindValues.clear();
        this->setValue(itSyncValue->second, true);
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
            syncTime = itSyncValue->first;
        }

        if(ratio > 1)
            ratio = 1;

        T wantedNewValue = itSyncValue->second * (1-ratio) + this->computeWantedValue(nextIt->second)*ratio;

        if(this->m_reconciliationDelay_client == 0)
        {
            if(uint32less(this->m_lastSyncTime, syncTime))
            {
                this->m_lastSyncTime = syncTime;
                this->setValue(wantedNewValue);
                return (true);
            }
        }
        else
        {
            if(this->m_rewindValues.empty())
                return (false);

            T evalValue = this->m_value;
            auto evalIt = --this->m_rewindValues.end();
            if(!this->m_rewindValues.empty())
            {
                evalIt = this->m_rewindValues.lower_bound(this->m_curLocalTime - this->m_reconciliationDelay_client);
                if(evalIt != this->m_rewindValues.begin())
                    --evalIt;
            }
            evalValue = evalIt->second;

            T deltaMove = wantedNewValue - evalValue;
            auto isSynced = glm::greaterThan(glm::abs(deltaMove), this->m_reconciliationPrecision);

                /*std::cout<<"SyncTime:"<<itSyncValue->first<<std::endl;
                std::cout<<"NextSyncTime:"<<nextIt->first<<std::endl;
                std::cout<<"InterpTime:"<<interpTime<<std::endl;
                std::cout<<"SyncValue:"<<itSyncValue->second.x<<std::endl;
                std::cout<<"NextSyncValue:"<<nextIt->second.x<<std::endl;
                std::cout<<"WantedValue:"<<wantedNewValue.x<<std::endl;
                std::cout<<"RealValue:"<<evalValue.x<<std::endl;
                std::cout<<std::endl;*/

            if(T(isSynced) != T())
            {
                deltaMove *= elapsed_time.count()*2;
                while(evalIt != this->m_rewindValues.end())
                {
                    evalIt->second += deltaMove;
                    ++evalIt;
                }
                this->setValue(this->m_value+deltaMove);
                return (true);

                /*if(!this->m_desyncTimer.isActive())
                    this->m_desyncTimer.reset(1.0f);
                else if(this->m_desyncTimer.update(elapsed_time.count()))
                {
                    this->m_lastSyncTime = syncTime;
                    while(evalIt != this->m_rewindValues.end())
                    {
                        evalIt->second += deltaMove;
                        ++evalIt;
                    }
                    this->setValue(this->m_value+deltaMove);
                    this->m_desyncTimer.reset(1.0f);
                    return (true);
                }*/
            } /**else
                this->m_desyncTimer.reset(1.0f);**/
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

}

