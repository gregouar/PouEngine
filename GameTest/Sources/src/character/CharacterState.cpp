#include "character/CharacterState.h"

#include "character/Character.h"

CharacterState::CharacterState(uint16_t type, Character *character) :
    m_type(type),
    m_character(character)
{
    //ctor
}

CharacterState::~CharacterState()
{
    //dtor
}

void CharacterState::handleInput(CharacterInput *input)
{
}

void CharacterState::update(const pou::Time &elapsedTime, uint32_t localTime)
{
}

void CharacterState::entered(CharacterInput *input)
{

}

void CharacterState::leaving(CharacterInput *input)
{

}

void CharacterState::switchState(CharacterStateTypes stateType)
{
    m_character->switchState(stateType);
}

void CharacterState::rotateCharacterToward(const pou::Time &elapsedTime, glm::vec2 direction)
{
    if(direction == glm::vec2(0))
        direction = glm::vec2(0,-1);

    auto &modelAtt = m_character->getModelAttributes();

    ///Introduce animationRotationSpeed
    if(!modelAtt.immovable)
    {
        float curRotation = m_character->getEulerRotation().z;
        float rotationAmount = elapsedTime.count()*10.0f;
        float wantedRotation = pou::MathTools::computeWantedRotation( curRotation, direction );

        if(glm::abs(curRotation - wantedRotation) < 0.001f)
            return;

        if(glm::abs(wantedRotation - curRotation) < rotationAmount)
            m_character->setRotation({0,0,wantedRotation});
        else
            m_character->rotate(rotationAmount, {0,0, (wantedRotation > curRotation) ? 1 : -1 });
    }
}

///
/// CharacterState_Standing
///


CharacterState_Standing::CharacterState_Standing(Character *character) :
    CharacterState(CharacterStateType_Standing,character)
{

}

CharacterState_Standing::~CharacterState_Standing()
{

}

void CharacterState_Standing::handleInput(CharacterInput *input)
{
    m_lookingDirection = input->getLookingDirectionInput();

    auto [isDashing, dashingDirection] = input->getDashingInputs();
    if(isDashing)
    {
        this->switchState(CharacterStateType_Dashing);
        return;
    }

    auto [isAttacking, attackingDirection] = input->getAttackingInputs();
    if(isAttacking)
    {
        this->switchState(CharacterStateType_Attacking);
        return;
    }

    auto [isWalking, walkingDirection] = input->getWalkingInputs();
    if(isWalking)
    {
        this->switchState(CharacterStateType_Walking);
        return;
    }
}

void CharacterState_Standing::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    this->rotateCharacterToward(elapsedTime, m_lookingDirection);
}

void CharacterState_Standing::entered(CharacterInput *input)
{
    m_character->startAnimation("stand", false);
}


///
/// CharacterState_Walking
///

CharacterState_Walking::CharacterState_Walking(Character *character) :
    CharacterState(CharacterStateType_Walking,character),
    m_isLateralWalking(false)
{

}

CharacterState_Walking::~CharacterState_Walking()
{

}

void CharacterState_Walking::handleInput(CharacterInput *input)
{
    m_lookingDirection = input->getLookingDirectionInput();

    auto [isDashing, dashingDirection] = input->getDashingInputs();
    if(isDashing)
    {
        this->switchState(CharacterStateType_Dashing);
        return;
    }

    auto [isAttacking, attackingDirection] = input->getAttackingInputs();
    if(isAttacking)
    {
        this->switchState(CharacterStateType_Attacking);
        return;
    }

    auto [isWalking, walkingDirection] = input->getWalkingInputs();
    if(!isWalking)
    {
        this->switchState(CharacterStateType_Standing);
        return;
    }

    m_walkingDirection = walkingDirection;
}

