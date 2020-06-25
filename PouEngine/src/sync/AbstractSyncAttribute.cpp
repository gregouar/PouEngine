#include "PouEngine/sync/AbstractSyncAttribute.h"

#include "PouEngine/net/NetEngine.h"

bool uint32less(uint32_t lhs, uint32_t rhs);
bool uint32leq(uint32_t lhs, uint32_t rhs);

#include <iostream>

namespace pou
{

AbstractSyncAttribute::AbstractSyncAttribute() :
    m_firstSync(true),
    m_curLocalTime(0),
    m_lastUpdateTime(-1),
    m_syncTime(-1),
    m_lastSyncTime(-1),
    m_maxRewindAmount(NetEngine::getMaxRewindAmount()),
    m_reconciliationDelay_client(0),
    m_reconciliationDelay_server(0)
{
}

AbstractSyncAttribute::~AbstractSyncAttribute()
{

}

void AbstractSyncAttribute::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    if(clientDelay == uint32_t(-1))
        clientDelay = serverDelay;

    m_reconciliationDelay_server = serverDelay;
    m_reconciliationDelay_client = clientDelay;
}

void AbstractSyncAttribute::setMaxRewindAmount(size_t maxRewind)
{
    m_maxRewindAmount = maxRewind;
}

uint32_t AbstractSyncAttribute::getLastUpdateTime() const
{
    return m_lastUpdateTime;
}

uint32_t AbstractSyncAttribute::getSyncTime() const
{
    return m_syncTime;
}

}
