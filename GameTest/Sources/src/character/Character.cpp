#include "character/Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "assets/CharacterModelAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"
#include "PouEngine/scene/MeshEntity.h"
#include "PouEngine/utils/MathTools.h"

//typedef pou::AssetHandler<CharacterModelAsset>     CharacterModelsHandler;

const float Character::DEFAULT_INTERRUPT_DELAY = .5f;

Character::Character() : SceneNode(-1,nullptr)
{
    m_model             = nullptr;

    m_rotationRadius    = 0.0f;

    m_isDestinationSet  = false;
    m_destination       = {0,0};

    m_isDead            = false;
    m_isWalking         = true;
    m_isAttacking       = false;
    m_walkingDirection  = {0,0};
    m_lookingDirection  = {0,-1};

    m_lastCharacterUpdateTime = -1;
    m_lastModelUpdateTime = -1;
    m_lastAttributesUpdateTime = -1;
    m_lastLookingDirectionUpdateTime = -1;
}

Character::~Character()
{
    this->cleanup();
}

void Character::cleanup()
{
    m_model = nullptr;
    m_limbs.clear();
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

    m_attributes = m_model->getAttributes();
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

    //auto *skelPtr = skeleton->second.get();

    auto *soundObject = this->addSound(soundModel);
    skeleton->second->attachSound(soundObject,soundModel->name);
    //skelPtr->attachSound(soundObject,skelPtr->getModel()->getSoundId(soundModel.name));

   // skeleton->attachSound(targetCharacter->addSound(&sound),
   //                       skeletonModel.second.skeleton->getSoundId(sound.name));

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

    //skeleton->second->detachSound(soundModel->node, sound->second.get());
    //skeleton->second->detachObject(sound->second.get());
    skeleton->second->detachSound(sound->second.get(),soundModel->name);
    m_sounds.erase(sound);

    return (true);
}

void Character::setWalkingSpeed(float speed)
{
    if(speed >= 0 && m_attributes.walkingSpeed != speed)
    {
        m_attributes.walkingSpeed = speed;
        m_lastAttributesUpdateTime = m_curLocalTime;
        this->setLastCharacterUpdateTime(m_curLocalTime);
    }
}

void Character::setRotationRadius(float radius)
{
    if(radius >= 0)
        m_rotationRadius = radius;
}

void Character::setLookingDirection(glm::vec2 direction)
{
    direction = glm::normalize(direction);
    if(m_lookingDirection != direction)
    {
        m_lookingDirection = direction;
        m_lastLookingDirectionUpdateTime = m_curLocalTime;
        this->setLastCharacterUpdateTime(m_curLocalTime);
    }
}

void Character::setDestination(glm::vec2 destination)
{
    if(m_destination != destination || !m_isDestinationSet)
    {
        m_destination       = destination;
        m_isDestinationSet  = true;
        //this->setLastCharacterUpdateTime(m_curLocalTime);
    }
}

void Character::walk(glm::vec2 direction)
{
    if(m_walkingDirection != direction)
    {
        m_walkingDirection = direction;
        if(direction != glm::vec2(0))
            this->setLookingDirection(m_walkingDirection);
            //m_lookingDirection = m_walkingDirection;

        //this->setLastCharacterUpdateTime(m_curLocalTime);
    }
}

bool Character::attack(glm::vec2 direction, const std::string &animationName)
{
    if(m_isDead)
        return (false);

    if(m_isAttacking)
        return (false);

    if(m_interruptTimer.isActive())
        return (false);

    if(m_attackDelayTimer.isActive())
        return (false);

    this->startAnimation(animationName,true);
    m_isAttacking = true;
    m_isWalking = false;
    //if(direction != glm::vec2(0))
    this->setLookingDirection(direction);

    m_attackDelayTimer.reset(m_attributes.attackDelay);

    this->setLastCharacterUpdateTime(m_curLocalTime);

    return (true);
}


bool Character::stopAttacking()
{
    bool wasAttacking = m_isAttacking;

    m_isAttacking = false;
    m_alreadyHitCharacters.clear();
    this->startAnimation("stand", true);

    //if(wasAttacking)
      //  this->setLastCharacterUpdateTime(m_curLocalTime);

    return wasAttacking;
}

