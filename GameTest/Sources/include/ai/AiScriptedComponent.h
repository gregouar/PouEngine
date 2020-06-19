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

    protected:
        AiScriptModelAsset *m_model;

    private:
        Character *m_target;
};

#endif // AISCRIPTEDCOMPONENT_H
