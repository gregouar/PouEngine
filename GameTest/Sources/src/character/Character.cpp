#include "character/Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "assets/CharacterModelAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/utils/MathTools.h"

//typedef pou::AssetHandler<CharacterModelAsset>     CharacterModelsHandler;


Character::Character() : Character(std::make_shared<CharacterInput> ())
{

}

Character::Character(std::shared_ptr<CharacterInput> characterInput) : SceneNode(-1,nullptr),
    m_input(characterInput),
    m_isDead(false,0),
    m_curAnimation(std::string(), 0)
{
    m_model             = nullptr;

    m_team = 0;

    m_isDestinationSet  = false;
    m_destination       = {0,0};

    m_lastCharacterSyncTime     = -1;
    m_lastCharacterUpdateTime   = -1;
    m_lastModelUpdateTime       = -1;

    m_position.setReconciliationPrecision(glm::vec3(2));
    m_eulerRotations.setReconciliationPrecision(glm::vec3(glm::pi<float>()/10.0f));
    m_scale.setReconciliationPrecision(glm::vec3(1.0f/NODE_SCALE_DECIMALS));

    m_disableInputSync  = false;
    m_disableDeath      = false;

    m_states[CharacterStateType_Standing]       = std::make_unique<CharacterState_Standing>(this);
    m_states[CharacterStateType_Walking]        = std::make_unique<CharacterState_Walking>(this);
    m_states[CharacterStateType_Attacking]      = std::make_unique<CharacterState_Attacking>(this);
    m_states[CharacterStateType_Dashing]        = std::make_unique<CharacterState_Dashing>(this);
    m_states[CharacterStateType_Interrupted]    = std::make_unique<CharacterState_Interrupted>(this);
    //m_states[CharacterStateType_Pushed]         = std::make_unique<CharacterState_Pushed>(this);
    m_states[CharacterStateType_Dead]           = std::make_unique<CharacterState_Dead>(this);

    m_curState = nullptr;
    this->switchState(CharacterStateType_Standing);
}

Character::~Character()
{
    this->cleanup();
}

void Character::cleanup()
{
    m_model = nullptr;
    m_limbs.clear();

    /*for(auto &it : m_states)
        it.release();*/

    m_skeletons.clear();
}

bool Character::loadModel(const std::string &path)
{
    return this->setModel(CharacterModelsHandler::loadAssetFromFile(path));
}

bool Character::setModel(CharacterModelAsset *model)
{
    this->cleanup();
    m_model = model;

    if(m_model == nullptr)
        return (false);

    if(!m_model->generateCharacter(this))
       return (false);

    m_modelAttributes.setValue(m_model->getAttributes());

    auto att = m_attributes.getValue();
    att.life            = m_modelAttributes.getValue().maxLife;
    att.walkingSpeed    = m_modelAttributes.getValue().walkingSpeed;
    m_attributes.setValue(att);

    if(!m_modelAttributes.getValue().immovable)
        this->disableRotationSync();

    m_lastModelUpdateTime = m_curLocalTime;

    this->setLastCharacterUpdateTime(m_curLocalTime);

    return (true);
}

bool Character::addSkeleton(std::unique_ptr<pou::Skeleton> skeleton, const std::string &name)
{
    return (m_skeletons.insert({name,std::move(skeleton)}).second);
}

pou::SceneEntity *Character::addLimb(LimbModel *limbModel)
{
    if(limbModel->spriteModel != nullptr)
    {
        std::unique_ptr<pou::SpriteEntity> limbEntity(new pou::SpriteEntity());

        limbEntity->setSpriteModel(limbModel->spriteModel);
        limbEntity->setOrdering(pou::ORDERED_BY_Z);
        limbEntity->setInheritRotation(true);

        auto limb = m_limbs.insert({limbModel, std::move(limbEntity)});
        return (limb.first->second.get());
    }
    else if(limbModel->mesh != nullptr)
    {
        std::unique_ptr<pou::MeshEntity> limbEntity(new pou::MeshEntity());

        limbEntity->setMesh(limbModel->mesh);
        limbEntity->setShadowCastingType(pou::ShadowCasting_All);

        auto limb = m_limbs.insert({limbModel, std::move(limbEntity)});
        return (limb.first->second.get());
    }
    else if(limbModel->lightModel != nullptr)
    {
        std::unique_ptr<pou::LightEntity> limbEntity(new pou::LightEntity());

        limbEntity->setModel(*limbModel->lightModel);

        auto limb = m_limbs.insert({limbModel, std::move(limbEntity)});
        return (limb.first->second.get());
    }

    return (nullptr);
}

