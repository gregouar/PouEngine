#ifndef PLAYABLECHARACTER_H
#define PLAYABLECHARACTER_H

#include <character/Character.h>

#include "assets/ItemModelAsset.h"

enum PlayerActionType
{
    PlayerActionType_Walk,
    NBR_PLAYERACTIONTYPES,
};

struct PlayerAction
{
    int actionType;

    glm::vec2 walkDirection;
};


class PlayableCharacter : public Character
{
    public:
        PlayableCharacter();
        virtual ~PlayableCharacter();

        //virtual bool loadModel(const std::string &path);
        virtual bool setModel(CharacterModelAsset *model);
        virtual bool loadItem(const std::string &path);

       // virtual void setWalkingSpeed(float speed);

        virtual void askToWalk(glm::vec2 direction);
        virtual void askToAttack(glm::vec2 direction = glm::vec2(0,0));
        virtual void askToDash(glm::vec2 direction = glm::vec2(0,0));

        virtual void walk(glm::vec2 direction);
        virtual bool attack(glm::vec2 direction = glm::vec2(0,0), const std::string &animationName = "attack");
        virtual void lookAt(glm::vec2 position);
        virtual void forceAttackMode(bool force = true);
        virtual bool dash(glm::vec2 direction = glm::vec2(0,0));

        virtual void update(const pou::Time &elapsedTime, float localTime = -1);

        virtual const std::list<Hitbox> *getHitboxes() const;

        bool syncFromPlayer(PlayableCharacter *srcPlayer);

        virtual void    setSyncAndLocalTime(float syncTime);
        void            setLastPlayerUpdateTime(float time, bool force = false);
        float           getLastPlayerUpdateTime();

    protected:
        virtual void updateGearsAttributes();

    protected:
        std::vector<ItemModelAsset *> m_gearsModel;
        std::list<Hitbox>             m_hitboxes;

        float m_lastPlayerSyncTime;
        float m_lastPlayerUpdateTime;

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

        glm::vec2   m_wantToWalkDirection;
        //float       m_normalWalkingSpeed;

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
