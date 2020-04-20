#include "Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/CharacterModelAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"

Character::Character() : SceneNode(-1,nullptr)
{
    m_walkingSpeed      = 250.0f;
    m_rotationRadius    = 0.0f;

    m_isDestinationSet  = false;
    m_destination       = {0,0};

    m_isWalking         = false;
    m_isAttacking       = false;
    m_walkingDirection  = {0,0};
    m_lookingDirection  = {0,-1};
}

Character::~Character()
{
    this->cleanup();
}

bool Character::loadModel(const std::string &path)
{
    this->cleanup();

    pou::CharacterModelAsset *characterModel
        = pou::CharacterModelsHandler::loadAssetFromFile(path);

    if(characterModel == nullptr)
        return (false);

    characterModel->generateOnNode(this, &m_skeletons, &m_limbs);

    return (true);
}

void Character::setWalkingSpeed(float speed)
{
    if(speed >= 0)
        m_walkingSpeed = speed;
}

void Character::setRotationRadius(float radius)
{
    if(radius >= 0)
        m_rotationRadius = radius;
}

void Character::setDestination(glm::vec2 destination)
{
    m_destination       = destination;
    m_isDestinationSet  = true;
}

void Character::walk(glm::vec2 direction)
{
    m_walkingDirection = direction;
    m_lookingDirection = m_walkingDirection;
}

void Character::attack(glm::vec2 direction, const std::string &animationName)
{
    if(!m_isAttacking)
    {
        this->startAnimation(animationName,true);
        m_isAttacking = true;
        m_isWalking = false;
        //if(direction != glm::vec2(0))
            m_lookingDirection = direction;
    }
}

bool Character::walkToDestination(const pou::Time& elapsedTime)
{
    glm::vec2 gpos{SceneNode::getGlobalPosition().x,
                   SceneNode::getGlobalPosition().y};
    glm::vec2 delta = m_destination - gpos;
    float ndelta    = glm::dot(delta,delta);

    if(ndelta <= m_walkingSpeed*elapsedTime.count()*m_walkingSpeed*elapsedTime.count())
    {
        SceneNode:setGlobalPosition(m_destination);
        m_isDestinationSet = false;
        this->walk({0,0});
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
    glm::vec2 gpos{SceneNode::getGlobalPosition().x,
                   SceneNode::getGlobalPosition().y};
    glm::vec2 delta = destination - gpos;
    glm::vec2 normalizedDelta = glm::normalize(delta);

    glm::vec2 coLookinDirection = {m_lookingDirection.y,
                                  -m_lookingDirection.x};

    if(glm::dot(normalizedDelta - m_lookingDirection,
                normalizedDelta - m_lookingDirection) <= 0.001f)
    {
        m_walkingDirection = normalizedDelta;
    } else {
        float arcLength = m_walkingSpeed*elapsedTime.count();
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
        m_lookingDirection = relNewLookingDir.x * coLookinDirection
                            + relNewLookingDir.y * m_lookingDirection;
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

void Character::update(const pou::Time& elapsedTime)
{
    bool wantToWalk = false;

    if(m_isAttacking)
    {
        bool isAnimationFinished = true;
        for(auto &skeleton : m_skeletons)
            isAnimationFinished = isAnimationFinished & !skeleton->isInAnimation();

        if(isAnimationFinished)
        {
            m_isAttacking = false;
            this->startAnimation("stand", true);
        }
    } else {
        if(m_isDestinationSet)
            wantToWalk = this->walkToDestination(elapsedTime);

        if(m_walkingDirection != glm::vec2(0))
        {
            m_walkingDirection = glm::normalize(m_walkingDirection);

            glm::vec2 charMove = {m_walkingDirection.x*m_walkingSpeed*elapsedTime.count(),
                                  m_walkingDirection.y*m_walkingSpeed*elapsedTime.count()};

            //m_lookingDirection = m_walkingDirection;

            SceneNode:move(charMove);
            wantToWalk = true;
        }

        if(wantToWalk && !m_isWalking)
        {
            this->startAnimation("walk", true);
            m_isWalking = true;
        }

        if(!wantToWalk && m_isWalking)
        {
            this->startAnimation("stand", true);
            m_isWalking = false;
        }
    }

    ///Introduce animationRotationSpeed

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

    SceneNode::update(elapsedTime);
}

void Character::startAnimation(const std::string &name, bool forceStart)
{
    for(auto &skeleton : m_skeletons)
        skeleton->startAnimation(name, forceStart);
}

void Character::cleanup()
{
    m_limbs.clear();
    m_skeletons.clear();
}

