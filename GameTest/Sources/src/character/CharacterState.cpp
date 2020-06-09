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

void CharacterState::entered()
{

}

void CharacterState::leaving()
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

void CharacterState_Standing::entered()
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

void CharacterState_Walking::entered()
{
    m_character->startAnimation("walk");
    m_isLateralWalking = false;
}

void CharacterState_Walking::leaving()
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
                                      enemy->getGlobalXYPosition()-m_character->getGlobalXYPosition());
                    }
                }

            }
        }
    }
}

void CharacterState_Attacking::entered()
{
    m_character->startAnimation("attack");
    m_attackTimer.reset(m_character->getModelAttributes().attackDelay);
    m_alreadyHitCharacters.clear();
}


///
/// CharacterState_Interrupted
///

CharacterState_Interrupted::CharacterState_Interrupted(Character *character) :
    CharacterState(CharacterStateType_Interrupted,character)
{

}

CharacterState_Interrupted::~CharacterState_Interrupted()
{

}

void CharacterState_Interrupted::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    if(m_interruptTimer.update(elapsedTime) || !m_interruptTimer.isActive())
    {
        //popState ?
        this->switchState(CharacterStateType_Standing);
    }
}

void CharacterState_Interrupted::entered()
{
    m_character->startAnimation("interrupt");
    m_interruptTimer.reset(Character::DEFAULT_INTERRUPT_DELAY); ///Could be computed from something or something
}



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

void CharacterState_Dead::entered()
{
    m_character->startAnimation("death");
}


