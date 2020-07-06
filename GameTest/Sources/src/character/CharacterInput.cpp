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
    m_lookingDirection(glm::vec2(0,-1)),
    m_walkingDirection(glm::vec2(0)),
    m_attackingInput(false),
    m_dashingInput(false),
    m_pushedInput(false)
{
    m_walkingDirection.setMinMaxAndPrecision(glm::vec2(-1),glm::vec2(1),glm::uvec2(2));
    m_syncComponent.addSyncElement(&m_walkingDirection);

    m_lookingDirection.setMinMaxAndPrecision(glm::vec2(-1),glm::vec2(1),glm::uvec2(2));
    m_syncComponent.addSyncElement(&m_lookingDirection);

    m_syncComponent.addSyncElement(&m_attackingInput);
    m_syncComponent.addSyncElement(&m_dashingInput);
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
    m_syncComponent.update(elapsedTime, localTime);
    m_pushedInput.setValue(false);
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

pou::SyncComponent *CharacterInput::getSyncComponent()
{
    return &m_syncComponent;
}

/*uint32_t CharacterInput::getLastUpdateTime()
{
    return m_lastUpdateTime;
}*/



void CharacterInput::setLookingAt(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    m_lookingAt.setValue(direction);
    //if(m_lookingAt.setValue(direction))
      //  this->setLastUpdateTime(m_lookingAt.getLastUpdateTime());
}

void CharacterInput::setLookingDirection(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    m_lookingDirection.setValue(direction);
    //if(m_lookingDirection.setValue(direction))
      //  this->setLastUpdateTime(m_lookingDirection.getLastUpdateTime());
}

void CharacterInput::setWalkingDirection(glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    m_walkingDirection.setValue(direction);

    //if(m_walkingDirection.setValue(direction))
      //  this->setLastUpdateTime(m_walkingDirection.getLastUpdateTime());
}

void CharacterInput::setAttacking(bool attackingInput, glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    m_attackingInput.setValue(attackingInput);
    m_attackingDirection.setValue(direction);

    //if(m_attackingInput.setValue(attackingInput))
      //  this->setLastUpdateTime(m_attackingInput.getLastUpdateTime());
    //if(m_attackingDirection.setValue(direction))
      //  this->setLastUpdateTime(m_attackingDirection.getLastUpdateTime());
}

void CharacterInput::setDashing(bool dashingInput, glm::vec2 direction)
{
    if(direction != glm::vec2(0))
        direction = glm::normalize(direction);

    m_dashingInput.setValue(dashingInput);
    m_dashingDirection.setValue(direction);

    //if(m_dashingInput.setValue(dashingInput))
      //  this->setLastUpdateTime(m_dashingInput.getLastUpdateTime());
    //if(m_dashingDirection.setValue(direction))
      //  this->setLastUpdateTime(m_dashingDirection.getLastUpdateTime());
}

void CharacterInput::setPush(bool pushedInput, glm::vec2 direction)
{
    m_pushedInput.setValue(pushedInput);
    m_pushedDirection.setValue(direction);

    //if(m_pushedInput.setValue(pushedInput))
      //  this->setLastUpdateTime(m_pushedInput.getLastUpdateTime());
    //if(m_pushedDirection.setValue(direction))
      //  this->setLastUpdateTime(m_pushedDirection.getLastUpdateTime());
}

/*void CharacterInput::setLastUpdateTime(uint32_t time)
{
    if(uint32less(m_lastUpdateTime, time))
        m_lastUpdateTime = time;
}*/


///
/// PlayerInput
///

const float PlayerInput::DEFAULT_COMBATMODE_DELAY = 3.0f;
const float PlayerInput::DEFAULT_WANTTOATTACK_DELAY = .1f;
const float PlayerInput::DEFAULT_WANTTODASH_DELAY = .5f;


PlayerInput::PlayerInput() : CharacterInput(),
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


