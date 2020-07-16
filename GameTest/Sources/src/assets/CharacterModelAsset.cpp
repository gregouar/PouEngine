#include "assets/CharacterModelAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"
#include "PouEngine/assets/MeshAsset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonModelAsset.h"

#include "PouEngine/scene/CollisionObject.h"

#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Parser.h"

#include "character/Character.h"
#include "ai/AiScriptedComponent.h"
#include "assets/AiScriptModelAsset.h"

CharacterModelAsset::CharacterModelAsset() :
    CharacterModelAsset(-1)
{
    //ctor
}


CharacterModelAsset::CharacterModelAsset(const pou::AssetTypeId id) : Asset(id),
    m_aiScriptModel(nullptr)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}

CharacterModelAsset::~CharacterModelAsset()
{
    //dtor
}


/*bool CharacterModelAsset::generateOnNode(pou::SceneNode *parentNode,
                            std::list<std::unique_ptr<pou::Skeleton> > *skeletons,
                            std::list<std::unique_ptr<pou::SpriteEntity> > *limbs)
{
    if(parentNode == nullptr || skeletons == nullptr || limbs == nullptr)
        return (false);

    for(auto &skeletonModel : m_skeletonModels)
    {
        std::unique_ptr<pou::Skeleton> skeleton(new pou::Skeleton(skeletonModel.second.skeleton));
        for(auto &limb : skeletonModel.second.limbs)
        {
            std::unique_ptr<pou::SpriteEntity> limbEntity(new pou::SpriteEntity());

            limbEntity->setSpriteModel(limb.spriteModel);
            limbEntity->setOrdering(pou::ORDERED_BY_Z);
            limbEntity->setInheritRotation(true);

            skeleton->attachLimb(limb.node,limbEntity.get());
            limbs->push_back(std::move(limbEntity));
        }

        parentNode->addChildNode(skeleton.get());
        skeletons->push_back(std::move(skeleton));
    }

    return (true);
}*/

bool CharacterModelAsset::generateCharacter(Character *targetCharacter)
{
    if(targetCharacter == nullptr)
        return (false);

    for(auto &skeletonModel : m_skeletonModels)
    {
        auto skeleton = std::make_shared<CharacterSkeleton>(skeletonModel.second.skeleton);
        for(auto &limb : *(skeletonModel.second.assetsModel.getLimbs()))
        {
            auto sceneEntity = targetCharacter->addLimb(&limb);
            skeleton->attachLimb(limb.node, limb.state, sceneEntity);
        }

        for(auto &sound : *(skeletonModel.second.assetsModel.getSounds()))
        {
            //targetCharacter->addSound(&sound);
            skeleton->attachSound(targetCharacter->addSound(&sound), sound.name);
        }


        targetCharacter/*->node()*/->addChildNode(skeleton);
        targetCharacter->addSkeleton(skeleton, skeletonModel.first);
    }

    if(m_aiScriptModel)
    {
        auto aiScript = std::make_shared<AiScriptedComponent>(targetCharacter);
        aiScript->createFromModel(m_aiScriptModel);
        targetCharacter->setAiComponent(aiScript);
    }

    for(auto &collisionBox : m_collisionBoxes)
    {
        auto collisionObject = std::make_shared<pou::CollisionObject>();
        collisionObject->setMass(collisionBox.mass);
        collisionObject->setBox(collisionBox.box);
        targetCharacter->attachObject(collisionObject);
    }

    for(auto &collisionDisk : m_collisionDisks)
    {
        auto collisionObject = std::make_shared<pou::CollisionObject>();
        collisionObject->setMass(collisionDisk.mass);
        collisionObject->setDisk(collisionDisk.radius);
        targetCharacter->attachObject(collisionObject);
    }

    return (true);
}

const CharacterModelAttributes &CharacterModelAsset::getAttributes() const
{
    return m_attributes;
}

const std::vector<Hitbox> *CharacterModelAsset::getHitboxes() const
{
    return &m_hitboxes;
}

const std::vector<Hitbox> *CharacterModelAsset::getHurtboxes() const
{
    return &m_hurtboxes;
}

/*const std::vector<pou::BoxBody> *CharacterModelAsset::getCollisionboxes() const
{
    return &m_collisionboxes;
}*/

