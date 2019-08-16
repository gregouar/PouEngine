#include "Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonModelAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"

Character::Character() : SceneNode(-1,nullptr)
{
    m_walkingDirection = {0,0};
    m_isWalking = false;

    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
        m_partsEntity[i]    = nullptr;
}

Character::~Character()
{
    this->cleanup();
}

bool Character::loadResources()
{
    pou::SpriteSheetAsset *spriteSheet
        = pou::SpriteSheetsHandler::loadAssetFromFile("../data/char1/char1SpritesheetXML.txt");

    if(spriteSheet == nullptr)
        return (false);

    pou::SkeletonModelAsset *skeletonModel
        = pou::SkeletonsHandler::loadAssetFromFile("../data/char1/char1SkeletonXML.txt");


    pou::SpriteModel  *partsModel[TOTAL_PARTS];

    partsModel[BODY_PART] = spriteSheet->getSpriteModel("body");
    partsModel[HEAD_PART] = spriteSheet->getSpriteModel("head");
    partsModel[BAG_PART] = spriteSheet->getSpriteModel("bag");
    partsModel[SHOULDER_L_PART] = spriteSheet->getSpriteModel("shoulderL");
    partsModel[SHOULDER_R_PART] = spriteSheet->getSpriteModel("shoulderR");
    partsModel[HAND_L_PART] = spriteSheet->getSpriteModel("handL");
    partsModel[HAND_R_PART] = spriteSheet->getSpriteModel("handR");
    partsModel[FOOT_L_PART] = spriteSheet->getSpriteModel("footL");
    partsModel[FOOT_R_PART] = spriteSheet->getSpriteModel("footR");
    partsModel[WEAPON_PART] = spriteSheet->getSpriteModel("sword");

    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        m_partsEntity[i] = new pou::SpriteEntity();
        m_partsEntity[i]->setSpriteModel(partsModel[i]);
        m_partsEntity[i]->setOrdering(pou::ORDERED_BY_Z);
        m_partsEntity[i]->setInheritRotation(true);
    }


    m_skeleton =  std::unique_ptr<pou::Skeleton>(new pou::Skeleton(skeletonModel));
    this->addChildNode(m_skeleton.get());

    m_skeleton->attachLimb("body", m_partsEntity[BODY_PART]);
    m_skeleton->attachLimb("head", m_partsEntity[HEAD_PART]);
    m_skeleton->attachLimb("bag", m_partsEntity[BAG_PART]);
    m_skeleton->attachLimb("shoulderL", m_partsEntity[SHOULDER_L_PART]);
    m_skeleton->attachLimb("shoulderR", m_partsEntity[SHOULDER_R_PART]);
    m_skeleton->attachLimb("handL", m_partsEntity[HAND_L_PART]);
    m_skeleton->attachLimb("handR", m_partsEntity[HAND_R_PART]);
    m_skeleton->attachLimb("footL", m_partsEntity[FOOT_L_PART]);
    m_skeleton->attachLimb("footR", m_partsEntity[FOOT_R_PART]);
    m_skeleton->attachLimb("weapon", m_partsEntity[WEAPON_PART]);

    return (true);
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

        glm::vec2 charMove = {m_walkingDirection.x*200*elapsedTime.count(),
                              m_walkingDirection.y*200*elapsedTime.count()};

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
            m_skeleton->startAnimation("walk", true);
        m_isWalking = true;
    } else if(m_isWalking) {
        m_skeleton->startAnimation("stand", true);
        m_isWalking = false;
    }


    SceneNode::update(elapsedTime);
}

void Character::cleanup()
{
    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        delete m_partsEntity[i];
        m_partsEntity[i]    = nullptr;
    }
}

