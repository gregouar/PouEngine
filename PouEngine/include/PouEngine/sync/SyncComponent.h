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

        virtual void syncFrom(const SyncComponent &syncComponent); //syncComponent MUST have the same syncedElement and subcomponent types
        virtual void serialize(Stream *stream, uint32_t clientTime);

        /**virtual void addSyncSubComponent(SyncComponent *syncComponent);
        virtual void removeSyncSubComponent(SyncComponent *syncComponent);
        bool containsSyncSubComponent(SyncComponent *syncComponent);**/

        virtual void addSyncElement(AbstractSyncElement *syncElement);
        bool containsSyncElement(AbstractSyncElement *syncElement);

        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        virtual void setMaxRewindAmount(int maxRewind);
        virtual void disableSync(bool disable = true);

        void updateLastUpdateTime();
        void setLastUpdateTime(uint32_t time);

        uint32_t getLastUpdateTime();
        uint32_t getLocalTime();

    protected:
        void setParentSyncComponent(SyncComponent *syncComponent);

    private:

        std::vector<AbstractSyncElement*> m_syncElements;

        ///SyncComponent *m_parentSyncComponent;
        ///std::vector<SyncComponent*> m_syncSubComponents;

        uint32_t m_curLocalTime;
        uint32_t m_lastUpdateTime;
        bool m_disableSync;
};

}

#endif // SYNCCOMPONENT_H