bool CharacterModelAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load character model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool CharacterModelAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    element = hdl->FirstChildElement("aiscript").Element();
    if(element != nullptr)
    {
        if(!this->loadAiScript(element))
            loaded = false;
    }

    element = hdl->FirstChildElement("spritesheet").Element();
    while(element != nullptr)
    {
        if(!this->loadSpriteSheet(element))
            loaded = false;
        element = element->NextSiblingElement("spritesheet");
    }

    element = hdl->FirstChildElement("light").Element();
    while(element != nullptr)
    {
        if(!this->loadLightModel(element))
            loaded = false;
        element = element->NextSiblingElement("light");
    }

    element = hdl->FirstChildElement("soundbank").Element();
    while(element != nullptr)
    {
        if(!this->loadSoundBank(element))
            loaded = false;
        element = element->NextSiblingElement("soundbank");
    }

    element = hdl->FirstChildElement("skeleton").Element();
    while(element != nullptr)
    {
        if(!this->loadSkeleton(element))
            loaded = false;
        element = element->NextSiblingElement("skeleton");
    }

    element = hdl->FirstChildElement("hitboxes").Element();
    if(element != nullptr)
        if(!this->loadHitboxes(element, m_hitboxes))
            loaded = false;

    element = hdl->FirstChildElement("hurtboxes").Element();
    if(element != nullptr)
        if(!this->loadHitboxes(element, m_hurtboxes))
            loaded = false;

    element = hdl->FirstChildElement("collisions").Element();
    if(element != nullptr)
    {
        if(!this->loadCollisionBoxes(element, m_collisionBoxes))
            loaded = false;

        if(!this->loadCollisionDisks(element, m_collisionDisks))
            loaded = false;
    }

    /*element = hdl->FirstChildElement("collisionboxes").Element();
    if(element != nullptr)
        if(!this->loadCollisionBoxes(element, m_collisionBoxes))
            loaded = false;

    element = hdl->FirstChildElement("collisiondisks").Element();
    if(element != nullptr)
        if(!this->loadCollisionDisks(element, m_collisionDisks))
            loaded = false;*/


    element = hdl->FirstChildElement("attributes").Element();
    if(element != nullptr)
        if(!this->loadAttributes(element))
            loaded = false;

    if(loaded)
        pou::Logger::write("Character model loaded from file: "+m_filePath);

    return (loaded);
}

bool CharacterModelAsset::loadAiScript(TiXmlElement *element)
{
    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    m_aiScriptModel = AiScriptModelsHandler::loadAssetFromFile(std::string(pathAtt));

    return (true);
}

bool CharacterModelAsset::loadSpriteSheet(TiXmlElement *element)
{
    std::string spriteSheetName = "spriteSheet"+std::to_string(m_spriteSheets.size());

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        spriteSheetName = std::string(nameAtt);


    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    if(!m_spriteSheets.insert({spriteSheetName,
                              pou::SpriteSheetsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType)}).second)
        pou::Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in character model:"+m_filePath);

    return (true);
}

bool CharacterModelAsset::loadLightModel(TiXmlElement *element)
{
    std::string lightName = "light"+std::to_string(m_lightModels.size());

    auto att = element->Attribute("name");
    if(att != nullptr)
        lightName = std::string(att);

    pou::LightModel lightModel;

    lightModel.loadFromXML(element);

    if(!m_lightModels.insert({lightName,lightModel}).second)
        pou::Logger::warning("Multiple lights with name \""+lightName+"\" in character model:"+m_filePath);

    return (true);
}


bool CharacterModelAsset::loadSoundBank(TiXmlElement *element)
{
    //std::string soundBankName = "soundbank"+std::to_string(m_soundBanks.size());

    /*auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        spriteSheetName = std::string(nameAtt);*/


    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    /*if(!m_spriteSheets.insert({spriteSheetName,
                              pou::SpriteSheetsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType)}).second)
        pou::Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in character model:"+m_filePath);*/

    pou::SoundBanksHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt),m_loadType);

    return (true);
}

