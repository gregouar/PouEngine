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
        /*///I could store last known path and update it at regular intervals
        Pathfinder m_pathfinder;
        pou::Timer m_pathfindingTimer;*/

        ///Implement this for fun
        glm::vec2 m_fixedPosition;
        glm::vec2 m_randomDestination;


};

#endif // AISCRIPTEDCOMPONENT_H