bool Character::damage(float damages, glm::vec2 direction)
{
    if(damages == 0)
        return (false);

    bool isFatal = false;
    m_attributes.life -= damages;
//    std::cout<<m_attributes.life<<"/"<<m_attributes.maxLife<<std::endl;

    if(m_attributes.life <= 0)
    {
        m_attributes.life  = 0;
        if(!m_isDead)
        {
            this->startAnimation("death",true);
            isFatal = true;
        }
        m_isDead        = true;
        m_isAttacking   = false;
        m_isWalking     = false;
    }
    //Do something to compute interrupt amount ?
    this->interrupt(damages);

    if(!m_attributes.immovable)
    if(direction != glm::vec2(0) && damages >= m_attributes.maxLife*.25)
    {
        m_pushTimer.reset(.1f);
        m_pushVelocity = glm::normalize(direction)*200.0f;
    }

    this->m_lastAttributesUpdateTime = m_curLocalTime;
    this->setLastCharacterUpdateTime(m_curLocalTime);

    return isFatal;
}

bool Character::interrupt(float amount)
{
    bool interrupt = true;

    if(!this->isAlive())
        return (false);

    if(!m_interruptTimer.isActive())
    {
        this->stopAttacking();
        m_isWalking     = false;

        this->startAnimation("interrupt",true);
        m_interruptTimer.reset(DEFAULT_INTERRUPT_DELAY);
        m_attackDelayTimer.reset(0);
        //m_isAttacking   = false;

        //this->setLastCharacterUpdateTime(m_curLocalTime);
    }

    return interrupt;
}

bool Character::resurrect()
{
    if(this->isAlive())
        return (false);

    m_attributes.life = m_attributes.maxLife;
    m_isDead = false;

    this->m_lastAttributesUpdateTime = m_curLocalTime;
    this->setLastCharacterUpdateTime(m_curLocalTime);

    return (true);
}

