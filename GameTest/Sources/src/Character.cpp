#include "Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/CharacterModelAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"

Character::Character() : SceneNode(-1,nullptr)
{
    m_walkingSpeed = 250.0f;

    m_walkingDirection = {0,0};
    m_isWalking = false;
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

void Character::walk(glm::vec2 direction)
{
    m_walkingDirection = direction;
}

void Character::update(const pou::Time& elapsedTime)
{
    if(m_walkingDirection != glm::vec2(0))
    {
        m_walkingDirection = glm::normalize(m_walkingDirection);

        glm::vec2 charMove = {m_walkingDirection.x*m_walkingSpeed*elapsedTime.count(),
                              m_walkingDirection.y*m_walkingSpeed*elapsedTime.count()};

        SceneNode:move(charMove);

        float curRot = SceneNode::getEulerRotation().z;
        float desiredRot = glm::pi<float>()/2.0+glm::atan(m_walkingDirection.y, m_walkingDirection.x);

        if(glm::abs(desiredRot-curRot) > glm::abs(desiredRot-curRot+glm::pi<float>()*2.0))
            desiredRot += glm::pi<float>()*2.0;

        if(glm::abs(desiredRot-curRot) > glm::abs(desiredRot-curRot-glm::pi<float>()*2.0))
            desiredRot -= glm::pi<float>()*2.0;

        float rotAmount = elapsedTime.count()*10.0f;
        float newRot = curRot;

        if(glm::abs(desiredRot - curRot) < rotAmount)
            SceneNode::setRotation({0,0,desiredRot});
        else
            SceneNode::rotate(rotAmount, {0,0, (desiredRot > curRot) ? 1 : -1 });

        if(!m_isWalking)
            this->startAnimation("walk", true);
        m_isWalking = true;
    } else if(m_isWalking) {
        this->startAnimation("stand", true);
        m_isWalking = false;
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
    /*for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        delete m_partsEntity[i];
        m_partsEntity[i]    = nullptr;
    }*/

    m_limbs.clear();
    m_skeletons.clear();
}

