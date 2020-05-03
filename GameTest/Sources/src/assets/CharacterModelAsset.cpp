#include "assets/CharacterModelAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonModelAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

#include "Character.h"

CharacterModelAsset::CharacterModelAsset() :
    CharacterModelAsset(-1)
{
    //ctor
}


CharacterModelAsset::CharacterModelAsset(const pou::AssetTypeId id) : Asset(id)
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
        std::unique_ptr<pou::Skeleton> skeleton(new pou::Skeleton(skeletonModel.second.skeleton));
        for(auto &limb : skeletonModel.second.limbs)
        {
            std::unique_ptr<pou::SpriteEntity> limbEntity(new pou::SpriteEntity());

            limbEntity->setSpriteModel(limb.spriteModel);
            limbEntity->setOrdering(pou::ORDERED_BY_Z);
            limbEntity->setInheritRotation(true);

            skeleton->attachLimb(limb.node,limbEntity.get());
            //targetCharacter->m_limbs.push_back(std::move(limbEntity));
            targetCharacter->addLimb(std::move(limbEntity));
        }

        targetCharacter->addChildNode(skeleton.get());
        //targetCharacter->m_skeletons.push_back(std::move(skeleton));
        targetCharacter->addSkeleton(std::move(skeleton), skeletonModel.first);
    }

    return (true);
}

const CharacterAttributes &CharacterModelAsset::getAttributes() const
{
    return m_attributes;
}

const std::list<Hitbox> *CharacterModelAsset::getHitboxes() const
{
    return &m_hitboxes;
}

const std::list<Hitbox> *CharacterModelAsset::getHurtboxes() const
{
    return &m_hurtboxes;
}

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

    TiXmlElement* spriteSheetElement = hdl->FirstChildElement("spritesheet").Element();
    while(spriteSheetElement != nullptr)
    {
        if(!this->loadSpriteSheet(spriteSheetElement))
            loaded = false;
        spriteSheetElement = spriteSheetElement->NextSiblingElement("spritesheet");
    }

    TiXmlElement* skeletonElement = hdl->FirstChildElement("skeleton").Element();
    while(skeletonElement != nullptr)
    {
        if(!this->loadSkeleton(skeletonElement))
            loaded = false;
        skeletonElement = skeletonElement->NextSiblingElement("skeleton");
    }

    TiXmlElement* hitboxesElement = hdl->FirstChildElement("hitboxes").Element();
    if(hitboxesElement != nullptr)
        if(!this->loadHitboxes(hitboxesElement, m_hitboxes))
            loaded = false;

    hitboxesElement = hdl->FirstChildElement("hurtboxes").Element();
    if(hitboxesElement != nullptr)
        if(!this->loadHitboxes(hitboxesElement, m_hurtboxes))
            loaded = false;


    TiXmlElement* attributesElement = hdl->FirstChildElement("attributes").Element();
    if(attributesElement != nullptr)
        if(!this->loadAttributes(attributesElement))
            loaded = false;

    if(loaded)
        pou::Logger::write("Character model loaded from file: "+m_filePath);

    return (loaded);
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

bool CharacterModelAsset::loadSkeleton(TiXmlElement *element)
{

    std::string skeletonName = "skeleton"+std::to_string(m_skeletonModels.size());

    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        skeletonName = std::string(nameAtt);

    auto skelPair = m_skeletonModels.insert({skeletonName, SkeletonWithLimbs ()});
    if(!skelPair.second)
        pou::Logger::warning("Multiple skeletons with name \""+skeletonName+"\" in character model:"+m_filePath);

    //m_skeletonModels.push_back(SkeletonWithLimbs ());
    auto &skeletonWithLimbs = skelPair.first->second;//m_skeletonModels.back();

    skeletonWithLimbs.skeleton = pou::SkeletonModelsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType);

    auto limbChild = element->FirstChildElement("limb");
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

    return (true);
}


bool CharacterModelAsset::loadHitboxes(TiXmlElement *element, std::list<Hitbox> &boxList)
{
    auto boxChild = element->FirstChildElement("box");
    while(boxChild != nullptr)
    {
        auto boxElement = boxChild->ToElement();
        auto skeletonAtt= boxElement->Attribute("skeleton");
        auto nodeAtt    = boxElement->Attribute("node");

        if(skeletonAtt != nullptr && nodeAtt != nullptr)
        {
            boxList.push_back(Hitbox(std::string(skeletonAtt), std::string(nodeAtt)));
            auto &box = boxList.back();

            auto factorAtt = boxElement->Attribute("factor");
            if(factorAtt != nullptr)
                box.factor = pou::Parser::parseFloat(std::string(factorAtt));

            auto sizeElement = boxElement->FirstChildElement("size");
            if(sizeElement != nullptr)
            {
                if(sizeElement->Attribute("x") != nullptr)
                    box.box.size.x = pou::Parser::parseFloat(std::string(sizeElement->Attribute("x")));
                if(sizeElement->Attribute("y") != nullptr)
                    box.box.size.y = pou::Parser::parseFloat(std::string(sizeElement->Attribute("y")));
            }

            auto centerElement = boxElement->FirstChildElement("center");
            if(centerElement != nullptr)
            {
                if(centerElement->Attribute("x") != nullptr)
                    box.box.center.x = pou::Parser::parseFloat(std::string(centerElement->Attribute("x")));
                if(centerElement->Attribute("y") != nullptr)
                    box.box.center.y = pou::Parser::parseFloat(std::string(centerElement->Attribute("y")));
            }
        } else
            pou::Logger::warning("Incomplete hitbox in character model: "+m_filePath);

        boxChild = boxChild->NextSiblingElement("box");
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
    {
        m_attributes.life       = pou::Parser::parseFloat(e->GetText());
        m_attributes.maxLife    = m_attributes.life;
    }

    e = element->FirstChildElement("attackDamages");
    if(e != nullptr)
        m_attributes.attackDamages = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("immovable");
    if(e != nullptr)
        m_attributes.immovable = pou::Parser::parseBool(e->GetText());

    return (true);
}

