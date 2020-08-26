#include "character/Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/tools/MathTools.h"


#include "assets/CharacterModelAsset.h"
#include "ai/AiComponent.h"
#include "logic/GameMessageTypes.h"
#include "world/GameWorld.h"

//typedef pou::AssetHandler<CharacterModelAsset>     CharacterModelsHandler;

///
///SyncCharacterAttributes
///


SyncCharacterAttributes::SyncCharacterAttributes() : SyncCharacterAttributes(CharacterAttributes())
{
    //ctor
}

SyncCharacterAttributes::SyncCharacterAttributes(const CharacterAttributes &v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1)
{
    this->useUpdateBit();
}

SyncCharacterAttributes::~SyncCharacterAttributes()
{
    //dtor
}

void SyncCharacterAttributes::setValue(const CharacterAttributes &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const CharacterAttributes &SyncCharacterAttributes::getValue() const
{
    return m_attribute.getValue();
}

void SyncCharacterAttributes::serializeImpl(pou::Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    stream->serializeFloat(v.walkingSpeed);
    stream->serializeFloat(v.life);

    if(stream->isReading())
        m_attribute.setValue(v, true);
}


///
///SyncCharacterModelAttributes
///


SyncCharacterModelAttributes::SyncCharacterModelAttributes() : SyncCharacterModelAttributes(CharacterModelAttributes())
{
    //ctor
}

SyncCharacterModelAttributes::SyncCharacterModelAttributes(const CharacterModelAttributes &v) : AbstractSyncElement(&m_attribute),
    m_attribute(v,-1)
{
    this->useUpdateBit();
}

SyncCharacterModelAttributes::~SyncCharacterModelAttributes()
{
    //dtor
}

void SyncCharacterModelAttributes::setValue(const CharacterModelAttributes &v)
{
    if(m_attribute.setValue(v))
        this->updateLastUpdateTime();
}

const CharacterModelAttributes &SyncCharacterModelAttributes::getValue() const
{
    return m_attribute.getValue();
}

void SyncCharacterModelAttributes::serializeImpl(pou::Stream *stream, uint32_t clientTime)
{
    auto v = m_attribute.getValue();

    stream->serializeFloat(v.walkingSpeed);
    stream->serializeFloat(v.attackDelay);
    stream->serializeFloat(v.maxLife);
    stream->serializeFloat(v.attackDamages);
    stream->serializeBool(v.immovable);

    if(stream->isReading())
        m_attribute.setValue(v, true);
}

///
///CharacterSkeleton
///

///const float CharacterSkeleton::DEFAULT_HURTCOLOR_DELAY = .02f;
///const float CharacterSkeleton::DEFAULT_HURTCOLOR_FADEOUTSPEED = 10.0f;

CharacterSkeleton::CharacterSkeleton(pou::SkeletonModelAsset *model) : pou::Skeleton(model)//,
  /**  m_hurtColorAmount(0),
    m_hurtColor(0)**/
{

}

CharacterSkeleton::~CharacterSkeleton()
{

}

void CharacterSkeleton::update(const pou::Time &elapsedTime, uint32_t localTime)
{
    /**m_hurtColorTimer.update(elapsedTime);

    if(m_hurtColorAmount > 0 && !m_hurtColorTimer.isActive())
    {
        float delta = elapsedTime.count() * DEFAULT_HURTCOLOR_FADEOUTSPEED;
        if(delta >= m_hurtColorAmount)
            delta = m_hurtColorAmount;

        this->colorize(-m_hurtColor * delta);
        m_hurtColorAmount -= delta;
    }**/

    pou::Skeleton::update(elapsedTime, localTime);
}

/**void CharacterSkeleton::setHurtColor(const glm::vec4 &hurtColor)
{
    this->colorize(-hurtColor * m_hurtColorAmount);

    m_hurtColorAmount = 1.0;
    m_hurtColor = hurtColor;
    m_hurtColorTimer.reset(DEFAULT_HURTCOLOR_DELAY);

    this->colorize(hurtColor);
}**/


///
///HurtNode
///

const float HurtNode::DEFAULT_HURTCOLOR_DELAY = .02f;
const float HurtNode::DEFAULT_HURTCOLOR_FADEOUTSPEED = 10.0f;

HurtNode::HurtNode(pou::SceneNode *node) :
    m_node(node),
    m_hurtColorAmount(0),
    m_hurtColor(0)
{

}

HurtNode::~HurtNode()
{

}

bool HurtNode::update(const pou::Time &elapsedTime)
{
    m_hurtColorTimer.update(elapsedTime);

    if(m_hurtColorAmount > 0 && !m_hurtColorTimer.isActive())
    {
        float delta = elapsedTime.count() * DEFAULT_HURTCOLOR_FADEOUTSPEED;
        if(delta >= m_hurtColorAmount)
            delta = m_hurtColorAmount;

        m_node->colorize(-m_hurtColor * delta);
        m_hurtColorAmount -= delta;
    }

    return (m_hurtColorAmount <= 0);
}

void HurtNode::setHurtColor(const glm::vec4 &hurtColor)
{
    m_node->colorize(-hurtColor * m_hurtColorAmount);

    m_hurtColorAmount = 1.0;
    m_hurtColor = hurtColor;
    m_hurtColorTimer.reset(DEFAULT_HURTCOLOR_DELAY);

    m_node->colorize(hurtColor);
}


///
///Character
///

Character::Character() : Character(std::make_shared<CharacterInput> ())
{

}

Character::Character(std::shared_ptr<CharacterInput> characterInput) :
    ///m_node(std::make_shared<pou::SceneNode>(nullptr)),
    m_input(characterInput),
    m_isDead(false),
    m_worldSync(nullptr),
    m_characterSyncId(0),
    m_nodeSyncer(nullptr)
    ///m_curAnimation(std::string(), 0)
{
    m_model             = nullptr;

    m_team = 0;

    /*m_isDestinationSet  = false;
    m_destination       = {0,0};*/

    m_lastModelUpdateTime       = -1;

    m_nodeSyncer = std::make_shared<NodeSyncer>(this);
    m_nodeSyncer->setReconciliationPrecision(glm::vec3(4),
                                             glm::vec3(glm::pi<float>()/10.0f),
                                             glm::vec3(1.0f/NodeSyncer::NODE_SCALE_DECIMALS));

   /// m_disableInputSync  = false;
    m_disableDeath      = false;
    m_disableDamageDealing = false;
    m_disableDamageReceiving = false;
    m_disableAI = false;

    m_states[CharacterStateType_Standing]       = std::make_unique<CharacterState_Standing>(this);
    m_states[CharacterStateType_Walking]        = std::make_unique<CharacterState_Walking>(this);
    m_states[CharacterStateType_Attacking]      = std::make_unique<CharacterState_Attacking>(this);
    m_states[CharacterStateType_Dashing]        = std::make_unique<CharacterState_Dashing>(this);
    m_states[CharacterStateType_Interrupted]    = std::make_unique<CharacterState_Interrupted>(this);
    //m_states[CharacterStateType_Pushed]         = std::make_unique<CharacterState_Pushed>(this);
    m_states[CharacterStateType_Dead]           = std::make_unique<CharacterState_Dead>(this);

    m_curState = nullptr;
    m_curStateId.useMinMax(0, NBR_CharacterStateTypes);

    this->setAiComponent(std::make_unique<AiComponent>(this));
    ///m_syncComponent.addSyncSubComponent(m_input->getSyncComponent());

    m_characterSyncComponent.addSyncElement(&m_modelAttributes);
    m_characterSyncComponent.addSyncElement(&m_attributes);
    m_characterSyncComponent.addSyncElement(&m_curStateId);
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
        it.reset();*/

    m_skeletons.clear();
}

/**std::shared_ptr<pou::SceneNode> Character::node()
{
    return m_node;
}**/

bool Character::createFromModel(const std::string &path)
{
    return this->createFromModel(CharacterModelsHandler::loadAssetFromFile(path));
}

bool Character::createFromModel(CharacterModelAsset *model)
{
    if(m_model == model)
        return (false);

    this->cleanup();
    m_model = model;

    if(m_model == nullptr)
        return (false);

    this->detachAllObjects();
    if(!m_model->generateCharacter(this))
       return (false);

    //m_aiComponent = std::make_unique<AiComponent>(this);

    m_modelAttributes.setValue(m_model->getAttributes());

    auto att = m_attributes.getValue();
    att.life            = m_modelAttributes.getValue().maxLife;
    att.walkingSpeed    = m_modelAttributes.getValue().walkingSpeed;
    m_attributes.setValue(att);

    if(!m_modelAttributes.getValue().immovable)
        m_nodeSyncer->disableRotationSync();

    m_characterSyncComponent.updateLastUpdateTime();
    m_lastModelUpdateTime = m_characterSyncComponent.getLocalTime();

    this->switchState(CharacterStateType_Standing);

    return (true);
}

void Character::setAiComponent(std::shared_ptr<AiComponent> aiComponent)
{
    ///if(m_aiComponent)
       /// m_syncComponent.removeSyncSubComponent(m_aiComponent->getSyncComponent());
    m_aiComponent = aiComponent;
   /// m_syncComponent.addSyncSubComponent(m_aiComponent->getSyncComponent());
}

bool Character::addSkeleton(std::shared_ptr<CharacterSkeleton> skeleton, pou::HashedString name)
{
    return (m_skeletons.insert({name,std::move(skeleton)}).second);
}

std::shared_ptr<pou::SceneEntity> Character::addLimb(LimbModel *limbModel)
{
    if(limbModel->spriteModel != nullptr)
    {
        auto limbEntity = std::make_shared<pou::SpriteEntity>();

        limbEntity->setSpriteModel(limbModel->spriteModel);
        limbEntity->setOrdering(pou::ORDERED_BY_Z);
        limbEntity->setInheritRotation(true);

        m_limbs.insert({limbModel, limbEntity});
        return limbEntity;
    }
    else if(limbModel->mesh != nullptr)
    {
        auto limbEntity = std::make_shared<pou::MeshEntity>();

        limbEntity->setMeshModel(limbModel->mesh);
        limbEntity->setShadowCastingType(pou::ShadowCasting_All);

        m_limbs.insert({limbModel, limbEntity});
        return limbEntity;
    }
    else if(limbModel->lightModel != nullptr)
    {
        auto limbEntity = std::make_shared<pou::LightEntity>();

        limbEntity->setModel(*limbModel->lightModel);

        m_limbs.insert({limbModel, limbEntity});
        return limbEntity;
    }

    return (nullptr);
}

std::shared_ptr<pou::SoundObject> Character::addSound(SoundModel *soundModel)
{
    auto soundObject = std::make_shared<pou::SoundObject> ();

    if(soundModel->isEvent())
        soundObject->setSoundEventModel(soundModel->getPath());

    m_sounds.insert({soundModel, soundObject});

    //this->attachObject(soundPtr);
    //m_soundsMap.insert({soundModel->tag, soundPtr});
    return soundObject;
}

bool Character::addLimbToSkeleton(LimbModel *limbModel, pou::HashedString skeletonName)
{
    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto sceneEntity = this->addLimb(limbModel);
    skeleton->second->attachLimb(limbModel->node,
                                 limbModel->state,
                                 sceneEntity);

    return (true);
}

bool Character::removeLimbFromSkeleton(LimbModel *limbModel, pou::HashedString skeletonName)
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

bool Character::addSoundToSkeleton(SoundModel *soundModel, pou::HashedString skeletonName)
{
    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto soundObject = this->addSound(soundModel);
    skeleton->second->attachSound(soundObject,soundModel->getName());

    return (true);
}

bool Character::removeSoundFromSkeleton(SoundModel *soundModel, pou::HashedString skeletonName)
{
    if(soundModel == nullptr)
        return (false);

    auto skeleton = m_skeletons.find(skeletonName);

    if(skeleton == m_skeletons.end())
        return (false);

    auto sound = m_sounds.find(soundModel);

    if(sound == m_sounds.end())
        return (false);

    skeleton->second->detachSound(sound->second.get(),soundModel->getName());
    m_sounds.erase(sound);

    return (true);
}

void Character::setSyncData(GameWorld_Sync *worldSync, int id)
{
    m_worldSync = worldSync;
    Character::m_characterSyncId = id;
}

void Character::setTeam(int team)
{
    m_team = team;
}

bool Character::damage(float damages, glm::vec2 direction, bool onlyCosmetic)
{
    if(damages == 0 || m_disableDamageReceiving)
        return (false);

    bool isFatal = false;

    auto att = m_attributes.getValue();
    if(!onlyCosmetic && !m_disableDamageDealing)
    {
        att.life -= damages;

        GameMessage_World_CharacterDamaged msg;
        msg.character   = this;
        msg.damages     = damages;
        msg.direction   = direction;

        pou::MessageBus::postMessage(GameMessageType_World_CharacterDamaged, &msg);
    }



//    std::cout<<m_attributes.life<<"/"<<m_attributes.maxLife<<std::endl;
    m_attributes.setValue(att);

    if(att.life <= 0 && !m_disableDeath)
        isFatal = this->kill(damages);
    else //if(!onlyCosmetic)
    {
        if(!m_modelAttributes.getValue().immovable)
        if(direction != glm::vec2(0) && damages >= m_modelAttributes.getValue().maxLife*.25)
            m_input->setPush(true, glm::normalize(direction)*200.0f);

        //Do something to compute interrupt amount ?
        this->interrupt(damages);
    }

    return isFatal;
}

void Character::setHurtNodeColor(pou::SceneNode *hurtNode, const glm::vec4 &hurtColor)
{
    auto hurtNodeIt = m_hurtNodes.find(hurtNode);
    if(hurtNodeIt == m_hurtNodes.end())
        hurtNodeIt = m_hurtNodes.insert({hurtNode, HurtNode(hurtNode)}).first;
    hurtNodeIt->second.setHurtColor(hurtColor);
}

/*void Character::setSkeletonHurtColor(pou::Skeleton *skeleton, const glm::vec4 &color)
{
    skeleton->setColor(color);
}*/


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

    this->disableCollisions();

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

    this->disableCollisions(false);

    this->switchState(CharacterStateType_Standing);

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

void Character::generateRenderingData(pou::SceneRenderingInstance *renderingInstance)
{
    pou::SceneNode::generateRenderingData(renderingInstance);

    /*auto &path = m_aiComponent->getExploredNodes();
    for(auto node : path)
    {
        glm::vec4 color(0,1,0,1);
        if(node.depth > 0 && node.depth < 10)
        {
            color.b = 0;
            color.r = (float)node.depth/10.0f;
        }
        else
        {
            color.b = 0;
            color.r = 1.0f;
            color.g = ((float)node.depth-10.0f)/10.0f;
        }

        renderingInstance->drawRectangle({node.position.x-3,node.position.y-3,100-(float)node.depth/100},{6+node.depth/2,6+node.depth/2},color);
    }*/

    /*auto &path = m_aiComponent->getPlannedPath();
    float i = 0;
    for(auto node : path)
    {
        if(i== 0)
            renderingInstance->drawRectangle({node.x-5,node.y-5,100},{10,10},{0,1,0,1});
        else
            renderingInstance->drawRectangle({node.x-5,node.y-5,100},{10,10},{i,i,1,1});
        i+=.1f;
    }*/
    //if(!path.empty())
    //    std::cout<<"Path Length:"<<path.size()<<std::endl;

    //renderingInstance->drawRectangle({this->getGlobalPosition().x-5,this->getGlobalPosition().y-5,101},{10,10},{1,0,0,1});
}

void Character::update(const pou::Time& elapsedTime, uint32_t localTime)
{
    this->updateHurtNodes(elapsedTime);
    this->updateSyncComponent(elapsedTime, localTime);

    pou::SceneNode::update(elapsedTime,localTime);

    GameMessage_World_CharacterUpdated msg;
    msg.character = this;
    pou::MessageBus::postMessage(GameMessageType_World_CharacterUpdated, &msg);
}

void Character::updateSyncComponent(const pou::Time& elapsedTime, uint32_t localTime)
{
    float oldLife = m_attributes.getValue().life;
    int oldState = m_curStateId.getValue();

    m_characterSyncComponent.update(elapsedTime, localTime);
    if(!m_disableAI)
        m_aiComponent->update(elapsedTime, localTime);
    m_input->update(elapsedTime, localTime);

    if(oldState != m_curStateId.getValue())
        this->switchState((CharacterStateTypes)m_curStateId.getValue());

    if(m_curState)
    {
        m_curState->handleInput(m_input.get());
        m_curState->update(elapsedTime, localTime);
    }

    if(m_attributes.getValue().life < oldLife)
    {
        if(m_attributes.getValue().life <= 0)
        {
            this->kill(oldLife-m_attributes.getValue().life);
            std::cout<<"Killed from Sync !"<<std::endl;
        }
        else
            this->interrupt(oldLife-m_attributes.getValue().life);
    }
    else if (m_attributes.getValue().life > oldLife)
    {
        //if(m_attributes.getValue().life > 0)
        if(m_isDead.getValue() && m_attributes.getValue().life > 0)
        {
            this->resurrect();
            std::cout<<"Resurrected from Sync !"<<std::endl;
        }
    }

    if(!m_isDead.getValue() && m_modelAttributes.getValue().maxLife != 0
       && m_attributes.getValue().life <= 0)
        this->kill();
}

void Character::updateHurtNodes(const pou::Time& elapsedTime)
{
    for(auto hurtNodeIt = m_hurtNodes.begin() ; hurtNodeIt != m_hurtNodes.end() ; )
    {
        if(hurtNodeIt->second.update(elapsedTime))
            hurtNodeIt = m_hurtNodes.erase(hurtNodeIt);
        else
            ++hurtNodeIt;
    }
}

/**void Character::rewind(uint32_t time)
{
    SceneNode::rewind(time);
**/
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
/**}**/

void Character::startAnimation(pou::HashedString name, bool forceStart)
{
    for(auto &skeleton : m_skeletons)
        skeleton.second->startAnimation(name, forceStart);

    ///this->setLastCharacterUpdateTime(m_curLocalTime);
    ///m_curAnimation.setValue(name);
}

/**void Character::addToNearbyCharacters(std::shared_ptr<Character> character)
{
    m_nearbyCharacters.insert(character);
}

void Character::removeFromNearbyCharacters(std::shared_ptr<Character> character)
{
    m_nearbyCharacters.erase(character);
}

std::unordered_set<std::shared_ptr<Character> > &Character::getNearbyCharacters()
{
    return m_nearbyCharacters;
}**/

std::vector<Character*> *Character::getNearbyCharacters()
{
    if(!m_worldSync)
        return (nullptr);

    return m_worldSync->getUpdatedCharacters();
}

bool Character::isAlive() const
{
    return (!m_isDead.getValue());
}

const std::vector<Hitbox> *Character::getHitboxes() const
{
    if(m_model == nullptr)
        return (nullptr);
    return m_model->getHitboxes();
}

const std::vector<Hitbox> *Character::getHurtboxes() const
{
    if(m_model == nullptr)
        return (nullptr);
    return m_model->getHurtboxes();
}

CharacterModelAsset *Character::getModel() const
{
    return m_model;
}

CharacterSkeleton *Character::getSkeleton(pou::HashedString skeletonName)
{
    auto skeletonIt = m_skeletons.find(skeletonName);
    if(skeletonIt == m_skeletons.end())
        return (nullptr);
    return skeletonIt->second.get();
}

CharacterInput *Character::getInput()
{
    return m_input.get();
}

AiComponent *Character::getAi()
{
    return m_aiComponent.get();
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

GameWorld_Sync* Character::getWorldSync() const
{
    return m_worldSync;
}

uint32_t Character::getCharacterSyncId() const
{
    return Character::m_characterSyncId;
}

void Character::setReconciliationDelay(uint32_t serverDelay, uint32_t clientDelay)
{
    m_nodeSyncer->setReconciliationDelay(serverDelay, clientDelay);
    m_characterSyncComponent.setReconciliationDelay(serverDelay, clientDelay);

    m_input->getSyncComponent()->setReconciliationDelay(serverDelay, clientDelay);
    if(m_aiComponent)
        m_aiComponent->getSyncComponent()->setReconciliationDelay(serverDelay, clientDelay);
}


uint32_t Character::getLastCharacterUpdateTime()
{
    auto lastUpdate = m_characterSyncComponent.getLastUpdateTime();

    lastUpdate = uint32max(m_input->getSyncComponent()->getLastUpdateTime(), lastUpdate);
    if(m_aiComponent)
        lastUpdate = uint32max(m_aiComponent->getSyncComponent()->getLastUpdateTime(), lastUpdate);

    return lastUpdate;
}

uint32_t Character::getLastModelUpdateTime()
{
    return m_lastModelUpdateTime;
}

std::shared_ptr<NodeSyncer> Character::getNodeSyncer()
{
    return m_nodeSyncer;
}


void Character::disableDeath(bool disable)
{
    m_disableDeath =  disable;
}

void Character::disableSync(bool disable)
{
    m_characterSyncComponent.disableSync(disable);
   // m_input->getSyncComponent()->disableSync(disable);
    m_aiComponent->getSyncComponent()->disableSync(disable);

    m_nodeSyncer->disableSync(disable);
}

void Character::disableInputSync(bool disable)
{
    m_input->getSyncComponent()->disableSync(disable);
}

void Character::disableStateSync(bool disable)
{
    m_curStateId.disableSync(disable);
}

void Character::disableDamageDealing(bool disable)
{
    m_disableDamageDealing = disable;
}

void Character::disableDamageReceiving(bool disable)
{
    m_disableDamageReceiving = disable;
}

void Character::disableAI(bool disable)
{
    m_disableAI = disable;
}

bool Character::areDamagesOnlyCosmetic()
{
    return m_disableDamageDealing;
}

void Character::serializeCharacter(pou::Stream *stream, uint32_t clientTime)
{
    m_characterSyncComponent.serialize(stream, clientTime);
    m_input->getSyncComponent()->serialize(stream, clientTime);
    m_aiComponent->getSyncComponent()->serialize(stream, clientTime);
}

void Character::syncFromCharacter(Character *srcCharacter)
{
    m_characterSyncComponent.syncFrom(srcCharacter->m_characterSyncComponent);

    m_input->getSyncComponent()->syncFrom(*srcCharacter->m_input->getSyncComponent());
    m_aiComponent->getSyncComponent()->syncFrom(*srcCharacter->m_aiComponent->getSyncComponent());
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

    m_curStateId = stateType;
}


