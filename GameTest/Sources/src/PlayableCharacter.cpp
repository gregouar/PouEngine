#include "PlayableCharacter.h"

const float PlayableCharacter::DEFAULT_COMBATMODE_DELAY = 3.0f;
const float PlayableCharacter::DEFAULT_WANTTOATTACK_DELAY = .1f;
const float PlayableCharacter::DEFAULT_DASH_DELAY = .5f;
const float PlayableCharacter::DEFAULT_DASH_TIME = .15f;
const float PlayableCharacter::DEFAULT_DASH_SPEED = 1500.0f;
const float PlayableCharacter::DEFAULT_WANTTODASH_DELAY = .1f;

PlayableCharacter::PlayableCharacter() : Character()
{
    m_normalWalkingSpeed = 250.0f;

    m_isInCombatMode    = false;
    m_combatModeDelay   = 0.0f;

    m_wantToAttackDelay = 0.0f;

    m_isLateralWalking  = false;
    m_lookingAt         = glm::vec2(0);

    m_isDashing         = false;
    m_dashDelay         = 0.0f;
}

PlayableCharacter::~PlayableCharacter()
{
    //dtor
}


void PlayableCharacter::askToAttack(glm::vec2 direction)
{
    m_wantToAttackDelay     = DEFAULT_WANTTOATTACK_DELAY;
    m_wantToAttackDirection = direction;
}

bool PlayableCharacter::attack(glm::vec2 direction, const std::string &animationName)
{
    if(m_isDashing)
        return (false);

    if(m_isAttacking)
        return (false);

    m_lookingAt = SceneNode::getGlobalXYPosition() + direction;
    this->forceAttackMode();

    return (Character::attack(direction, animationName));
}

void PlayableCharacter::lookAt(glm::vec2 position)
{
    if(!m_isAttacking)
        m_lookingAt = position;
}

void PlayableCharacter::forceAttackMode(bool force)
{
    m_isInCombatMode    = true;
    m_combatModeDelay   = DEFAULT_COMBATMODE_DELAY;
}

void PlayableCharacter::askToDash(glm::vec2 direction)
{
    m_wantToDashDelay     = DEFAULT_WANTTODASH_DELAY;
    m_wantToDashDirection = direction;
}

bool PlayableCharacter::dash(glm::vec2 direction)
{
    if(m_dashDelay > 0)
        return (false);

    ///Comment if we want dash to interrupt attack
    if(m_isAttacking)
        return (false);

    m_isAttacking   = false;
    m_isDashing     = true;
    if(direction == glm::vec2())
        m_dashDirection = m_lookingDirection;
    else
        m_dashDirection = direction;
    m_dashDelay     = DEFAULT_DASH_DELAY;
    m_dashTime      = DEFAULT_DASH_TIME;

    return (true);
}

void PlayableCharacter::update(const pou::Time &elapsedTime)
{
    if(m_wantToDashDelay > 0)
    {
        if(this->dash(m_wantToDashDirection))
            m_wantToDashDelay = 0;
        else
        {
            m_wantToDashDelay -= elapsedTime.count();
            if(m_wantToDashDelay <= 0)
                m_wantToDashDelay = 0;
        }
    }
    else if(m_wantToAttackDelay > 0)
    {
        if(this->attack(m_wantToAttackDirection))
            m_wantToAttackDelay = 0;
        else
        {
            m_wantToAttackDelay -= elapsedTime.count();
            if(m_wantToAttackDelay <= 0)
                m_wantToAttackDelay = 0;
        }
    }

    if(m_dashDelay > 0)
    {
        m_dashDelay -= elapsedTime.count();
        if(m_dashDelay <= 0)
            m_dashDelay = 0;
    }

    if(m_dashTime > 0)
    {
        m_dashTime -= elapsedTime.count();
        if(m_dashTime <= 0)
        {
            m_dashTime = 0;
            m_isDashing = false;
        }
    }

    if(m_isDashing)
    {
        m_walkingSpeed      = DEFAULT_DASH_SPEED;
        /*m_walkingDirection  = m_dashDirection;
        m_isWalking         = true;*/
        Character::walk(m_dashDirection);
    }
    else if(m_dashDelay > 0)
        m_walkingSpeed = m_normalWalkingSpeed * .25f;
    else
        m_walkingSpeed = m_normalWalkingSpeed;

    if(m_isInCombatMode)
    {
        m_combatModeDelay -= elapsedTime.count();

        if(m_combatModeDelay <= 0)
        {
            m_combatModeDelay       = 0;
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


