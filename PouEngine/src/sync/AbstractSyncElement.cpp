#include "PouEngine/sync/AbstractSyncElement.h"

#include "PouEngine/sync/SyncComponent.h"

namespace pou
{

AbstractSyncElement::AbstractSyncElement(pou::AbstractSyncAttribute *syncAttributePtr) :
    m_syncAttributePtr(syncAttributePtr),
    m_syncComponent(nullptr),
    m_useUpdateBit(false),
    m_disableSync(false)
{
    assert(m_syncAttributePtr);
}

AbstractSyncElement::~AbstractSyncElement()
{
    //dtor
}


bool AbstractSyncElement::update(const Time &elapsedTime, uint32_t localTime)
{
    if(m_disableSync)
        return m_syncAttributePtr->updateWithoutSync(elapsedTime, localTime);
    return m_syncAttributePtr->update(elapsedTime, localTime);
}

void AbstractSyncElement::updateLastUpdateTime()
{
    if(m_syncComponent)
        m_syncComponent->setLastUpdateTime(m_syncAttributePtr->getLastUpdateTime());
}

void AbstractSyncElement::syncFrom(const AbstractSyncElement *syncElement)
{
    if(m_disableSync)
        return;

    m_syncAttributePtr->syncFrom(syncElement->m_syncAttributePtr);
}

void AbstractSyncElement::serialize(Stream *stream, uint32_t clientTime)
{
    bool update = !m_useUpdateBit;

    if(m_useUpdateBit)
    {
        if(!stream->isReading() && uint32less(clientTime,m_syncAttributePtr->getLastUpdateTime()))
            update = true;
        stream->serializeBool(update);
    }

    if(update)
        this->serializeImpl(stream, clientTime);
}

void AbstractSyncElement::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_syncAttributePtr->setReconciliationDelay(serverDelay, clientDelay);
}

void AbstractSyncElement::setMaxRewindAmount(size_t maxRewind)
{
    m_syncAttributePtr->setMaxRewindAmount(maxRewind);
}

void AbstractSyncElement::useUpdateBit(bool use)
{
    m_useUpdateBit = use;
}

void AbstractSyncElement::disableSync(bool disable)
{
    m_disableSync = disable;
}

uint32_t AbstractSyncElement::getLastUpdateTime()
{
    return m_syncAttributePtr->getLastUpdateTime();
}

///Protected

void AbstractSyncElement::setSyncComponent(SyncComponent *syncComponent)
{
    m_syncComponent = syncComponent;
}

}