pou::SoundObject *Character::addSound(SoundModel *soundModel)
{
    std::unique_ptr<pou::SoundObject> soundObject(new pou::SoundObject());

    if(soundModel->isEvent)
        soundObject->setSoundEventModel(soundModel->path);

    auto sound = m_sounds.insert({soundModel, std::move(soundObject)});
    auto *soundPtr = sound.first->second.get();

    //this->attachObject(soundPtr);
    //m_soundsMap.insert({soundModel->tag, soundPtr});
    return (soundPtr);
}

bool Character::addLimbToSkeleton(LimbModel *limbModel, const std::string &skeletonName)
{
    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto *sceneEntity = this->addLimb(limbModel);
    skeleton->second->attachLimb(limbModel->node,
                                 limbModel->state,
                                 sceneEntity);

    return (true);
}

bool Character::removeLimbFromSkeleton(LimbModel *limbModel, const std::string &skeletonName)
{
    if(limbModel == nullptr)
        return (false);

    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto limb = m_limbs.find(limbModel);

    if(limb == m_limbs.end())
        return (false);

    skeleton->second->detachLimb(limbModel->node,
                                limbModel->state,
                                limb->second.get());
    m_limbs.erase(limb);

    return (true);
}

bool Character::addSoundToSkeleton(SoundModel *soundModel, const std::string &skeletonName)
{
    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto *soundObject = this->addSound(soundModel);
    skeleton->second->attachSound(soundObject,soundModel->name);

    return (true);
}

bool Character::removeSoundFromSkeleton(SoundModel *soundModel, const std::string &skeletonName)
{
    if(soundModel == nullptr)
        return (false);

    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto sound = m_sounds.find(soundModel);

    if(sound == m_sounds.end())
        return (false);

    skeleton->second->detachSound(sound->second.get(),soundModel->name);
    m_sounds.erase(sound);

    return (true);
}

void Character::setTeam(int team)
{
    m_team = team;
}

bool Character::damage(float damages, glm::vec2 direction)
{
    if(damages == 0)
        return (false);

    bool isFatal = false;

    auto att = m_attributes.getValue();
    att.life -= damages;
//    std::cout<<m_attributes.life<<"/"<<m_attributes.maxLife<<std::endl;
    m_attributes.setValue(att);

    if(att.life <= 0 && !m_disableDeath)
        isFatal = this->kill(damages);
    else
    {
        if(!m_modelAttributes.getValue().immovable)
        if(direction != glm::vec2(0) && damages >= m_modelAttributes.getValue().maxLife*.25)
            m_input->setPush(true, glm::normalize(direction)*200.0f);

        //Do something to compute interrupt amount ?
        this->interrupt(damages);
    }

    //this->m_lastAttributesUpdateTime = m_curLocalTime;
    this->setLastCharacterUpdateTime(m_curLocalTime);

    return isFatal;
}

void Character::interrupt(float amount)
{
    ///Could be flagged in inputComponent and then used in state or something so that we do not interrupt when dead
    this->switchState(CharacterStateType_Interrupted);
}

bool Character::kill(float amount)
{
    auto att = m_attributes.getValue();
    att.life  = 0;
    m_attributes.setValue(att);

    m_isDead.setValue(true);

    this->switchState(CharacterStateType_Dead);

    return 1;
}

bool Character::resurrect()
{
    if(this->isAlive())
        return (false);

    auto att = m_attributes.getValue();
    att.life = m_modelAttributes.getValue().maxLife;
    m_attributes.setValue(att);
    m_isDead.setValue(false);

    this->switchState(CharacterStateType_Standing);

    //this->m_lastAttributesUpdateTime = m_curLocalTime;
    this->setLastCharacterUpdateTime(m_curLocalTime);

    return (true);
}

