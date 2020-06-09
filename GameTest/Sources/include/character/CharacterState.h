#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include "PouEngine/Types.h"
#include "PouEngine/utils/Timer.h"

#include "character/CharacterInput.h"

#include <set>

class Character;

enum CharacterStateTypes
{
    CharacterStateType_Standing,
    CharacterStateType_Walking,
    CharacterStateType_Attacking,
   // CharacterStateType_Dashing,
    CharacterStateType_Interrupted,
   // CharacterStateType_Pushed,
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

        virtual void entered();
        virtual void leaving();

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

        virtual void entered();

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

        virtual void entered();
        virtual void leaving();

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

        virtual void entered();

    protected:

    private:
        pou::Timer m_attackTimer;

        std::set<Character*> m_alreadyHitCharacters;
};


class CharacterState_Interrupted : public CharacterState
{
    public:
        CharacterState_Interrupted(Character *character);
        virtual ~CharacterState_Interrupted();

        //virtual void handleInput(CharacterInput *input);
        virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered();

    protected:

    private:
        pou::Timer m_interruptTimer;
};


class CharacterState_Dead : public CharacterState
{
    public:
        CharacterState_Dead(Character *character);
        virtual ~CharacterState_Dead();

        //virtual void handleInput(CharacterInput *input);
        //virtual void update(const pou::Time &elapsedTime, uint32_t localTime);

        virtual void entered();

    protected:

    private:
};


#endif // CHARACTERSTATE_H
