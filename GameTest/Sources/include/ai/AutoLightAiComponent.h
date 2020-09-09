#ifndef AUTOLIGHTAICOMPONENT_H
#define AUTOLIGHTAICOMPONENT_H

#include "ai/AiComponent.h"


class AutoLightAiComponent : public AiComponent
{
    public:
        AutoLightAiComponent(Character *character);
        virtual ~AutoLightAiComponent();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

    protected:
        //void lookForTarget(float maxDistance);

    private:
        bool m_activated;

        const pou::HashedString ANIMATION_NAME_ACTIVATE;
};

#endif // AUTOLIGHTAICOMPONENT_H
