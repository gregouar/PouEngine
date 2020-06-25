#ifndef SYNCCOMPONENT_H
#define SYNCCOMPONENT_H

#include "PouEngine/sync/AbstractSyncElement.h"

namespace pou
{

class SyncComponent
{
    public:
        SyncComponent();
        virtual ~SyncComponent();

        virtual bool update(const Time &elapsedTime, uint32_t localTime = -1);

        virtual void syncFrom(SyncComponent *syncComponent); //syncComponent MUST have the same syncedElement types
        virtual void serialize(Stream *stream, uint32_t clientTime);

        virtual void addSyncElement(AbstractSyncElement *netSyncElement);
        bool containsSyncElement(AbstractSyncElement *netSyncElement);

        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);

        void setLastUpdateTime(uint32_t time);

        uint32_t getLastUpdateTime();

    protected:

    private:
        std::vector<AbstractSyncElement*> m_syncElements;

        uint32_t m_lastUpdateTime;
};

}

#endif // SYNCCOMPONENT_H