/** bool Character::walkToDestination(const pou::Time& elapsedTime)
{
    glm::vec2 gpos  = SceneNode::getGlobalXYPosition();
    glm::vec2 delta = m_destination - gpos;
    float ndelta    = glm::dot(delta,delta);
    float walkingAmount = m_attributes.getValue().walkingSpeed*elapsedTime.count();

    if(ndelta <= walkingAmount*walkingAmount)
    {
        SceneNode::setGlobalPosition(m_destination);
        m_isDestinationSet = false;
        ///m_walkingDirection.setValue(glm::vec2(0));
        this->walk(glm::vec2(0));
        return (false);
    }

    if(m_rotationRadius == 0)
    {
        this->walk(delta);
    } else {
        glm::vec2 coLookinDirection = {m_lookingDirection.getValue().y,
                                      -m_lookingDirection.getValue().x};

        glm::vec2 lDiskPos = gpos + m_rotationRadius * coLookinDirection;
        glm::vec2 rDiskPos = gpos - m_rotationRadius * coLookinDirection;

        glm::vec2 deltaLDisk = m_destination - lDiskPos;
        glm::vec2 deltaRDisk = m_destination - rDiskPos;

        float nDeltaLDisk = glm::dot(deltaLDisk,deltaLDisk);
        float nDeltaRDisk = glm::dot(deltaRDisk,deltaRDisk);

        if(nDeltaLDisk < m_rotationRadius*m_rotationRadius)
        {
            this->rotateToDestination(elapsedTime,
                                      gpos + m_rotationRadius * coLookinDirection * 2.1f,
                                      m_rotationRadius * 2.1f);
        } else if(nDeltaRDisk < m_rotationRadius*m_rotationRadius) {
            this->rotateToDestination(elapsedTime,
                                      gpos - m_rotationRadius * coLookinDirection * 2.1f,
                                      m_rotationRadius * 2.1f);
        } else {
            this->rotateToDestination(elapsedTime, m_destination, m_rotationRadius);
        }
    }

    return (true);
}

void Character::rotateToDestination(const pou::Time& elapsedTime, glm::vec2 destination, float rotationRadius)
{
    glm::vec2 delta = destination - SceneNode::getGlobalXYPosition();
    glm::vec2 normalizedDelta = glm::normalize(delta);

    glm::vec2 coLookinDirection = {m_lookingDirection.getValue().y,
                                  -m_lookingDirection.getValue().x};

    if(glm::dot(normalizedDelta - m_lookingDirection.getValue(),
                normalizedDelta - m_lookingDirection.getValue()) <= 0.0005f)
    {
        this->walk(normalizedDelta);
        ///m_walkingDirection.setValue(normalizedDelta);
        ///this->setLookingDirection(normalizedDelta);
    } else {
        float arcLength = m_attributes.getValue().walkingSpeed*elapsedTime.count();
        float normalizedArcLength = arcLength/rotationRadius;

        glm::vec2 relDest = {glm::dot(delta, coLookinDirection),
                             glm::dot(delta, m_lookingDirection.getValue())};

        if(relDest.x < 0)
            coLookinDirection = -coLookinDirection;

        float normalizedDeltaArc = glm::atan(relDest.x, relDest.y);
        if(normalizedDeltaArc <= 0)
            normalizedDeltaArc += 2*glm::pi<float>();

        if(normalizedDeltaArc <= normalizedArcLength)
            normalizedArcLength = normalizedDeltaArc;

        float cosArcLength = glm::cos(normalizedArcLength);
        float sinArcLength = glm::sin(normalizedArcLength);

        glm::vec2 normalizedRelMove{1.0f-cosArcLength,
                                    sinArcLength};
        glm::vec2 relMove = rotationRadius * normalizedRelMove;

        SceneNode::move(relMove.x * coLookinDirection + relMove.y * m_lookingDirection.getValue());

        glm::vec2 relNewLookingDir{sinArcLength, cosArcLength};
        this->setLookingDirection(relNewLookingDir.x * coLookinDirection
                                + relNewLookingDir.y * m_lookingDirection.getValue());
        ///m_walkingDirection.setValue(glm::vec2(0));
        this->walk(glm::vec2(0));
    }
}**/

