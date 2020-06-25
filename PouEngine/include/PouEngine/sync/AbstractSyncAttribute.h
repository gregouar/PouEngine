#ifndef ABSTRACTSyncAttribute_H
#define ABSTRACTSyncAttribute_H

#include "PouEngine/Types.h"
#include "PouEngine/system/Timer.h"

namespace pou
{

class AbstractSyncAttribute
{
    public:
        AbstractSyncAttribute();
        virtual ~AbstractSyncAttribute();

        virtual void syncFrom(const AbstractSyncAttribute *t) = 0;
        virtual bool update(const Time &elapsed_time, uint32_t curTime) = 0;

        uint32_t getLastUpdateTime() const;
        uint32_t getSyncTime() const;

        void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1); //-1 means it takes the server value
        void setMaxRewindAmount(size_t maxRewind);

    protected:
        bool m_firstSync;
        uint32_t m_curLocalTime;
        uint32_t m_lastUpdateTime;

        uint32_t m_syncTime;
        uint32_t m_lastSyncTime;

        size_t m_maxRewindAmount;

        uint32_t    m_reconciliationDelay_client,
                    m_reconciliationDelay_server;
};

}

#endif // ABSTRACTSyncAttribute_H
