#include "PlayableCharacter.h"

const float PlayableCharacter::DEFAULT_COMBAT_DELAY = 3.0;

PlayableCharacter::PlayableCharacter() : Character()
{
    m_isInCombatMode    = false;
    m_combatDelay       = 0.0;

    m_isLateralWalking  = false;
    m_lookingAt         = glm::vec2(0);
}

PlayableCharacter::~PlayableCharacter()
{
    //dtor
}

void PlayableCharacter::attack(glm::vec2 direction, const std::string &animationName)
{
    if(!m_isAttacking)
        m_lookingAt = SceneNode::getGlobalXYPosition() + direction;

    Character::attack(direction, animationName);
    this->forceAttackMode();
}

void PlayableCharacter::lookAt(glm::vec2 position)
{
    if(!m_isAttacking)
        m_lookingAt = position;
}

void PlayableCharacter::forceAttackMode(bool force)
{
    m_isInCombatMode    = true;
    m_combatDelay       = DEFAULT_COMBAT_DELAY;
}

void PlayableCharacter::update(const pou::Time &elapsedTime)
{
    if(m_isInCombatMode)
    {
        m_combatDelay -= elapsedTime.count();

        if(m_combatDelay <= 0)
        {
            m_combatDelay       = 0;
            m_isInCombatMode    = false;
        }

        //if(!m_isAttacking)
            m_lookingDirection = m_lookingAt - SceneNode::getGlobalXYPosition();
        //SceneNode::setRotation({0,0,desiredRot});
        if(m_isWalking && !m_isAttacking)
        {
            //0 = up, from -PI to PI
            //std::cout<<SceneNode::getEulerRotation().z<<std::endl;

            bool wantToLateralWalk = false;

            if(m_walkingDirection != glm::vec2(0))
            {
                float deltaRotation = abs(Character::computeWantedRotation(SceneNode::getEulerRotation().z,
                                           m_walkingDirection)-SceneNode::getEulerRotation().z);

                if(deltaRotation > glm::pi<float>()*.25 && deltaRotation <  glm::pi<float>()*.75)
                    wantToLateralWalk = true;
            }

            if(wantToLateralWalk && !m_isLateralWalking)
            {
                Character::startAnimation("lateralWalk", true);
                m_isLateralWalking = true;
            }
            else if(!wantToLateralWalk && m_isLateralWalking)
            {
                Character::startAnimation("walk", true);
                m_isLateralWalking = false;
            }
        }
    }
    else if(m_isWalking && m_isLateralWalking)
    {
        m_isLateralWalking = false;
        Character::startAnimation("walk", true);
    }

    Character::update(elapsedTime);
}


