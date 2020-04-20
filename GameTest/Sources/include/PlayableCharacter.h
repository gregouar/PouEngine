#ifndef PLAYABLECHARACTER_H
#define PLAYABLECHARACTER_H

#include <Character.h>

class PlayableCharacter : public Character
{
    public:
        PlayableCharacter();
        virtual ~PlayableCharacter();

        virtual void attack(glm::vec2 direction = glm::vec2(0,0), const std::string &animationName = "attack");
        virtual void lookAt(glm::vec2 position);
        virtual void forceAttackMode(bool force = true);

        virtual void update(const pou::Time &elapsedTime);

    protected:

    private:
        bool    m_isInCombatMode;
        float   m_combatDelay;

        bool        m_isLateralWalking;
        glm::vec2   m_lookingAt;

        static const float DEFAULT_COMBAT_DELAY;
};

#endif // PLAYABLECHARACTER_H