bool Character::walkToDestination(const pou::Time& elapsedTime)
{
    glm::vec2 gpos  = SceneNode::getGlobalXYPosition();
    glm::vec2 delta = m_destination - gpos;
    float ndelta    = glm::dot(delta,delta);

    if(ndelta <= m_attributes.walkingSpeed*elapsedTime.count()*m_attributes.walkingSpeed*elapsedTime.count())
    {
        SceneNode::setGlobalPosition(m_destination);
        m_isDestinationSet = false;
        m_walkingDirection = glm::vec2(0);
        //this->walk({0,0});
        return (false);
    }

    if(m_rotationRadius == 0)
    {
        this->walk(delta);
    } else {
        glm::vec2 coLookinDirection = {m_lookingDirection.y,
                                      -m_lookingDirection.x};

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

    glm::vec2 coLookinDirection = {m_lookingDirection.y,
                                  -m_lookingDirection.x};

    if(glm::dot(normalizedDelta - m_lookingDirection,
                normalizedDelta - m_lookingDirection) <= 0.0005f)
    {
        m_walkingDirection = normalizedDelta;
        this->setLookingDirection(normalizedDelta);
    } else {
        float arcLength = m_attributes.walkingSpeed*elapsedTime.count();
        float normalizedArcLength = arcLength/rotationRadius;

        glm::vec2 relDest = {glm::dot(delta, coLookinDirection),
                             glm::dot(delta, m_lookingDirection)};

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

        SceneNode::move(relMove.x * coLookinDirection + relMove.y * m_lookingDirection);

        glm::vec2 relNewLookingDir{sinArcLength, cosArcLength};
        this->setLookingDirection(relNewLookingDir.x * coLookinDirection
                                + relNewLookingDir.y * m_lookingDirection);
        m_walkingDirection = glm::vec2(0);
    }
}

float Character::computeWantedRotation(float startingRotation, glm::vec2 position)
{
    float wantedRotation = glm::pi<float>()/2.0+glm::atan(position.y, position.x);

    if(glm::abs(wantedRotation-startingRotation) > glm::abs(wantedRotation-startingRotation+glm::pi<float>()*2.0))
        wantedRotation += glm::pi<float>()*2.0;

    if(glm::abs(wantedRotation-startingRotation) > glm::abs(wantedRotation-startingRotation-glm::pi<float>()*2.0))
        wantedRotation -= glm::pi<float>()*2.0;

    return wantedRotation;
}

void Character::update(const pou::Time& elapsedTime, float localTime)
{
    if(m_pushTimer.isActive())
        this->move(m_pushVelocity*static_cast<float>(elapsedTime.count()));
    m_pushTimer.update(elapsedTime);

    if(!m_isDead)
    {
        m_attackDelayTimer.update(elapsedTime);

        //if(m_interruptTimer.isActive()) {}
        //else
        if(m_isAttacking)
            this->updateAttacking(elapsedTime);
        else
            this->updateWalking(elapsedTime);
        this->updateLookingDirection(elapsedTime);


        if(m_interruptTimer.update(elapsedTime))
            this->startAnimation("stand");
    }

    //m_nearbyCharacters.clear();

    SceneNode::update(elapsedTime, localTime);

    //this->updateSounds();
}

void Character::updateWalking(const pou::Time &elapsedTime)
{
    bool wantToWalk = false;

    if(m_interruptTimer.isActive())
        return;

    if(m_isDestinationSet)
        wantToWalk = this->walkToDestination(elapsedTime);

    if(m_walkingDirection != glm::vec2(0))
    {
        m_walkingDirection = glm::normalize(m_walkingDirection);
        glm::vec2 charMove = {m_walkingDirection.x*m_attributes.walkingSpeed*elapsedTime.count(),
                              m_walkingDirection.y*m_attributes.walkingSpeed*elapsedTime.count()};
        //m_lookingDirection = m_walkingDirection;
        SceneNode::move(charMove);
        wantToWalk = true;
    }

    if(m_attributes.walkingSpeed <= 0)
        wantToWalk = false;

    if(wantToWalk && !m_isWalking)
    {
        this->startAnimation("walk", true);
        m_isWalking = true;
    }
    else if(!wantToWalk && m_isWalking)
    {
        this->startAnimation("stand", true);
        m_isWalking = false;
    }
}

void Character::updateAttacking(const pou::Time &elapsedTime)
{
    bool isAnimationFinished = true;
    for(auto &skeleton : m_skeletons)
    {
        isAnimationFinished = isAnimationFinished & !skeleton.second->isInAnimation();

         //if(skeleton.second->hasTag("attackTest"))
           // std::cout<<"attackTest"<<std::endl;

        if(skeleton.second->hasTag("attack"))
        for(auto c : m_nearbyCharacters)
        if(c != nullptr && c->isAlive() && c->getHurtboxes() != nullptr
           && m_alreadyHitCharacters.find(c) == m_alreadyHitCharacters.end())
        for(const auto hurtBox : *c->getHurtboxes())
        if(this->getHitboxes() != nullptr)
        for(const auto hitBox : *this->getHitboxes())
        {
            auto hitSkeleton    = this->m_skeletons.find(hitBox.getSkeleton());
            auto hurtSkeleton   = c->m_skeletons.find(hurtBox.getSkeleton());

            if(hitSkeleton != this->m_skeletons.end() &&
               hurtSkeleton != c->m_skeletons.end())
            {
                auto hitNode    = hitSkeleton->second->findNode(hitBox.getNode());
                auto hurtNode   = hurtSkeleton->second->findNode(hurtBox.getNode());

                if(hitNode != nullptr && hurtNode != nullptr)
                {
                    bool collision = pou::MathTools::detectBoxCollision(hitBox.getBox(),hurtBox.getBox(),
                                                                        hitNode,hurtNode);

                    if(collision)
                    {
                        m_alreadyHitCharacters.insert(c);

                        float totalDamages = 0;
                        for(auto i = 0 ; i < NBR_DAMAGE_TYPES ; ++i)
                            totalDamages += m_attributes.attackDamages * hitBox.getFactor(i) * hurtBox.getFactor(i);

                        c->damage(totalDamages,c->getGlobalXYPosition()-this->getGlobalXYPosition());
                    }
                }
            }
        }
    }

    if(isAnimationFinished && m_isAttacking)
        this->stopAttacking();
}

void Character::updateLookingDirection(const pou::Time &elapsedTime)
{
    ///Introduce animationRotationSpeed
    if(!m_attributes.immovable)
    if(m_lookingDirection != glm::vec2(0))
    {
        float curRotation = SceneNode::getEulerRotation().z;
        float rotationAmount = elapsedTime.count()*10.0f;
        float wantedRotation = this->computeWantedRotation( curRotation,
                                                            m_lookingDirection);

        if(glm::abs(wantedRotation - curRotation) < rotationAmount)
            SceneNode::setRotation({0,0,wantedRotation});
        else
            SceneNode::rotate(rotationAmount, {0,0, (wantedRotation > curRotation) ? 1 : -1 });
    }
}

/*void Character::updateSounds()
{
    for(auto &skeleton : m_skeletons)
    {
        auto skelPtr = skeleton.second.get();
        if(skelPtr->isNewFrame())
        {
            auto soundTags = skelPtr->getTagValues("playSound");
            for(auto it = soundTags.first ; it != soundTags.second ; ++it)
            {
                auto soundTag = it->second.value;
                auto foundedSound = m_soundsMap.find(soundTag);
                if(foundedSound != m_soundsMap.end())
                    foundedSound->second->play();
            }
        }
    }
}*/

void Character::startAnimation(const std::string &name, bool forceStart)
{
    for(auto &skeleton : m_skeletons)
        skeleton.second->startAnimation(name, forceStart);
}

void Character::addToNearbyCharacters(Character *character)
{
    m_nearbyCharacters.insert(character);
}


bool Character::isAlive() const
{
    return (!m_isDead);
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

const CharacterAttributes &Character::getAttributes() const
{
    return m_attributes;
}

void Character::setLastCharacterUpdateTime(float time, bool force)
{
    if(force || m_lastCharacterUpdateTime < time)
        m_lastCharacterUpdateTime = time;
}

float Character::getLastCharacterUpdateTime()
{
    return m_lastCharacterUpdateTime;
}

float Character::getLastModelUpdateTime()
{
    return m_lastModelUpdateTime;
}

void Character::serializeCharacter(pou::Stream *stream, float clientTime)
{
    bool updateAttributes = false;
    if(!stream->isReading() && clientTime < m_lastAttributesUpdateTime)
        updateAttributes = true;
    stream->serializeBool(updateAttributes);
    if(updateAttributes)
    {
        stream->serializeFloat(m_attributes.maxLife);
        stream->serializeFloat(m_attributes.life, 0, (int)m_attributes.maxLife+1, 2);
    }

    bool updateLookingDirection = false;
    if(!stream->isReading() && clientTime < m_lastLookingDirectionUpdateTime)
        updateLookingDirection = true;
    stream->serializeBool(updateLookingDirection);
    if(updateLookingDirection)
    {
        std::cout<<"UpdateLookingDir:"<<m_lookingDirection.x<<" "<<m_lookingDirection.y<<std::endl;
        glm::vec2 lookingDirection = m_lookingDirection;
        stream->serializeFloat(lookingDirection.x, -1, 1, 2);
        stream->serializeFloat(lookingDirection.y, -1, 1, 2);

        if(stream->isReading())
            this->setLookingDirection(lookingDirection);
        std::cout<<"UpdatedLookingDir:"<<m_lookingDirection.x<<" "<<m_lookingDirection.y<<std::endl;
    }

    bool updateAnimation = false;
}

bool Character::syncFromCharacter(Character *srcCharacter)
{
    if(m_curLocalTime > srcCharacter->m_curLocalTime)
        return (false);

    m_curLocalTime = srcCharacter->m_curLocalTime;

    if(m_lastAttributesUpdateTime < srcCharacter->m_lastAttributesUpdateTime)
    {
        m_attributes.maxLife = srcCharacter->m_attributes.maxLife;
        m_attributes.life = srcCharacter->m_attributes.life;
        m_lastAttributesUpdateTime = srcCharacter->m_lastAttributesUpdateTime;
    }

    if(m_lastLookingDirectionUpdateTime < srcCharacter->m_lastLookingDirectionUpdateTime)
    {
        std::cout<<m_lookingDirection.x<<" "<<m_lookingDirection.y<<" vs "<<srcCharacter->m_lookingDirection.x<<" "<<srcCharacter->m_lookingDirection.y<<std::endl;
        this->setLookingDirection(srcCharacter->m_lookingDirection);
    }

    return (true);
}


