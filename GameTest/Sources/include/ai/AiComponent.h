#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include "character/Character.h"

class AiComponent : public pou::NotificationListener
{
    public:
        AiComponent(Character *character);
        virtual ~AiComponent();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

    protected:
        virtual void notify(pou::NotificationSender*, int notificationType,
                            void* data) override;


    protected:
        Character *m_character;

    private:
};

#endif // AICOMPONENT_H