bool CharacterModelAsset::loadSkeleton(TiXmlElement *element)
{

    std::string skeletonName = "skeleton"+std::to_string(m_skeletonModels.size());

    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        skeletonName = std::string(nameAtt);



    auto skeleton = pou::SkeletonModelsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType);
    AssetsForSkeletonModel  assetsModel(&m_spriteSheets, &m_lightModels, m_fileDirectory);

    auto skelPair = m_skeletonModels.insert({skeletonName, {skeleton, assetsModel}});
    if(!skelPair.second)
        pou::Logger::warning("Multiple skeletons with name \""+skeletonName+"\" in character model:"+m_filePath);

    skelPair.first->second.assetsModel.loadFromXML(element);

    //m_skeletonModels.push_back(SkeletonWithLimbs ());

   // auto &skeletonModelWithAssets = skelPair.first->second;//m_skeletonModels.back();

    /*auto limbChild = element->FirstChildElement("limb");
    while(limbChild != nullptr)
    {
        auto limbElement = limbChild->ToElement();

        auto nodeAtt        = limbElement->Attribute("node");
        auto spriteAtt      = limbElement->Attribute("sprite");
        auto spriteSheetAtt = limbElement->Attribute("spritesheet");

        if(nodeAtt != nullptr && spriteAtt != nullptr && spriteSheetAtt != nullptr)
        {

            auto spritesheetIt = m_spriteSheets.find(std::string(spriteSheetAtt));
            if(spritesheetIt != m_spriteSheets.end())
            {
                auto spriteModel = spritesheetIt->second->getSpriteModel(std::string(spriteAtt));
                skeletonWithLimbs.limbs.push_back({std::string(nodeAtt),
                                                  spriteModel});
                if(spriteModel == nullptr)
                    pou::Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                                    +std::string(spriteSheetAtt)+"\" in: "+m_filePath);

            } else
                pou::Logger::warning("Spritesheet named \""+std::string(nodeAtt)+"\" not found in: "+m_filePath);
        } else
            pou::Logger::warning("Incomplete limb in character model: "+m_filePath);

        limbChild = limbChild->NextSiblingElement("limb");
    }


    auto soundChild = element->FirstChildElement("sound");
    while(soundChild != nullptr)
    {
        auto soundElement = soundChild->ToElement();

        auto nameAtt    = soundElement->Attribute("name");
        auto pathAtt    = soundElement->Attribute("path");
        auto typeAtt    = soundElement->Attribute("type");

        if(nameAtt != nullptr && pathAtt != nullptr)
        {
            skeletonWithLimbs.sounds.push_back(SoundModel ());
            skeletonWithLimbs.sounds.back().name = nameAtt;
            skeletonWithLimbs.sounds.back().path = pathAtt;

            if(typeAtt != nullptr && std::string(typeAtt) != "event")
                skeletonWithLimbs.sounds.back().isEvent = false;
            else
                skeletonWithLimbs.sounds.back().isEvent = true;

        }

        soundChild = soundChild->NextSiblingElement("sound");
    }*/

    return (true);
}


bool CharacterModelAsset::loadHitboxes(TiXmlElement *element, std::vector<Hitbox> &boxList)
{
    auto boxChild = element->FirstChildElement("box");
    while(boxChild != nullptr)
    {
        boxList.push_back(Hitbox ());

        if(!boxList.back().loadFromXML(boxChild->ToElement()))
            pou::Logger::warning("Incomplete hitbox in character model: "+m_filePath);

        boxChild = boxChild->NextSiblingElement("box");
    }

    return (true);
}

bool CharacterModelAsset::loadCollisionBoxes(TiXmlElement *element, std::vector<pou::BoxBody> &boxList)
{
    auto boxChild = element->FirstChildElement("box");
    while(boxChild != nullptr)
    {
        boxList.push_back(pou::BoxBody ());

        auto &box = boxList.back().box;

        auto boxElement = boxChild->ToElement();

        float mass = 0;
        auto massChild = boxElement->FirstChildElement("mass");
        if(massChild != nullptr)
            mass = pou::Parser::parseFloat(massChild->GetText());

        auto sizeElement = boxElement->FirstChildElement("size");
        if(sizeElement != nullptr)
        {
            if(sizeElement->Attribute("x") != nullptr)
                box.size.x = pou::Parser::parseFloat(std::string(sizeElement->Attribute("x")));
            if(sizeElement->Attribute("y") != nullptr)
                box.size.y = pou::Parser::parseFloat(std::string(sizeElement->Attribute("y")));
        }

        auto centerElement = boxElement->FirstChildElement("center");
        if(centerElement != nullptr)
        {
            if(centerElement->Attribute("x") != nullptr)
                box.center.x = pou::Parser::parseFloat(std::string(centerElement->Attribute("x")));
            if(centerElement->Attribute("y") != nullptr)
                box.center.y = pou::Parser::parseFloat(std::string(centerElement->Attribute("y")));
        }

        if(mass < 0)
            mass = -1;
        boxList.back().mass = mass;
        boxChild = boxChild->NextSiblingElement("box");
    }

    return (true);
}

bool CharacterModelAsset::loadCollisionDisks(TiXmlElement *element, std::vector<pou::DiskBody> &diskList)
{
    auto diskChild = element->FirstChildElement("disk");
    while(diskChild != nullptr)
    {
        diskList.push_back(pou::DiskBody ());

        auto boxElement = diskChild->ToElement();

        float radius = 0;
        auto radiusChild = boxElement->FirstChildElement("radius");
        if(radiusChild != nullptr)
            radius = pou::Parser::parseFloat(radiusChild->GetText());

        if(radius < 0)
            radius = 0;

        float mass = 0;
        auto massChild = boxElement->FirstChildElement("mass");
        if(massChild != nullptr)
            mass = pou::Parser::parseFloat(massChild->GetText());

        if(mass < 0)
            mass = -1;

        diskList.back().radius = radius;
        diskList.back().mass = mass;
        diskChild = diskChild->NextSiblingElement("disk");
    }

    return (true);
}