void CharacterState_Walking::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    float walkingAmount = m_character->getAttributes().walkingSpeed*elapsedTime.count();
    glm::vec2 charMove = walkingAmount*m_walkingDirection;
    m_character->move(charMove);

    this->rotateCharacterToward(elapsedTime, m_lookingDirection);

    if(m_walkingDirection == glm::vec2(0))
        return;

    bool wantToLateralWalk = false;

    float deltaRotation = abs(
                    pou::MathTools::computeWantedRotation(m_character->getEulerRotation().z,m_walkingDirection)
                     - m_character->getEulerRotation().z);

    if(deltaRotation > glm::pi<float>()*.25 && deltaRotation <  glm::pi<float>()*.75)
        wantToLateralWalk = true;

    if(wantToLateralWalk && !m_isLateralWalking)
    {
        m_character->startAnimation("lateralWalk", true);
        m_isLateralWalking = true;
    }
    else if(!wantToLateralWalk && m_isLateralWalking)
    {
        m_character->startAnimation("walk", true);
        m_isLateralWalking = false;
    }
}

void CharacterState_Walking::entered(CharacterInput *input)
{
    this->handleInput(input);

    m_character->startAnimation("walk");
    m_isLateralWalking = false;
}

void CharacterState_Walking::leaving(CharacterInput *input)
{
    m_character->startAnimation("stand");
}


///
/// CharacterState_Attacking
///

CharacterState_Attacking::CharacterState_Attacking(Character *character) :
    CharacterState(CharacterStateType_Attacking,character)
{

}

CharacterState_Attacking::~CharacterState_Attacking()
{

}

void CharacterState_Attacking::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(m_attackTimer.update(elapsedTime) || !m_attackTimer.isActive())
    {
        //this->popState(); ?
        this->switchState(CharacterStateType_Standing);
    }

    this->rotateCharacterToward(elapsedTime, m_attackingDirection);

    for(auto enemy : m_character->getNearbyCharacters())
    {
        if(!enemy->isAlive())
            continue;
        if(enemy->getTeam() == m_character->getTeam())
            continue;

        auto hurtboxes  = enemy->getHurtboxes();
        auto hitboxes   = m_character->getHitboxes();
        if(!hurtboxes || !hitboxes)
            continue;

        if(m_alreadyHitCharacters.find(enemy) != m_alreadyHitCharacters.end())
            continue;

        for(const auto hitBox : *hitboxes)
        {
            auto hitSkeleton = m_character->getSkeleton(hitBox.getSkeleton());

            if(!hitSkeleton)
                continue;

            if(!hitSkeleton->hasTag("attack")) ///I should find a better way to encode this tag...
                continue;

            for(const auto hurtBox : *hurtboxes)
            {
                auto hurtSkeleton   = enemy->getSkeleton(hurtBox.getSkeleton());

                if(!hurtSkeleton)
                    continue;

                auto hitNode    = hitSkeleton->findNode(hitBox.getNode());
                auto hurtNode   = hurtSkeleton->findNode(hurtBox.getNode());

                if(hitNode != nullptr && hurtNode != nullptr)
                {

                    bool collision = pou::MathTools::detectBoxCollision(hitBox.getBox(),hurtBox.getBox(),
                                                                        hitNode,hurtNode);

                    if(collision)
                    {
                        m_alreadyHitCharacters.insert(enemy);

                        float totalDamages = 0;
                        for(auto i = 0 ; i < NBR_DAMAGE_TYPES ; ++i)
                            totalDamages += m_character->getModelAttributes().attackDamages * hitBox.getFactor(i) * hurtBox.getFactor(i);

                        enemy->damage(totalDamages,
                                      enemy->getGlobalXYPosition()-m_character->getGlobalXYPosition(),
                                      m_character->areDamagesOnlyCosmetic());
                    }
                }

            }
        }
    }
}

void CharacterState_Attacking::entered(CharacterInput *input)
{
    m_character->startAnimation("attack");
    m_attackTimer.reset(m_character->getModelAttributes().attackDelay);
    m_alreadyHitCharacters.clear();

    auto attackingDirection = input->getAttackingInputs().second;
    if(attackingDirection == glm::vec2(0))
        attackingDirection = input->getLookingDirectionInput();
    m_attackingDirection = attackingDirection;
}


///
/// CharacterState_Dashing
///

const float CharacterState_Dashing::DEFAULT_DASH_DELAY = .5f;
//const float CharacterState_Dashing::DEFAULT_DASH_TIME = .15f;
//const float CharacterState_Dashing::DEFAULT_DASH_SPEED = 1500.0f;
const float CharacterState_Dashing::DEFAULT_DASH_TIME = .3f;
const float CharacterState_Dashing::DEFAULT_DASH_SPEED = 750.0f;

