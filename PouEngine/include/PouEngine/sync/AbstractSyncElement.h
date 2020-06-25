#ifndef ABSTRACTSYNCELEMENT_H
#define ABSTRACTSYNCELEMENT_H

#include "PouEngine/sync/SyncAttribute.h"

namespace pou
{

class SyncComponent;

class AbstractSyncElement
{
    friend class SyncComponent;

    public:
        AbstractSyncElement(AbstractSyncAttribute *syncAttributePtr);
        virtual ~AbstractSyncElement();

        bool update(const Time &elapsedTime, uint32_t localTime = -1);

        void updateLastUpdateTime();

        virtual void syncFrom(AbstractSyncElement *syncElement);
        virtual void serialize(Stream *stream, uint32_t clientTime);

        virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);
        void useUpdateBit(bool use = true);

        uint32_t getLastUpdateTime();

    protected:
        void setSyncComponent(SyncComponent *syncComponent);


        virtual void serializeImpl(Stream *stream, uint32_t clientTime) = 0;

    private:
        AbstractSyncAttribute *m_syncAttributePtr;
        SyncComponent *m_syncComponent;

        bool m_useUpdateBit;
};

}

#endif // AbstractSyncElement_H
