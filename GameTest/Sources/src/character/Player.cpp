#include "character/Player.h"

#include "PouEngine/assets/AssetHandler.h"

const float Player::DEFAULT_DASH_DELAY = .5f;
const float Player::DEFAULT_DASH_TIME = .15f;
const float Player::DEFAULT_DASH_SPEED = 1500.0f;
Player::Player(bool userControlled) :
    Character(userControlled ? std::make_shared<PlayerInput>() : std::make_shared<CharacterInput>()),
    m_userControlled(userControlled)
{
    m_gearsModel.resize(NBR_GEAR_TYPES, nullptr);

    m_lastPlayerUpdateTime  = -1;
    m_lastPlayerSyncTime    = -1;
    m_lastGearUpdateTime    = -1;
    m_lastInventoryUpdateTime = -1;

    m_position.setSyncPrecision(glm::vec3(32));
    m_eulerRotations.setSyncPrecision(glm::vec3(glm::pi<float>()/10.0f));
    m_scale.setSyncPrecision(glm::vec3(1.0f/NODE_SCALE_DECIMALS));
}

Player::~Player()
{
    //dtor
}


/*bool Player::loadModel(const std::string &path)
{
    if(!Character::loadModel(path))
        return (false);

    m_normalWalkingSpeed = m_attributes.walkingSpeed;

    return (true);
}*/

bool Player::setModel(CharacterModelAsset *model)
{
    if(!Character::setModel(model))
        return (false);

    this->setLastPlayerUpdateTime(m_curLocalTime);

    return (true);
}

/*bool Player::loadItem(const std::string &path)
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
}*/

ItemModelAsset* Player::removeGear(GearType type)
{
    if(type == NBR_GEAR_TYPES)
        return (nullptr);

    if(m_gearsModel[type] != nullptr)
        m_gearsModel[type]->removeFromCharacter(this);

    auto oldGear = m_gearsModel[type];
    m_gearsModel[type] = nullptr;

    this->updateGearsAttributes();

    return (oldGear);
}

ItemModelAsset* Player::useGear(ItemModelAsset *itemModel)
{
    if(!itemModel)
        return (nullptr);

    GearType type = itemModel->getAttributes().type;

    auto *oldGear= this->removeGear(type);

    itemModel->generateOnCharacter(this);

    m_gearsModel[type] = itemModel;
    this->updateGearsAttributes();

    m_lastGearUpdateTime = m_curLocalTime;
    this->setLastPlayerUpdateTime(m_curLocalTime);

    return (oldGear);
}

ItemModelAsset* Player::useGear(size_t itemNbr)
{
    if(itemNbr >= m_inventory.size())
        return (nullptr);

    return this->useGear(m_inventory[itemNbr]);
}

ItemModelAsset* Player::addItemToInventory(ItemModelAsset* newItem, size_t itemNbr)
{
    if(itemNbr >= m_inventory.size())
        m_inventory.resize(itemNbr+1, nullptr);
    auto oldItem = m_inventory[itemNbr];
    m_inventory[itemNbr] = newItem;

    m_lastInventoryUpdateTime = m_curLocalTime;
    this->setLastPlayerUpdateTime(m_curLocalTime);

    return oldItem;
}

ItemModelAsset* Player::getItemFromInventory(size_t itemNbr)
{
    if(itemNbr >= m_inventory.size())
        return (nullptr);
    return m_inventory[itemNbr];
}

ItemModelAsset* Player::removeItemFromInventory(size_t itemNbr)
{
    if(itemNbr >= m_inventory.size())
        return (nullptr);
    auto oldItem = m_inventory[itemNbr];
    m_inventory[itemNbr] = nullptr;
    return oldItem;
}

void Player::processAction(const PlayerAction &playerAction)
{
    if(playerAction.actionType == PlayerActionType_UseItem)
    {
        this->useGear(playerAction.value);
        return;
    }

    if(!m_userControlled)
        return;

    auto playerInput = std::dynamic_pointer_cast<PlayerInput>(m_input);
    playerInput->processAction(playerAction);
}

