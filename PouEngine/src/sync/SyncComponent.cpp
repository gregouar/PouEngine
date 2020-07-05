#include "PouEngine/sync/SyncComponent.h"

namespace pou
{

SyncComponent::SyncComponent() :
    //m_parentSyncComponent(nullptr),
    m_curLocalTime(0),
    m_lastUpdateTime(-1),
    m_disableSync(false)
{
    //ctor
}

SyncComponent::~SyncComponent()
{
    //dtor
}

bool SyncComponent::update(const Time &elapsedTime, uint32_t localTime)
{
    if(localTime != uint32_t(-1))
        m_curLocalTime = localTime;

    bool r = false;
    for(auto *syncElement : m_syncElements)
        r |= syncElement->update(elapsedTime, localTime);

    /**for(auto *syncComponent : m_syncSubComponents)
        r |= syncComponent->update(elapsedTime, localTime);**/

    if(r)
        this->setLastUpdateTime(localTime);

    return (r);
}

void SyncComponent::syncFrom(const SyncComponent &syncComponent)
{
    if(m_disableSync)
        return;

    auto it1 = m_syncElements.begin();
    auto it2 = syncComponent.m_syncElements.begin();
    for(;it1 != m_syncElements.end(); ++it1, ++it2)
        (*it1)->syncFrom(*it2);

    /**auto c1 = m_syncSubComponents.begin();
    auto c2 = syncComponent->m_syncSubComponents.begin();
    for(;c1 != m_syncSubComponents.end(); ++c1, ++c2)
        (*c1)->syncFrom(*c2);**/
}

void SyncComponent::serialize(Stream *stream, uint32_t clientTime)
{
    for(auto *syncElement : m_syncElements)
        syncElement->serialize(stream, clientTime);

    /**for(auto *syncComponent : m_syncSubComponents)
        syncComponent->serialize(stream, clientTime);**/
}


/**void SyncComponent::addSyncSubComponent(SyncComponent *syncComponent)
{
    if(!syncComponent)
        return;

    if(!this->containsSyncSubComponent(syncComponent))
    {
        m_syncSubComponents.push_back(syncComponent);
        syncComponent->setParentSyncComponent(this);
    }
}

void SyncComponent::removeSyncSubComponent(SyncComponent *syncComponent)
{
    for(auto it = m_syncSubComponents.begin() ; it != m_syncSubComponents.end() ; ++it)
        if(*it == syncComponent)
        {
            m_syncSubComponents.erase(it);
            return;
        }
}

bool SyncComponent::containsSyncSubComponent(SyncComponent *syncComponent)
{
    for(auto *syncComponentIt : m_syncSubComponents)
        if(syncComponentIt == syncComponent)
            return (true);
    return (false);
}**/

void SyncComponent::addSyncElement(AbstractSyncElement *syncElement)
{
    if(!syncElement)
        return;

    if(!this->containsSyncElement(syncElement))
    {
        m_syncElements.push_back(syncElement);
        syncElement->setSyncComponent(this);
    }
}

bool SyncComponent::containsSyncElement(AbstractSyncElement *syncElement)
{
    for(auto *syncElementIt : m_syncElements)
        if(syncElementIt == syncElement)
            return (true);
    return (false);
}

void SyncComponent::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    for(auto *syncElement : m_syncElements)
        syncElement->setReconciliationDelay(serverDelay, clientDelay);
    /**for(auto *syncComponent : m_syncSubComponents)
        syncComponent->setReconciliationDelay(serverDelay, clientDelay);**/
}

void SyncComponent::setMaxRewindAmount(int maxRewind)
{
    for(auto *syncElement : m_syncElements)
        syncElement->setMaxRewindAmount(maxRewind);
}

void SyncComponent::updateLastUpdateTime()
{
    this->setLastUpdateTime(m_curLocalTime);
}

void SyncComponent::setLastUpdateTime(uint32_t time)
{
    if(uint32less(m_lastUpdateTime, time))
        m_lastUpdateTime = time;
    /**if(m_parentSyncComponent)
        m_parentSyncComponent->setLastUpdateTime(time);**/
}

void SyncComponent::disableSync(bool disable)
{
    m_disableSync = disable;
}

uint32_t SyncComponent::getLastUpdateTime()
{
    return m_lastUpdateTime;
}

uint32_t SyncComponent::getLocalTime()
{
    return m_curLocalTime;
}

///
///Protected
///

/**void SyncComponent::setParentSyncComponent(SyncComponent *syncComponent)
{
    m_parentSyncComponent = syncComponent;
}**/


}