void Character::update(const pou::Time& elapsedTime, uint32_t localTime)
{
    SceneNode::update(elapsedTime, localTime);

    m_input->update(elapsedTime, localTime);
    m_curState->handleInput(m_input.get());
    m_curState->update(elapsedTime, localTime);


    m_modelAttributes.update(elapsedTime, m_curLocalTime);
    float oldLife = m_attributes.getValue().life;
    m_attributes.update(elapsedTime, m_curLocalTime);
    if(m_attributes.getValue().life < oldLife)
    {
        if(m_attributes.getValue().life <= 0)
            this->kill(oldLife-m_attributes.getValue().life);
        else
            this->interrupt(oldLife-m_attributes.getValue().life);
    }
    else if (m_attributes.getValue().life > oldLife)
    {
        //if(m_attributes.getValue().life > 0)
        if(m_isDead.getValue() && m_attributes.getValue().life > 0)
        {
            this->resurrect();
            //this->startAnimation("stand");
        }
    }

    if(!m_isDead.getValue() && m_modelAttributes.getValue().maxLife != 0
       && m_attributes.getValue().life <= 0)
        this->kill();

      /**bool wasDead = m_isDead.getValue();
    m_isDead.update(elapsedTime, m_curLocalTime);
    if(!wasDead && m_isDead.getValue())
    {
        m_isDead.setValue(false);
        this->kill();
    } else if(wasDead && !m_isDead.getValue())
    {
        //std::cout<<"RESUR"<<std::endl;
        //m_isDead.setValue(true);
        //this->resurrect();
        this->startAnimation("stand");
    }**/

    /**if(m_curAnimation.update(elapsedTime, m_curLocalTime))
    {
        std::cout<<"Start sync animation:"<<m_curAnimation.getValue()<<std::endl;
        this->startAnimation(m_curAnimation.getValue(), true);
    }**/
}

void Character::rewind(uint32_t time)
{
    SceneNode::rewind(time);

  /*  auto oldWalkingDirection = m_walkingDirection.getValue();
    m_walkingDirection.rewind(time);
    if(m_walkingDirection.getValue() != oldWalkingDirection)
        this->walk(m_walkingDirection.getValue());                   //So that we update walk virtual method of Player

    m_lookingDirection.rewind(time);
    m_curAnimation.rewind(time);

    //m_attackDelayTimer.rewind(time);
    m_pushTimer.rewind(time);
    m_interruptTimer.rewind(time);
    //m_isAttacking.rewind(time);
    m_isDead.rewind(time);

    m_attributes.rewind(time);
    m_modelAttributes.rewind(time);
    //m_alreadyHitCharacters.rewind(time);*/
}

void Character::startAnimation(const std::string &name, bool forceStart)
{
    for(auto &skeleton : m_skeletons)
        skeleton.second->startAnimation(name, forceStart);

    this->setLastCharacterUpdateTime(m_curLocalTime);
    m_curAnimation.setValue(name);
}

void Character::addToNearbyCharacters(Character *character)
{
    m_nearbyCharacters.insert(character);
}

void Character::removeFromNearbyCharacters(Character *character)
{
    m_nearbyCharacters.erase(character);
}

std::set<Character*> &Character::getNearbyCharacters()
{
    return m_nearbyCharacters;
}

bool Character::isAlive() const
{
    return (!m_isDead.getValue());
}

const std::list<Hitbox> *Character::getHitboxes() const
{
    if(m_model == nullptr)
        return (nullptr);
    return m_model->getHitboxes();
}

const std::list<Hitbox> *Character::getHurtboxes() const
{
    if(m_model == nullptr)
        return (nullptr);
    return m_model->getHurtboxes();
}

CharacterModelAsset *Character::getModel() const
{
    return m_model;
}

pou::Skeleton *Character::getSkeleton(const std::string &skeletonName)
{
    auto skeletonIt = m_skeletons.find(skeletonName);
    if(skeletonIt == m_skeletons.end())
        return (nullptr);
    return skeletonIt->second.get();
}

const CharacterAttributes &Character::getAttributes() const
{
    return m_attributes.getValue();
}

const CharacterModelAttributes &Character::getModelAttributes() const
{
    return m_modelAttributes.getValue();
}

int Character::getTeam() const
{
    return m_team;
}


