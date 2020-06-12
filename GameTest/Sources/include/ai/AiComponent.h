#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include "character/Character.h"

class AiComponent
{
    public:
        AiComponent(Character *character);
        virtual ~AiComponent();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

    protected:
        Character *m_character;

    private:
};

#endif // AICOMPONENT_H