bool CharacterModelAsset::loadAttributes(TiXmlElement *element)
{
    auto e = element->FirstChildElement("walkingSpeed");
    if(e != nullptr)
        m_attributes.walkingSpeed = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("attackDelay");
    if(e != nullptr)
        m_attributes.attackDelay = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("life");
    if(e != nullptr)
        m_attributes.maxLife     = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("attackDamages");
    if(e != nullptr)
        m_attributes.attackDamages = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("immovable");
    if(e != nullptr)
        m_attributes.immovable = pou::Parser::parseBool(e->GetText());

    e = element->FirstChildElement("rotationRadius");
    if(e != nullptr)
        m_attributes.rotationRadius = pou::Parser::parseFloat(e->GetText());

    return (true);
}



/// AssetsForSkeletonModel ///


AssetsForSkeletonModel::AssetsForSkeletonModel(const std::map<std::string, pou::SpriteSheetAsset*> * spriteSheets,
                                                const std::map<std::string, pou::LightModel> *lightModels,
                                               const std::string &fileDirectory) :
    m_spriteSheets(spriteSheets),
    m_lightModels(lightModels),
    m_fileDirectory(fileDirectory)
{

}

std::list<LimbModel> *AssetsForSkeletonModel::getLimbs()
{
    return &m_limbs;
}

std::list<SoundModel> *AssetsForSkeletonModel::getSounds()
{
    return &m_sounds;
}

bool AssetsForSkeletonModel::loadFromXML(TiXmlElement *element)
{
    auto limbChild = element->FirstChildElement("limb");
    while(limbChild != nullptr)
    {
        auto limbElement = limbChild->ToElement();

        auto nodeAtt        = limbElement->Attribute("node");
        auto stateAtt       = limbElement->Attribute("state");
        auto spriteAtt      = limbElement->Attribute("sprite");
        auto spriteSheetAtt = limbElement->Attribute("spritesheet");
        auto meshAtt        = limbElement->Attribute("mesh");
        auto lightAtt       = limbElement->Attribute("light");

        std::string state;
        if(stateAtt != nullptr)
            state = std::string(stateAtt);

        if(nodeAtt != nullptr)
        {
            if(spriteAtt != nullptr && spriteSheetAtt != nullptr)
            {
                auto spritesheetIt = m_spriteSheets->find(std::string(spriteSheetAtt));
                if(spritesheetIt != m_spriteSheets->end())
                {
                    auto spriteModel = spritesheetIt->second->getSpriteModel(std::string(spriteAtt));
                    m_limbs.push_back({std::string(nodeAtt), state, spriteModel, nullptr, nullptr});
                    if(spriteModel == nullptr)
                        pou::Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                                        +std::string(spriteSheetAtt)+"\"");

                } else
                    pou::Logger::warning("Spritesheet named \""+std::string(spriteSheetAtt)+"\" not found");
            }
            else if(meshAtt != nullptr)
            {
                auto *meshAsset = pou::MeshAssetsHandler::loadAssetFromFile(m_fileDirectory+std::string(meshAtt), pou::LoadType_InThread);
                m_limbs.push_back({std::string(nodeAtt), state, nullptr, meshAsset,nullptr});
            }
            else if(lightAtt != nullptr)
            {
                auto lightIt = m_lightModels->find(std::string(lightAtt));
                if(lightIt != m_lightModels->end())
                {
                    /*auto spriteModel = lightIt->second->getSpriteModel(std::string(spriteAtt));
                    m_limbs.push_back({std::string(nodeAtt), state, spriteModel, nullptr});
                    if(spriteModel == nullptr)
                        pou::Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                                        +std::string(spriteSheetAtt)+"\"");*/
                    m_limbs.push_back({std::string(nodeAtt), state, nullptr, nullptr,&lightIt->second});

                } else
                    pou::Logger::warning("Light named \""+std::string(lightAtt)+"\" not found");
            }

        } else
            pou::Logger::warning("Incomplete limb");

        limbChild = limbChild->NextSiblingElement("limb");
    }


    auto soundChild = element->FirstChildElement("sound");
    while(soundChild != nullptr)
    {
        auto soundElement = soundChild->ToElement();

        auto nameAtt    = soundElement->Attribute("name");
        auto pathAtt    = soundElement->Attribute("path");
        auto typeAtt    = soundElement->Attribute("type");

        if(nameAtt != nullptr && pathAtt != nullptr)
        {
            m_sounds.push_back(SoundModel ());
            m_sounds.back().name = nameAtt;
            m_sounds.back().path = pathAtt;

            if(typeAtt != nullptr && std::string(typeAtt) != "event")
                m_sounds.back().isEvent = false;
            else
                m_sounds.back().isEvent = true;
        }

        soundChild = soundChild->NextSiblingElement("sound");
    }

    return (true);
}




