#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include "character/Character.h"
#include "PouEngine/sync/IntSyncElement.h"


class AiComponent : public pou::NotificationListener
{
    public:
        AiComponent(Character *character);
        virtual ~AiComponent();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        //virtual void syncFrom(AiComponent *aiComponent);
        //virtual void serialize(pou::Stream *stream, uint32_t clientTime);

        //virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);

        //uint32_t getLastUpdateTime();

        pou::SyncComponent *getSyncComponent();

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;


    protected:
        Character *m_character;

        Character *m_target;
        pou::IntSyncElement m_targetId;

        pou::SyncComponent m_syncComponent;

    private:
};

#endif // AICOMPONENT_H
