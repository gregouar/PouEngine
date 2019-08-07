#include "Character.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/scene/SpriteEntity.h"

Character::Character() : SceneNode(-1,nullptr)
{
    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        m_partsModel[i]     = nullptr;
        m_partsEntity[i]    = nullptr;
        m_partsNode[i]      = nullptr;
    }
}

Character::~Character()
{
    this->cleanup();
}

bool Character::loadResources()
{
    pou::SpriteSheetAsset *spriteSheet
        = pou::SpriteSheetsHandler::instance()->loadAssetFromFile("../data/char1/char1XML.txt");

    if(spriteSheet == nullptr)
        return (false);

    m_partsModel[BODY_PART] = spriteSheet->getSpriteModel("body");
    m_partsModel[HEAD_PART] = spriteSheet->getSpriteModel("head");
    m_partsModel[BAG_PART] = spriteSheet->getSpriteModel("bag");
    m_partsModel[SHOULDER_L_PART] = spriteSheet->getSpriteModel("shoulderL");
    m_partsModel[SHOULDER_R_PART] = spriteSheet->getSpriteModel("shoulderR");
    m_partsModel[HAND_L_PART] = spriteSheet->getSpriteModel("handL");
    m_partsModel[HAND_R_PART] = spriteSheet->getSpriteModel("handR");
    m_partsModel[FOOT_L_PART] = spriteSheet->getSpriteModel("footL");
    m_partsModel[FOOT_R_PART] = spriteSheet->getSpriteModel("footR");
    m_partsModel[WEAPON_PART] = spriteSheet->getSpriteModel("weapon");

    m_partsNode[BODY_PART]  = this->createChildNode(0,0,1);//m_partsNode[BODY_PART]  = this;
    m_partsNode[HEAD_PART]  = this->createChildNode(0,-4,5); //m_partsNode[BODY_PART]->createChildNode(0,-4,5);
    m_partsNode[BAG_PART]   = m_partsNode[BODY_PART]->createChildNode(0,4,1);
    m_partsNode[SHOULDER_L_PART]    = m_partsNode[BODY_PART]->createChildNode(-16,-4,4);
    m_partsNode[SHOULDER_R_PART]    = m_partsNode[BODY_PART]->createChildNode(16,-4,4);
    m_partsNode[HAND_L_PART]        = m_partsNode[BODY_PART]->createChildNode(-8,-8,3);
    m_partsNode[HAND_R_PART]        = m_partsNode[BODY_PART]->createChildNode(10,-8,3);
    m_partsNode[FOOT_L_PART]  = this->createChildNode(-10,4,0);
    m_partsNode[FOOT_R_PART]  = this->createChildNode(10,4,0);
    m_partsNode[WEAPON_PART]        = m_partsNode[BODY_PART]->createChildNode(-2,-10,2);


    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        m_partsEntity[i] = new pou::SpriteEntity();
        m_partsEntity[i]->setSpriteModel(m_partsModel[i]);
        m_partsEntity[i]->setOrdering(pou::ORDERED_BY_Z);
        m_partsEntity[i]->setInheritRotation(true);

        if(m_partsNode[i] != nullptr)
            m_partsNode[i]->attachObject(m_partsEntity[i]);
    }

    return (true);

    /*pou::TexturesHandler* textureHandler =  pou::TexturesHandler::instance();
    pou::TextureAsset *texture = textureHandler->loadAssetFromFile("../data/char1/char1spritesheet_scale.png");

    if(texture == nullptr)
        return (false);


    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        m_partsModel[i] = new pou::SpriteModel();
        m_partsModel[i]->setTexture(texture);
    }

    m_partsModel[BODY_PART]->setSize({26.0,16.0});
    m_partsModel[BODY_PART]->setCenter({12.0,6.0});
    m_partsModel[BODY_PART]->setTextureRect({56.0,36.0},
                                            {26.0,16.0},false);

    m_partsModel[HEAD_PART]->setSize({34.0,30.0});
    m_partsModel[HEAD_PART]->setCenter({16.0,18.0});
    m_partsModel[HEAD_PART]->setTextureRect({0.0,0.0},
                                            {34.0,30.0},false);

    m_partsModel[BAG_PART]->setSize({26,14});
    m_partsModel[BAG_PART]->setCenter({12,0});
    m_partsModel[BAG_PART]->setTextureRect({26,32},
                                            {26,14},false);

    m_partsModel[SHOULDER_L_PART]->setSize({24,28});
    m_partsModel[SHOULDER_L_PART]->setCenter({16,12});
    m_partsModel[SHOULDER_L_PART]->setTextureRect({36,2},
                                            {24,28},false);

    m_partsModel[SHOULDER_R_PART]->setSize({24,28});
    m_partsModel[SHOULDER_R_PART]->setCenter({6,12});
    m_partsModel[SHOULDER_R_PART]->setTextureRect({62,2},
                                            {24,28},false);

    m_partsModel[HAND_L_PART]->setSize({10,10});
    m_partsModel[HAND_L_PART]->setCenter({0,8});
    m_partsModel[HAND_L_PART]->setTextureRect({2,36},
                                            {10,10},false);

    m_partsModel[HAND_R_PART]->setSize({8,12});
    m_partsModel[HAND_R_PART]->setCenter({6,10});
    m_partsModel[HAND_R_PART]->setTextureRect({14,34},
                                            {8,12},false);

    m_partsModel[FOOT_L_PART]->setSize({8,16});
    m_partsModel[FOOT_L_PART]->setCenter({4,10});
    m_partsModel[FOOT_L_PART]->setTextureRect({2,48},
                                            {8,16},false);

    m_partsModel[FOOT_R_PART]->setSize({8,16});
    m_partsModel[FOOT_R_PART]->setCenter({2,10});
    m_partsModel[FOOT_R_PART]->setTextureRect({24,48},
                                            {8,16},false);

    m_partsModel[WEAPON_PART]->setSize({64,64});
    m_partsModel[WEAPON_PART]->setCenter({0,62});
    m_partsModel[WEAPON_PART]->setTextureRect({92,4},
                                            {64,64},false);

    m_partsNode[BODY_PART]  = this->createChildNode(0,0,1);//m_partsNode[BODY_PART]  = this;
    m_partsNode[HEAD_PART]  = this->createChildNode(0,-4,5); //m_partsNode[BODY_PART]->createChildNode(0,-4,5);
    m_partsNode[BAG_PART]   = m_partsNode[BODY_PART]->createChildNode(0,4,1);
    m_partsNode[SHOULDER_L_PART]    = m_partsNode[BODY_PART]->createChildNode(-16,-4,4);
    m_partsNode[SHOULDER_R_PART]    = m_partsNode[BODY_PART]->createChildNode(16,-4,4);
    m_partsNode[HAND_L_PART]        = m_partsNode[BODY_PART]->createChildNode(-8,-8,3);
    m_partsNode[HAND_R_PART]        = m_partsNode[BODY_PART]->createChildNode(10,-8,3);
    m_partsNode[FOOT_L_PART]  = this->createChildNode(-10,4,0);
    m_partsNode[FOOT_R_PART]  = this->createChildNode(10,4,0);
    m_partsNode[WEAPON_PART]        = m_partsNode[BODY_PART]->createChildNode(-2,-10,2);


    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        m_partsEntity[i] = new pou::SpriteEntity();
        m_partsEntity[i]->setSpriteModel(m_partsModel[i]);
        m_partsEntity[i]->setOrdering(pou::ORDERED_BY_Z);
        m_partsEntity[i]->setInheritRotation(true);

        if(m_partsNode[i] != nullptr)
            m_partsNode[i]->attachObject(m_partsEntity[i]);
    }

    return (true);*/
}

/*bool Character::addToScene(pou::Scene *scene)
{
    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        scene->
    }
}*/

void Character::cleanup()
{
    for(int i = 0 ; i < TOTAL_PARTS ; ++i)
    {
        delete m_partsEntity[i];
        m_partsEntity[i]    = nullptr;
    }
}

