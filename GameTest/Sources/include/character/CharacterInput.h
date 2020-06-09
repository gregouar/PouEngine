#ifndef CHARACTERINPUT_H
#define CHARACTERINPUT_H

#include "PouEngine/Types.h"
#include "PouEngine/utils/Timer.h"
#include "PouEngine/utils/SyncedAttribute.h"

enum PlayerActionType
{
    PlayerActionType_CursorMove,
    PlayerActionType_Walk,
    PlayerActionType_Look,
    PlayerActionType_Attack,
    PlayerActionType_Dash,
    PlayerActionType_UseItem,
    PlayerActionType_CombatMode,
    NBR_PLAYERACTIONTYPES,
};

struct PlayerAction
{
    int actionType;

    glm::vec2   direction;
    int         value;
};

class CharacterInput
{
    public:
        CharacterInput();
        virtual ~CharacterInput();

        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);
        virtual void syncFrom(CharacterInput *input);
        virtual void serialize(pou::Stream *stream, uint32_t clientTime);

        virtual void setSyncDelay(uint32_t delay);

        glm::vec2 getLookingAt(); //Passively looking at
        glm::vec2 getLookingDirectionInput(); //Actively looking toward
        std::pair<bool, glm::vec2>   getWalkingInputs();
        std::pair<bool, glm::vec2>   getAttackingInputs();

    protected:
        pou::SyncedAttribute<glm::vec2> m_lookingAt; //Passively looking at
        pou::SyncedAttribute<glm::vec2> m_lookingDirection; //Actively looking toward

        pou::SyncedAttribute<glm::vec2> m_walkingDirection;

        pou::SyncedAttribute<bool>      m_attackingInput;
        pou::SyncedAttribute<glm::vec2> m_attackingDirection;

    private:
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
        //glm::vec2       m_wantToWalkDirection;

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
