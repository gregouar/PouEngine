#include "character/PlayableCharacter.h"

#include "PouEngine/assets/AssetHandler.h"

const float PlayableCharacter::DEFAULT_COMBATMODE_DELAY = 3.0f;
const float PlayableCharacter::DEFAULT_WANTTOATTACK_DELAY = .1f;
const float PlayableCharacter::DEFAULT_DASH_DELAY = .5f;
const float PlayableCharacter::DEFAULT_DASH_TIME = .15f;
const float PlayableCharacter::DEFAULT_DASH_SPEED = 1500.0f;
const float PlayableCharacter::DEFAULT_WANTTODASH_DELAY = .5f;

PlayableCharacter::PlayableCharacter() : Character()
{
    //m_normalWalkingSpeed = 250.0f;

    //m_isInCombatMode    = false;
    //m_combatModeDelay   = 0.0f;

    //m_wantToAttackDelay = 0.0f;

    m_isLateralWalking  = false;
    m_lookingAt         = glm::vec2(0);

    m_gearsModel.resize(NBR_GEAR_TYPES, nullptr);

    m_lastPlayerUpdateTime = -1;
    //m_isDashing         = false;
    //m_dashDelay         = 0.0f;
}

PlayableCharacter::~PlayableCharacter()
{
    //dtor
}


/*bool PlayableCharacter::loadModel(const std::string &path)
{
    if(!Character::loadModel(path))
        return (false);

    m_normalWalkingSpeed = m_attributes.walkingSpeed;

    return (true);
}*/

bool PlayableCharacter::setModel(CharacterModelAsset *model)
{
    if(!Character::setModel(model))
        return (false);

    m_normalWalkingSpeed = m_attributes.walkingSpeed;
    this->setLastPlayerUpdateTime(m_curLocalTime);

    return (true);
}

bool PlayableCharacter::loadItem(const std::string &path)
{
    ItemModelAsset* itemModel = pou::AssetHandler<ItemModelAsset>::loadAssetFromFile(path);

    if(itemModel == nullptr)
        return (false);

    GearType type = itemModel->getAttributes().type;

    if(type == NBR_GEAR_TYPES)
        return (false);

    if(m_gearsModel[type] != nullptr)
    {
        m_gearsModel[type]->removeFromCharacter(this);
        //Do Something
    }

    itemModel->generateOnCharacter(this);

    m_gearsModel[type] = itemModel;
    this->updateGearsAttributes();

    return (true);
}


void PlayableCharacter::setWalkingSpeed(float speed)
{
    Character::setWalkingSpeed(speed);
    m_normalWalkingSpeed = m_attributes.walkingSpeed;
}


void PlayableCharacter::askToAttack(glm::vec2 direction)
{
    //m_wantToAttackDelay     = DEFAULT_WANTTOATTACK_DELAY;
    m_wantToAttackTimer.reset(DEFAULT_WANTTOATTACK_DELAY);
    m_wantToAttackDirection = direction;
}

bool PlayableCharacter::attack(glm::vec2 direction, const std::string &animationName)
{
    if(m_dashTimer.isActive())
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
    //m_isInCombatMode    = true;
    m_combatModeTimer.reset(DEFAULT_COMBATMODE_DELAY);
    //m_combatModeDelay   = DEFAULT_COMBATMODE_DELAY;
}

void PlayableCharacter::askToDash(glm::vec2 direction)
{
    //m_wantToDashDelay     = DEFAULT_WANTTODASH_DELAY;
    m_wantToDashDirection = direction;
    m_wantToDashTimer.reset(DEFAULT_WANTTODASH_DELAY);
}

bool PlayableCharacter::dash(glm::vec2 direction)
{
    if(m_dashDelayTimer.isActive())
        return (false);

    ///Comment if we want dash to interrupt attack
    if(m_isAttacking)
        return (false);

    //m_isAttacking   = false;
    if(direction == glm::vec2(0))
        m_dashDirection = m_lookingDirection;
    else
        m_dashDirection = direction;
    m_dashDelayTimer.reset(DEFAULT_DASH_DELAY);
    m_dashTimer.reset(DEFAULT_DASH_TIME);

    this->startAnimation("dash",true);

    return (true);
}

void PlayableCharacter::update(const pou::Time &elapsedTime, float localTime)
{
    if(!m_isDead)
    {

    if(m_wantToDashTimer.isActive())
    {
        m_wantToDashTimer.update(elapsedTime);
        if(this->dash(m_wantToDashDirection))
            m_wantToDashTimer.reset(0);
    }
    else if(m_wantToAttackTimer.isActive())
    {
        m_wantToAttackTimer.update(elapsedTime);
        if(this->attack(m_wantToAttackDirection))
            m_wantToAttackTimer.reset(0);
    }

    m_dashDelayTimer.update(elapsedTime);
    if(m_dashTimer.update(elapsedTime))
        this->startAnimation("walk");

    if(m_dashTimer.isActive())
    {
        m_attributes.walkingSpeed = DEFAULT_DASH_SPEED;
        Character::walk(m_dashDirection);
        m_isWalking = true;
    }
    else if(m_dashDelayTimer.isActive())
        m_attributes.walkingSpeed = m_normalWalkingSpeed * .25f;
    else
        m_attributes.walkingSpeed = m_normalWalkingSpeed;

    if(m_combatModeTimer.isActive())
    {
        m_combatModeTimer.update(elapsedTime);

        //if(!m_isAttacking)
            m_lookingDirection = m_lookingAt - SceneNode::getGlobalXYPosition();
        //SceneNode::setRotation({0,0,desiredRot});
        if(m_isWalking && !m_isAttacking)
        {
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

    }

    //for(auto &skeleton : m_skeletons)
      //  std::cout<<skeleton.second->getCurrentAnimationName()<<std::endl;

    Character::update(elapsedTime, localTime);
}


const std::list<Hitbox> *PlayableCharacter::getHitboxes() const
{
    return &m_hitboxes;
}


void PlayableCharacter::updateGearsAttributes()
{
    m_hitboxes.clear();

    if(Character::getModel() != nullptr)
        for(auto box : *Character::getModel()->getHitboxes())
            m_hitboxes.push_back(box);


    for(auto gear : m_gearsModel)
    {
        if(gear != nullptr)
            for(auto box : *gear->getHitboxes())
                m_hitboxes.push_back(box);
    }

    if(m_gearsModel[GearType_Weapon] != nullptr)
    {
        m_attributes.attackDamages  = m_gearsModel[GearType_Weapon]->getAttributes().attackDamages;
        m_attributes.attackDelay    = m_gearsModel[GearType_Weapon]->getAttributes().attackDelay;
    }
}

void PlayableCharacter::setLastPlayerUpdateTime(float time, bool force)
{
    if(force || m_lastPlayerUpdateTime < time)
        m_lastPlayerUpdateTime = time;
}

float PlayableCharacter::getLastPlayerUpdateTime()
{
    return m_lastPlayerUpdateTime;
}
