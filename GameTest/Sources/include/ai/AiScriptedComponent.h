#ifndef AISCRIPTEDCOMPONENT_H
#define AISCRIPTEDCOMPONENT_H

#include "ai/AiComponent.h"


class AiScriptedComponent : public AiComponent
{
    public:
        AiScriptedComponent(Character *character);
        virtual ~AiScriptedComponent();

        void createFromModel(AiScriptModelAsset *model);

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;

        void lookForTarget(float maxDistance);
        //void avoidCollisionsTo(glm::vec2 destination);

    protected:
        AiScriptModelAsset *m_model;

    private:
        /*///I should store last known path and update it at regular intervals
        Pathfinder m_pathfinder;
        pou::Timer m_pathfindingTimer;*/
};

#endif // AISCRIPTEDCOMPONENT_H
