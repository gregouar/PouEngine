#ifndef CHARACTERINPUT_H
#define CHARACTERINPUT_H

#include "PouEngine/Types.h"
#include "PouEngine/system/Timer.h"
#include "PouEngine/sync/SyncElements.h"

enum PlayerActionType
{
    PlayerActionType_CursorMove,
    PlayerActionType_Walk,
    PlayerActionType_Look,
    PlayerActionType_Attack,
    PlayerActionType_Dash,
    PlayerActionType_UseItem,
    PlayerActionType_CombatMode,
    PlayerActionType_Respawn,
    NBR_PLAYERACTIONTYPES,
};

struct PlayerAction
{
    PlayerAction(PlayerActionType type = NBR_PLAYERACTIONTYPES);
    PlayerAction(PlayerActionType type, glm::vec2 d);
    PlayerAction(PlayerActionType type, int v);

    PlayerActionType actionType;

    glm::vec2   direction;
    int         value;
};

class CharacterInput
{
    public:
        CharacterInput();
        virtual ~CharacterInput();

        virtual void reset();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        //virtual void syncFrom(CharacterInput *input);
        //virtual void serialize(pou::Stream *stream, uint32_t clientTime);

        void setPush(bool pushing, glm::vec2 direction = glm::vec2(0));

        //virtual void setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay = -1);

        glm::vec2 getLookingAt(); //Passively looking at
        glm::vec2 getLookingDirectionInput(); //Actively looking toward
        std::pair<bool, glm::vec2>   getWalkingInputs();
        std::pair<bool, glm::vec2>   getAttackingInputs();
        std::pair<bool, glm::vec2>   getDashingInputs();
        std::pair<bool, glm::vec2>   getPushedInputs();

        //uint32_t getLastUpdateTime();
        pou::SyncComponent *getSyncComponent();

    //protected:
        void setLookingAt(glm::vec2 direction);
        void setLookingDirection(glm::vec2 direction);
        void setWalkingDirection(glm::vec2 direction);
        void setAttacking(bool attackingInput, glm::vec2 direction = glm::vec2(0));
        void setDashing(bool dashingInput, glm::vec2 direction = glm::vec2(0));

    protected:
        //void setLastUpdateTime(uint32_t time);

    protected:

    private:
        //uint32_t m_lastUpdateTime;

        pou::SyncComponent   m_syncComponent;

        pou::Vec2SyncElement m_lookingAt; //Passively looking at
        pou::Vec2SyncElement m_lookingDirection; //Actively looking toward

        pou::Vec2SyncElement m_walkingDirection;

        pou::BoolSyncElement m_attackingInput;
        pou::Vec2SyncElement m_attackingDirection;

        pou::BoolSyncElement m_dashingInput;
        pou::Vec2SyncElement m_dashingDirection;

        pou::BoolSyncElement m_pushedInput;
        pou::Vec2SyncElement m_pushedDirection;

        /*pou::SyncAttribute<glm::vec2> m_lookingAt; //Passively looking at
        pou::SyncAttribute<glm::vec2> m_lookingDirection; //Actively looking toward

        pou::SyncAttribute<glm::vec2> m_walkingDirection;

        pou::SyncAttribute<bool>      m_attackingInput;
        pou::SyncAttribute<glm::vec2> m_attackingDirection;

        pou::SyncAttribute<bool>      m_dashingInput;
        pou::SyncAttribute<glm::vec2> m_dashingDirection;

        pou::SyncAttribute<bool>      m_pushedInput;
        pou::SyncAttribute<glm::vec2> m_pushedDirection;*/
};

class PlayerInput : public CharacterInput
{
    public:
        PlayerInput();
        virtual ~PlayerInput();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        void processAction(const PlayerAction &playerAction);

    protected:

    private:
        bool            m_wantToLookAt;
        glm::vec2       m_wantToLookAtDirection;

        bool            m_wantToLook;
        glm::vec2       m_wantedLookingDirection;

        bool            m_wantToWalk;
        glm::vec2       m_wantedWalkingDirection;

        pou::Timer      m_combatModeTimer;

        pou::Timer      m_wantToAttackTimer;
        glm::vec2       m_wantToAttackDirection;

        pou::Timer      m_wantToDashTimer;
        glm::vec2       m_wantToDashDirection;

    private:
        static const float DEFAULT_COMBATMODE_DELAY;
        static const float DEFAULT_WANTTOATTACK_DELAY;
        static const float DEFAULT_WANTTODASH_DELAY;
};

#endif // CHARACTERINPUT_H
