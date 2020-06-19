#include "character/CharacterInput.h"

PlayerAction::PlayerAction(PlayerActionType type) :
    actionType(type),
    direction(0),
    value(0)
{
}

PlayerAction::PlayerAction(PlayerActionType type, glm::vec2 d) : PlayerAction(type)
{
    direction = d;
}

PlayerAction::PlayerAction(PlayerActionType type, int v) : PlayerAction(type)
{
    value = v;
}

///
///CharacterInput
///

CharacterInput::CharacterInput() :
    m_lookingDirection(glm::vec2(0,-1),0),
    m_walkingDirection(glm::vec2(0),0),
    m_attackingInput(false,0),
    m_dashingInput(false,0),
    m_pushedInput(false,0)
{
    //m_lookingDirection.setReconciliationPrecision(glm::vec2(.2f));
    //ctor
}

CharacterInput::~CharacterInput()
{
    //dtor
}

void CharacterInput::reset()
{
    m_walkingDirection.setValue(glm::vec2(0));
    m_attackingInput.setValue(0);
    m_dashingInput.setValue(0);
}

void CharacterInput::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    m_lookingAt.update(elapsedTime, localTime);
    m_lookingDirection.update(elapsedTime, localTime);
    m_walkingDirection.update(elapsedTime, localTime);
    m_attackingInput.update(elapsedTime, localTime);
    m_attackingDirection.update(elapsedTime, localTime);
    m_dashingInput.update(elapsedTime, localTime);
    m_dashingDirection.update(elapsedTime, localTime);
    m_pushedInput.update(elapsedTime, localTime);
    m_pushedDirection.update(elapsedTime, localTime);

    m_pushedInput.setValue(false);
}

void CharacterInput::syncFrom(CharacterInput *input)
{
    m_lookingAt.syncFrom(input->m_lookingAt);
    m_lookingDirection.syncFrom(input->m_lookingDirection);
    m_walkingDirection.syncFrom(input->m_walkingDirection);
    m_attackingInput.syncFrom(input->m_attackingInput);
    m_attackingDirection.syncFrom(input->m_attackingDirection);
    m_dashingInput.syncFrom(input->m_dashingInput);
    m_dashingDirection.syncFrom(input->m_dashingDirection);
    m_pushedInput.syncFrom(input->m_pushedInput);
    m_pushedDirection.syncFrom(input->m_pushedDirection);
}

void CharacterInput::serialize(pou::Stream *stream, uint32_t clientTime)
{
    bool updateWalking = false;
    if(!stream->isReading() && uint32less(clientTime,m_walkingDirection.getLastUpdateTime()))
        updateWalking = true;
    stream->serializeBool(updateWalking);
    if(updateWalking)
    {
        auto walking = m_walkingDirection.getValue();

        stream->serializeFloat(walking.x,-1,1,2);
        stream->serializeFloat(walking.y,-1,1,2);

        if(stream->isReading())
            m_walkingDirection.setValue(walking, true);
    }

    bool updateLooking = false;
    if(!stream->isReading() && uint32less(clientTime,m_lookingDirection.getLastUpdateTime()))
        updateLooking = true;
    stream->serializeBool(updateLooking);
    if(updateLooking)
    {
        auto looking = m_lookingDirection.getValue();

        stream->serializeFloat(looking.x,-1,1,2);
        stream->serializeFloat(looking.y,-1,1,2);

        if(stream->isReading())
            m_lookingDirection.setValue(looking, true);
    }

    {
        bool isAttacking = m_attackingInput.getValue();
        stream->serializeBool(isAttacking);
        if(stream->isReading())
            m_attackingInput.setValue(isAttacking, true);
    }

    {
        bool isDashing = m_dashingInput.getValue();
        stream->serializeBool(isDashing);
        if(stream->isReading())
            m_dashingInput.setValue(isDashing, true);
    }
}

void CharacterInput::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_lookingDirection.setReconciliationDelay(serverDelay, clientDelay);
    m_walkingDirection.setReconciliationDelay(serverDelay, clientDelay);
    m_attackingInput.setReconciliationDelay(serverDelay, clientDelay);
    m_dashingInput.setReconciliationDelay(serverDelay, clientDelay);
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

std::pair<bool, glm::vec2>   CharacterInput::getDashingInputs()
{
    return {m_dashingInput.getValue(), m_dashingDirection.getValue()};
}

std::pair<bool, glm::vec2>   CharacterInput::getPushedInputs()
{
    return {m_pushedInput.getValue(), m_pushedDirection.getValue()};
}


uint32_t CharacterInput::getLastUpdateTime()
{
    return m_lastUpdateTime;
}



void CharacterInput::setLookingAt(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    if(m_lookingAt.setValue(direction))
        this->setLastUpdateTime(m_lookingAt.getLastUpdateTime());
}

void CharacterInput::setLookingDirection(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    if(m_lookingDirection.setValue(direction))
        this->setLastUpdateTime(m_lookingDirection.getLastUpdateTime());
}