void Character::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    SceneNode::setReconciliationDelay(serverDelay, clientDelay);
    m_input->setReconciliationDelay(serverDelay, clientDelay);
    m_curAnimation.setReconciliationDelay(serverDelay, clientDelay);
    m_attributes.setReconciliationDelay(serverDelay, clientDelay);
}

void Character::setLastCharacterUpdateTime(uint32_t time, bool force)
{
    if(force || m_lastCharacterUpdateTime < time || m_lastCharacterUpdateTime == (uint32_t)(-1))
        m_lastCharacterUpdateTime = time;
}

uint32_t Character::getLastCharacterUpdateTime()
{
    auto lastUpdate = m_lastCharacterUpdateTime;

    lastUpdate = std::max(m_input->getLastUpdateTime(), lastUpdate);

    return lastUpdate;
}

uint32_t Character::getLastModelUpdateTime()
{
    return m_lastModelUpdateTime;
}


void Character::disableDeath(bool disable)
{
    m_disableDeath =  disable;
}


void Character::disableInputSync(bool disable)
{
    m_disableInputSync = disable;
}


void Character::serializeCharacter(pou::Stream *stream, uint32_t clientTime)
{
    bool updateModelAttributes = false;
    if(!stream->isReading() && uint32less(clientTime,m_modelAttributes.getLastUpdateTime()))
        updateModelAttributes = true;
    stream->serializeBool(updateModelAttributes);
    if(updateModelAttributes)
    {
        auto att = m_modelAttributes.getValue();

        stream->serializeFloat(att.walkingSpeed);
        stream->serializeFloat(att.attackDelay);
        stream->serializeFloat(att.maxLife);
        stream->serializeFloat(att.attackDamages);
        stream->serializeBool(att.immovable);

        if(stream->isReading())
        {
            m_modelAttributes.setValue(att, true);
            this->setLastCharacterUpdateTime(m_modelAttributes.getLastUpdateTime());
        }
    }

    bool updateAttributes = false;
    if(!stream->isReading() && uint32less(clientTime,m_attributes.getLastUpdateTime()))
        updateAttributes = true;
    stream->serializeBool(updateAttributes);
    if(updateAttributes)
    {
        auto att = m_attributes.getValue();

        stream->serializeFloat(att.walkingSpeed);
        stream->serializeFloat(att.life/*, 0, (int)m_modelAttributes.getValue().maxLife+1, 2*/);

        if(stream->isReading())
        {
            m_attributes.setValue(att, true);
            this->setLastCharacterUpdateTime(m_attributes.getLastUpdateTime());
        }
    }

    m_input->serialize(stream, clientTime);

   /**bool updateAnimation = false;
    if(!stream->isReading() && uint32less(clientTime,m_curAnimation.getLastUpdateTime()))
        updateAnimation = true;
    stream->serializeBool(updateAnimation);
    if(updateAnimation)
    {
        std::string curAnimation = m_curAnimation.getValue(true);
        stream->serializeString(curAnimation);

        //std::cout<<"Serialize:"<<curAnimation<<std::endl;

        if(stream->isReading())
        {
            m_curAnimation.setValue(curAnimation, true);
            this->setLastCharacterUpdateTime(m_curAnimation.getLastUpdateTime());
        }
    }**/

}

bool Character::syncFromCharacter(Character *srcCharacter)
{
    ///if(m_lastCharacterSyncTime > srcCharacter->getLastCharacterUpdateTime() && m_lastCharacterSyncTime != (uint32_t)(-1))
       /// return (false);

    m_modelAttributes.syncFrom(srcCharacter->m_modelAttributes);
    m_attributes.syncFrom(srcCharacter->m_attributes);
    if(!m_disableInputSync)
        m_input->syncFrom(srcCharacter->m_input.get());

    ///m_curAnimation.syncFrom(srcCharacter->m_curAnimation);

    m_lastCharacterSyncTime = srcCharacter->m_curLocalTime;

    return (true);
}


void Character::switchState(CharacterStateTypes stateType)
{
    if(stateType == NBR_CharacterStateTypes)
        return;

    if(m_curState == m_states[stateType].get())
        return;

    if(m_curState)
        m_curState->leaving(m_input.get());
    m_curState = m_states[stateType].get();
    m_curState->entered(m_input.get());
    //m_curState->handleInput(m_input.get());
}


