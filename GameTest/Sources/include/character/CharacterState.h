#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include "PouEngine/Types.h"
#include "PouEngine/system/Timer.h"

#include "character/CharacterInput.h"

#include <set>

class Character;

enum CharacterStateTypes
{
    CharacterStateType_Standing,
    CharacterStateType_Walking,
    CharacterStateType_Attacking,
    CharacterStateType_Dashing,
    CharacterStateType_Interrupted,
    //CharacterStateType_Pushed,
    CharacterStateType_Dead,
    NBR_CharacterStateTypes
};


class CharacterState
{
    public:
        CharacterState(uint16_t type, Character *character);
        virtual ~CharacterState();

        virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        virtual void entered(CharacterInput *input);
        virtual void leaving(CharacterInput *input);

    protected:
        void switchState(CharacterStateTypes stateType/*, CharacterInput *input*/);

        void rotateCharacterToward(const pou::Time &elapsedTime, glm::vec2 direction);

    private:
        uint16_t    m_type;

    protected:
        Character  *m_character;
};

class CharacterState_Standing : public CharacterState
{
    public:
        CharacterState_Standing(Character *character);
        virtual ~CharacterState_Standing();

        virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime = -1);

        virtual void entered(CharacterInput *input);

    protected:
        glm::vec2 m_lookingDirection;

    private:

};

class CharacterState_Walking : public CharacterState
{
    public:
        CharacterState_Walking(Character *character);
        virtual ~CharacterState_Walking();

        virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);
        virtual void leaving(CharacterInput *input);

    protected:
        glm::vec2 m_lookingDirection;
        glm::vec2 m_walkingDirection;

    private:
        bool m_isLateralWalking;
};

class CharacterState_Attacking : public CharacterState
{
    public:
        CharacterState_Attacking(Character *character);
        virtual ~CharacterState_Attacking();

        //virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);

    protected:

    private:
        pou::Timer  m_attackTimer;
        glm::vec2   m_attackingDirection;

        ///std::set<std::shared_ptr<Character> > m_alreadyHitCharacters;
        std::set<Character*> m_alreadyHitCharacters;
};

class CharacterState_Dashing : public CharacterState
{
    public:
        CharacterState_Dashing(Character *character);
        virtual ~CharacterState_Dashing();

        //virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);
        virtual void leaving(CharacterInput *input);

    protected:
        glm::vec2 m_dashingDirection;

    private:
        pou::Timer  m_dashDelayTimer,
                    m_dashTimer;

        static const float DEFAULT_DASH_DELAY;
        static const float DEFAULT_DASH_TIME;
        static const float DEFAULT_DASH_SPEED;
};

class CharacterState_Interrupted : public CharacterState
{
    public:
        CharacterState_Interrupted(Character *character);
        virtual ~CharacterState_Interrupted();

        //virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);

    protected:

    private:
        pou::Timer m_interruptTimer;

        glm::vec2   m_pushDirection;
        pou::Timer  m_pushTimer;

    static const float DEFAULT_INTERRUPT_DELAY;
    static const float DEFAULT_PUSH_TIME;
};

/*class CharacterState_Pushed : public CharacterState_Interrupted
{
    public:
        CharacterState_Pushed(Character *character);
        virtual ~CharacterState_Pushed();

        //virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);

    protected:

    private:
        glm::vec2   m_pushDirection;
        pou::Timer  m_pushTimer;

    static const float DEFAULT_PUSH_TIME;
};*/


class CharacterState_Dead : public CharacterState
{
    public:
        CharacterState_Dead(Character *character);
        virtual ~CharacterState_Dead();

        //virtual void handleInput(CharacterInput *input);
        //virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered(CharacterInput *input);

    protected:

    private:
};


#endif // CHARACTERSTATE_H