void CharacterInput::setWalkingDirection(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    if(m_walkingDirection.setValue(direction))
        this->setLastUpdateTime(m_walkingDirection.getLastUpdateTime());
}

void CharacterInput::setAttacking(bool attackingInput, glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    if(m_attackingInput.setValue(attackingInput))
        this->setLastUpdateTime(m_attackingInput.getLastUpdateTime());
    if(m_attackingDirection.setValue(direction))
        this->setLastUpdateTime(m_attackingDirection.getLastUpdateTime());
}

void CharacterInput::setDashing(bool dashingInput, glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    if(m_dashingInput.setValue(dashingInput))
        this->setLastUpdateTime(m_dashingInput.getLastUpdateTime());
    if(m_dashingDirection.setValue(direction))
        this->setLastUpdateTime(m_dashingDirection.getLastUpdateTime());
}

void CharacterInput::setPush(bool pushedInput, glm::vec2 direction)
{
    if(m_pushedInput.setValue(pushedInput))
        this->setLastUpdateTime(m_pushedInput.getLastUpdateTime());
    if(m_pushedDirection.setValue(direction))
        this->setLastUpdateTime(m_pushedDirection.getLastUpdateTime());
}

void CharacterInput::setLastUpdateTime(uint32_t time)
{
    if(uint32less(m_lastUpdateTime, time))
        m_lastUpdateTime = time;
}


///
/// PlayerInput
///

const float PlayerInput::DEFAULT_COMBATMODE_DELAY = 3.0f;
const float PlayerInput::DEFAULT_WANTTOATTACK_DELAY = .1f;
const float PlayerInput::DEFAULT_WANTTODASH_DELAY = .5f;


PlayerInput::PlayerInput() :
    m_wantToLookAt(false),
    m_wantToLook(false),
    m_wantToWalk(false)
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
    CharacterInput::update(elapsedTime, localTime);

    m_combatModeTimer.update(elapsedTime);
    m_wantToAttackTimer.update(elapsedTime);
    m_wantToDashTimer.update(elapsedTime);

    if(m_wantToLookAt)
    {
        this->setLookingAt(m_wantToLookAtDirection);
        m_wantToLookAt = false;
    }

    if(m_wantToLook)
    {
        this->setLookingDirection(m_wantedLookingDirection);
        m_wantToLook = false;
    }

    if(m_wantToWalk)
    {
        this->setWalkingDirection(m_wantedWalkingDirection);
        m_wantToWalk = false;
    }

    if(m_wantToAttackTimer.isActive())
        this->setAttacking(true, m_wantToAttackDirection);
    else
        this->setAttacking(false);

    if(m_wantToDashTimer.isActive())
        this->setDashing(true, m_wantToDashDirection);
    else
        this->setDashing(false);

    if(m_combatModeTimer.isActive())
        this->setLookingDirection(this->getLookingAt());
    else if(this->getWalkingInputs().second != glm::vec2(0))
        this->setLookingDirection(this->getWalkingInputs().second);
}

void PlayerInput::processAction(const PlayerAction &playerAction)
{
    switch(playerAction.actionType)
    {
        case PlayerActionType_CursorMove:{
            m_wantToLookAt = true;
            m_wantToLookAtDirection = playerAction.direction;
            //m_lookingAt.setValue(playerAction.direction);
           //this->setLookingAt(playerAction.direction);
        }break;
        case PlayerActionType_Look:{
            m_wantToLook = true;
            m_wantedLookingDirection =  playerAction.direction;
            //m_lookingDirection.setValue(playerAction.direction);
        }break;
        case PlayerActionType_Walk:{
            m_wantToWalk = true;
            m_wantedWalkingDirection = playerAction.direction;
        }break;
        case PlayerActionType_Dash:{
            m_wantToDashTimer.reset(DEFAULT_WANTTODASH_DELAY);
            m_wantToDashDirection = glm::vec2(0);

            if(playerAction.direction == glm::vec2(0))
                m_wantToDashDirection = this->getWalkingInputs().second;
            else
                m_wantToDashDirection = playerAction.direction;
        }break;
        case PlayerActionType_Attack:{
            m_combatModeTimer.reset(DEFAULT_COMBATMODE_DELAY);
            m_wantToAttackTimer.reset(DEFAULT_WANTTOATTACK_DELAY);

            if(playerAction.direction == glm::vec2(0))
                m_wantToAttackDirection = this->getLookingDirectionInput();
            else
            {
                m_wantToAttackDirection     = playerAction.direction;
                m_wantedLookingDirection    =  playerAction.direction;
                m_wantToLook = true;
            }
        }break;
        case PlayerActionType_UseItem:{
            ///player->useGear(playerAction.value);
        }break;
        case PlayerActionType_CombatMode:{
            m_combatModeTimer.reset(DEFAULT_COMBATMODE_DELAY);
        }break;
        case NBR_PLAYERACTIONTYPES:{
        }
    }
}


