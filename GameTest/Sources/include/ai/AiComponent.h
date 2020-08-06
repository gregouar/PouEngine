#ifndef AICOMPONENT_H
#define AICOMPONENT_H


#include "PouEngine/sync/IntSyncElement.h"

#include "ai/Pathfinder.h"
#include "character/Character.h"


class AiComponent : public pou::NotificationListener
{
    public:
        AiComponent(Character *character);
        virtual ~AiComponent();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        std::list<glm::vec2> &getPlannedPath();


        //virtual void syncFrom(AiComponent *aiComponent);
        //virtual void serialize(pou::Stream *stream, uint32_t clientTime);

        //virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);

        //uint32_t getLastUpdateTime();

        void setTarget(Character *target);
        Character *getTarget();

        pou::SyncComponent *getSyncComponent();

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;

        void avoidCollisionsTo(glm::vec2 destination);


    protected:
        Character *m_character;

        Character *m_target;
        pou::IntSyncElement m_targetId;

        pou::SyncComponent m_syncComponent;

    private:
        ///I should store last known path and update it at regular intervals
        Pathfinder m_pathfinder;
        pou::Timer m_pathfindingTimer;
};

#endif // AICOMPONENT_H
