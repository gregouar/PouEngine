#include "character/CharacterInput.h"

CharacterInput::CharacterInput() :
    m_lookingDirection(glm::vec2(0,-1),0),
    m_walkingDirection(glm::vec2(0),0),
    m_attackingInput(false,0)
{
    //ctor
}

CharacterInput::~CharacterInput()
{
    //dtor
}


void CharacterInput::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_lookingAt.update(elapsedTime, localTime);
    m_lookingDirection.update(elapsedTime, localTime);
    m_walkingDirection.update(elapsedTime, localTime);
    m_attackingDirection.update(elapsedTime, localTime);
}

void CharacterInput::syncFrom(CharacterInput *input)
{
    m_lookingAt.syncFrom(input->m_lookingAt);
    m_lookingDirection.syncFrom(input->m_lookingDirection);
    m_walkingDirection.syncFrom(input->m_walkingDirection);
    m_attackingDirection.syncFrom(input->m_attackingDirection);
}

void CharacterInput::serialize(pou::Stream *stream, uint32_t clientTime)
{
    bool updateWalking = false;
    if(!stream->isReading() && uint32less(clientTime,m_walkingDirection.getLastUpdateTime()))
        updateWalking = true;
    stream->serializeBool(updateWalking);
    if(updateWalking)
    {
        auto walking = m_walkingDirection.getValue(true);

        stream->serializeFloat(walking.x);
        stream->serializeFloat(walking.y);

        if(stream->isReading())
        {
            m_walkingDirection.setValue(walking, true);
            //this->setLastCharacterUpdateTime(m_walkingDirection.getLastUpdateTime());
        }
    }

    bool updateLooking = false;
    if(!stream->isReading() && uint32less(clientTime,m_lookingDirection.getLastUpdateTime()))
        updateLooking = true;
    stream->serializeBool(updateLooking);
    if(updateLooking)
    {
        auto looking = m_lookingDirection.getValue(true);

        stream->serializeFloat(looking.x);
        stream->serializeFloat(looking.y);

        if(stream->isReading())
        {
            m_lookingDirection.setValue(looking, true);
            //this->setLastCharacterUpdateTime(m_lookingDirection.getLastUpdateTime());
        }
    }

    {
        bool isAttacking = m_attackingInput.getValue(true);
        stream->serializeBool(isAttacking);
        if(stream->isReading())
            m_attackingInput.setValue(isAttacking, true);
    }
}

void CharacterInput::setSyncDelay(uint32_t delay)
{
    m_lookingAt.setSyncDelay(delay);
    m_lookingDirection.setSyncDelay(delay);
    m_walkingDirection.setSyncDelay(delay);
    m_attackingDirection.setSyncDelay(delay);
}

glm::vec2 CharacterInput::getLookingAt()
{
    return m_lookingAt.getValue();
}

glm::vec2 CharacterInput::getLookingDirectionInput()
{
    return m_lookingDirection.getValue();
}

std::pair<bool, glm::vec2>   CharacterInput::getWalkingInputs()
{
    return {m_walkingDirection.getValue() != glm::vec2(0), m_walkingDirection.getValue()};
}

std::pair<bool, glm::vec2>   CharacterInput::getAttackingInputs()
{
    return {m_attackingInput.getValue(), m_attackingDirection.getValue()};
}



///
/// PlayerInput
///

const float PlayerInput::DEFAULT_COMBATMODE_DELAY = 3.0f;
const float PlayerInput::DEFAULT_WANTTOATTACK_DELAY = .1f;
const float PlayerInput::DEFAULT_WANTTODASH_DELAY = .5f;


PlayerInput::PlayerInput() //:
   // m_wantToWalkDirection(glm::vec2(0))
{
    //ctor
}

PlayerInput::~PlayerInput()
{
    //dtor
}


void PlayerInput::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_combatModeTimer.update(elapsedTime);
    m_wantToAttackTimer.update(elapsedTime);
    m_wantToDashTimer.update(elapsedTime);

    m_attackingInput.setValue(false);
    if(m_wantToAttackTimer.isActive())
    {
        m_attackingInput.setValue(true);
        m_attackingDirection.setValue(m_wantToAttackDirection);
    }
   // else
     //   m_walkingDirection.setValue(m_wantToWalkDirection);

    if(m_combatModeTimer.isActive())
    {
        m_lookingDirection.setValue(m_lookingAt.getValue());
    } else if(m_walkingDirection.getValue() != glm::vec2(0)) {
        m_lookingDirection.setValue(m_walkingDirection.getValue());
    }

    CharacterInput::update(elapsedTime, localTime);
}

void PlayerInput::processAction(const PlayerAction &playerAction)
{
    switch(playerAction.actionType)
    {
        case PlayerActionType_CursorMove:{
           // player->lookAt(player->getGlobalXYPosition() + playerAction.direction *100.0f);
           m_lookingAt.setValue(playerAction.direction);
        }break;
        case PlayerActionType_Look:{
            m_lookingDirection.setValue(playerAction.direction);
        }break;
        case PlayerActionType_Walk:{
            //m_wantToWalkDirection = playerAction.direction;
            m_walkingDirection.setValue(playerAction.direction);
        }break;
        case PlayerActionType_Dash:{
            m_wantToDashTimer.reset(DEFAULT_WANTTODASH_DELAY);
            m_wantToDashDirection = playerAction.direction;
        }break;
        case PlayerActionType_Attack:{
            m_combatModeTimer.reset(DEFAULT_COMBATMODE_DELAY);
            m_wantToAttackTimer.reset(DEFAULT_WANTTOATTACK_DELAY);
            m_wantToAttackDirection = playerAction.direction;
        }break;
        case PlayerActionType_UseItem:{
            ///player->useGear(playerAction.value);
        }break;
        case PlayerActionType_CombatMode:{
            m_combatModeTimer.reset(DEFAULT_COMBATMODE_DELAY);
        }break;
    }
}


