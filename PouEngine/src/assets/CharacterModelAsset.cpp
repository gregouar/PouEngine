#include "PouEngine/assets/CharacterModelAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonModelAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{

CharacterModelAsset::CharacterModelAsset() :
    CharacterModelAsset(-1)
{
    //ctor
}


CharacterModelAsset::CharacterModelAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}

CharacterModelAsset::~CharacterModelAsset()
{
    //dtor
}


bool CharacterModelAsset::generateOnNode(SceneNode *parentNode,
                            std::list<std::unique_ptr<Skeleton> > *skeletons,
                            std::list<std::unique_ptr<SpriteEntity> > *limbs)
{
    if(parentNode == nullptr || skeletons == nullptr || limbs == nullptr)
        return (false);

    for(auto &skeletonModel : m_skeletonModels)
    {
        std::unique_ptr<Skeleton> skeleton(new Skeleton(skeletonModel.skeleton));
        for(auto &limb : skeletonModel.limbs)
        {
            std::unique_ptr<SpriteEntity> limbEntity(new SpriteEntity());

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
}

bool CharacterModelAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        Logger::error("Cannot load character model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        Logger::error(errorReport);
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


    if(loaded)
        Logger::write("Character model loaded from file: "+m_filePath);

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
                              SpriteSheetsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType)}).second)
        Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in character model:"+m_filePath);

    return (true);
}

bool CharacterModelAsset::loadSkeleton(TiXmlElement *element)
{
    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    m_skeletonModels.push_back(SkeletonWithLimbs ());
    auto &skeletonWithLimbs = m_skeletonModels.back();

    skeletonWithLimbs.skeleton = SkeletonModelsHandler::loadAssetFromFile(m_fileDirectory+std::string(pathAtt), m_loadType);


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
                    Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                                    +std::string(spriteSheetAtt)+"\" in: "+m_filePath);

            } else
                Logger::warning("Spritesheet named \""+std::string(nodeAtt)+"\" not found in: "+m_filePath);
        } else
            Logger::warning("Incomplete limb in character model: "+m_filePath);

        limbChild = limbChild->NextSiblingElement("limb");
    }

    return (true);
}

}