void Player::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    //std::cout<<"PosX:"<<m_position.getValue().x<<std::endl;

    Character::update(elapsedTime, localTime);

    /*m_dashDirection.update(elapsedTime, localTime);
    m_wantToDashDirection.update(elapsedTime, localTime);
    m_wantToAttackDirection.update(elapsedTime, localTime);

    if(m_timeShift.update(elapsedTime, localTime))
        this->setSyncDelay(m_timeShift.getValue());

    if(m_wantToWalkDirection != m_walkingDirection.getValue())
        this->walk(m_wantToWalkDirection);


    if(!m_isDead.getValue())
    {

    if(m_wantToDashTimer.isActive()) //=>This should maybe go to inputComponent
    {
        if(this->dash(m_wantToDashDirection.getValue()))
            m_wantToDashTimer.reset(0);
    }
    else if(m_wantToAttackTimer.isActive()) //=>This should maybe go to inputComponent
    {
        if(this->attack(m_wantToAttackDirection.getValue()))
            m_wantToAttackTimer.reset(0);
    }
    else if(m_wantToWalkDirection != m_walkingDirection.getValue()) //=>This should maybe go to inputComponent
    {
        this->walk(m_wantToWalkDirection);
    }

    m_wantToDashTimer.update(elapsedTime,localTime);
    m_wantToAttackTimer.update(elapsedTime,localTime);*/

    /*m_dashDelayTimer.update(elapsedTime,localTime);
    if(m_dashTimer.update(elapsedTime,localTime))
        this->startAnimation("walk");

    auto att = m_attributes.getValue();
    if(m_dashTimer.isActive())
    {
        att.walkingSpeed = DEFAULT_DASH_SPEED;
        Character::walk(m_dashDirection.getValue());
        m_isWalking = true;
    }
    else if(m_dashDelayTimer.isActive())
    {
        //att.walkingSpeed = m_modelAttributes.getValue().walkingSpeed * .25f;
        Character::walk(glm::vec2(0));
    }
    else
        att.walkingSpeed = m_modelAttributes.getValue().walkingSpeed;
    m_attributes.setValue(att);

    if(m_combatModeTimer.isActive())
    {
        //if(!m_isAttacking)
        this->setLookingDirection(m_lookingAt);
            //m_lookingDirection = m_lookingAt - SceneNode::getGlobalXYPosition();
        //SceneNode::setRotation({0,0,desiredRot});
        if(m_isWalking && !m_isAttacking.getValue())
        {
            bool wantToLateralWalk = false;

            if(m_walkingDirection.getValue() != glm::vec2(0))
            {
                float deltaRotation = abs(Character::computeWantedRotation(SceneNode::getEulerRotation().z,
                                           m_walkingDirection.getValue())-SceneNode::getEulerRotation().z);

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
    }*/

    /*m_combatModeTimer.update(elapsedTime,localTime); //=>This should maybe go to inputComponent
    }*/
}

const std::list<Hitbox> *Player::getHitboxes() const
{
    return &m_hitboxes;
}

ItemModelAsset *Player::getItemModel(GearType type)
{
    if(type == NBR_GEAR_TYPES)
        return (nullptr);
    return m_gearsModel[type];
}

size_t Player::getInventorySize() const
{
    return m_inventory.size();
}

void Player::updateGearsAttributes()
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
        auto att = m_modelAttributes.getValue();
        att.attackDamages  = m_gearsModel[GearType_Weapon]->getAttributes().attackDamages;
        att.attackDelay    = m_gearsModel[GearType_Weapon]->getAttributes().attackDelay;
        m_modelAttributes.setValue(att);
    }
}

void Player::serializePlayer(pou::Stream *stream, uint32_t clientTime)
{
    {
        auto timeShift = m_timeShift.getValue(true);
        stream->serializeBits(timeShift,12);
        if(stream->isReading())
            m_timeShift.setValue(timeShift, true);
    }
}

bool Player::syncFromPlayer(Player *srcPlayer)
{
    ///if(uint32less(srcPlayer->getLastPlayerUpdateTime(),m_lastPlayerSyncTime))
       /// return (false);

    m_timeShift.syncFrom(srcPlayer->m_timeShift);

    m_lastPlayerSyncTime = srcPlayer->m_curLocalTime;

    return (true);
}

void Player::setTimeShift(int shift)
{
    m_timeShift.setValue(shift);
}

void Player::setLastPlayerUpdateTime(uint32_t time, bool force)
{
    if(force || m_lastPlayerUpdateTime == (uint32_t)(-1) || m_lastPlayerUpdateTime < time)
        m_lastPlayerUpdateTime = time;
}

uint32_t Player::getLastPlayerUpdateTime()
{
    return m_lastPlayerUpdateTime + m_syncDelay;
}

uint32_t Player::getLastGearUpdateTime(bool useSyncDelay)
{
    if(useSyncDelay)
        return m_lastGearUpdateTime + m_syncDelay;
    return m_lastGearUpdateTime;
}

uint32_t Player::getLastInventoryUpdateTime(bool useSyncDelay)
{
    if(useSyncDelay)
        return m_lastInventoryUpdateTime + m_syncDelay;
    return m_lastInventoryUpdateTime;
}