CharacterState_Dashing::CharacterState_Dashing(Character *character) :
    CharacterState(CharacterStateType_Dashing,character)
{

}

CharacterState_Dashing::~CharacterState_Dashing()
{

}

void CharacterState_Dashing::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    auto elapsedDashTime = std::min(elapsedTime,m_dashTimer.remainingTime());
    float walkingAmount = DEFAULT_DASH_SPEED*elapsedDashTime.count();
    glm::vec2 charMove = walkingAmount*m_dashingDirection;
    m_character->move(charMove);

    this->rotateCharacterToward(elapsedTime*2, m_dashingDirection);

    m_dashTimer.update(elapsedTime);

    if(m_dashDelayTimer.update(elapsedTime))
        this->switchState(CharacterStateType_Standing);
}

void CharacterState_Dashing::entered(CharacterInput *input)
{
    m_character->startAnimation("dash");
    m_dashDelayTimer.reset(DEFAULT_DASH_DELAY);
    m_dashTimer.reset(DEFAULT_DASH_TIME);

    auto dashingDirection = input->getDashingInputs().second;
    if(dashingDirection == glm::vec2(0))
        dashingDirection = input->getLookingDirectionInput();
    m_dashingDirection = dashingDirection;
}

void CharacterState_Dashing::leaving(CharacterInput *input)
{
    m_character->startAnimation("stand");
}


///
/// CharacterState_Interrupted
///

const float CharacterState_Interrupted::DEFAULT_INTERRUPT_DELAY = .5f;
const float CharacterState_Interrupted::DEFAULT_PUSH_TIME = .2f;

CharacterState_Interrupted::CharacterState_Interrupted(Character *character) :
    CharacterState(CharacterStateType_Interrupted,character)
{

}

CharacterState_Interrupted::~CharacterState_Interrupted()
{

}

void CharacterState_Interrupted::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(m_pushTimer.isActive())
    {
        auto elapsedPushTime = std::min(elapsedTime,m_pushTimer.remainingTime());
        glm::vec2 charMove = m_pushDirection*(float)elapsedPushTime.count();
        m_character->move(charMove);

        m_pushTimer.update(elapsedTime);
    }


    if(m_interruptTimer.update(elapsedTime) || !m_interruptTimer.isActive())
    {
        //popState ?
        this->switchState(CharacterStateType_Standing);
    }


}

void CharacterState_Interrupted::entered(CharacterInput *input)
{
    m_character->startAnimation("interrupt");
    m_interruptTimer.reset(DEFAULT_INTERRUPT_DELAY); ///Could be computed from something or something

    auto [pushedInput, pushedDirection] = input->getPushedInputs();
    if(pushedInput)
    {
        m_pushDirection = pushedDirection;
        m_pushTimer.reset(DEFAULT_PUSH_TIME);
    }
}


///
/// CharacterState_Pushed
///

/*const float CharacterState_Pushed::DEFAULT_PUSH_TIME = .5f;

CharacterState_Pushed::CharacterState_Pushed(Character *character) :
    CharacterState(CharacterStateType_Pushed,character)
{

}

CharacterState_Pushed::~CharacterState_Pushed()
{

}

void CharacterState_Pushed::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    auto elapsedPushTime = std::min(elapsedTime,m_pushTimer.remainingTime());
    glm::vec2 charMove = m_pushDirection*elapsedPushTime.count();
    m_character->move(charMove);

    CharacterStateType_Interrupted::update(elapsedTime, localTime);
}

void CharacterState_Interrupted::entered(CharacterInput *input)
{
    CharacterState_Interrupted::entered(input);

    m_pushDirection = input->getPushedInputs().second;
    m_pushTimer.reset(DEFAULT_PUSH_TIME);
}*/


///
/// CharacterState_Dead
///

CharacterState_Dead::CharacterState_Dead(Character *character) :
    CharacterState(CharacterStateType_Dead,character)
{

}

CharacterState_Dead::~CharacterState_Dead()
{

}

void CharacterState_Dead::entered(CharacterInput *input)
{
    m_character->startAnimation("death");
}


