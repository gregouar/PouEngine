#ifndef PLAYABLECHARACTER_H
#define PLAYABLECHARACTER_H

#include <Character.h>

#include "assets/ItemModelAsset.h"


class PlayableCharacter : public Character
{
    public:
        PlayableCharacter();
        virtual ~PlayableCharacter();

        virtual bool loadModel(const std::string &path);
        virtual bool loadItem(const std::string &path);

        virtual void setWalkingSpeed(float speed);

        virtual void askToAttack(glm::vec2 direction = glm::vec2(0,0));
        virtual bool attack(glm::vec2 direction = glm::vec2(0,0), const std::string &animationName = "attack");
        virtual void lookAt(glm::vec2 position);
        virtual void forceAttackMode(bool force = true);
        virtual void askToDash(glm::vec2 direction = glm::vec2(0,0));
        virtual bool dash(glm::vec2 direction = glm::vec2(0,0));

        virtual void update(const pou::Time &elapsedTime);

        virtual const std::list<Hitbox> *getHitboxes() const;

    protected:
        virtual void updateGearsAttributes();

    protected:
        std::vector<ItemModelAsset *> m_gearsModel;
        std::list<Hitbox>             m_hitboxes;

    private:
        //bool        m_isInCombatMode;
        //float   m_combatModeDelay;
        pou::Timer  m_combatModeTimer;

        //float       m_wantToAttackDelay;
        pou::Timer  m_wantToAttackTimer;
        glm::vec2   m_wantToAttackDirection;

        pou::Timer  m_dashDelayTimer, m_dashTimer;
        //bool        m_isDashing;
        glm::vec2   m_dashDirection;
        //float       m_dashDelay, m_dashTime;

        float       m_normalWalkingSpeed;

        pou::Timer  m_wantToDashTimer;
        //float       m_wantToDashDelay;
        glm::vec2   m_wantToDashDirection;

        bool        m_isLateralWalking;
        glm::vec2   m_lookingAt;

        static const float DEFAULT_COMBATMODE_DELAY;
        static const float DEFAULT_WANTTOATTACK_DELAY;
        static const float DEFAULT_DASH_DELAY;
        static const float DEFAULT_DASH_TIME;
        static const float DEFAULT_DASH_SPEED;
        static const float DEFAULT_WANTTODASH_DELAY;
};

#endif // PLAYABLECHARACTER_H
