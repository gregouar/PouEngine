#include "PouEngine/sync/SyncComponent.h"

namespace pou
{

SyncComponent::SyncComponent() :
    m_lastUpdateTime(-1)
{
    //ctor
}

SyncComponent::~SyncComponent()
{
    //dtor
}

bool SyncComponent::update(const Time &elapsedTime, uint32_t localTime)
{
    bool r = false;
    for(auto *syncElement : m_syncElements)
        r |= syncElement->update(elapsedTime, localTime);

    if(r)
        this->setLastUpdateTime(localTime);

    return (r);
}

void SyncComponent::syncFrom(SyncComponent *syncComponent)
{
    auto it1 = m_syncElements.begin();
    auto it2 = syncComponent->m_syncElements.begin();
    for(;it1 != m_syncElements.end(); ++it1, ++it2)
        (*it1)->syncFrom(*it2);
}

void SyncComponent::serialize(Stream *stream, uint32_t clientTime)
{
    for(auto *syncElement : m_syncElements)
        syncElement->serialize(stream, clientTime);
}

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
}

void SyncComponent::setLastUpdateTime(uint32_t time)
{
    if(uint32less(m_lastUpdateTime, time))
        m_lastUpdateTime = time;
}

uint32_t SyncComponent::getLastUpdateTime()
{
    return m_lastUpdateTime;
}

}
