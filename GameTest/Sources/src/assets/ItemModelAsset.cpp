#include "assets/ItemModelAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SoundBankAsset.h"
#include "PouEngine/assets/MeshAsset.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/SkeletonModelAsset.h"


#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

#include "Character.h"

ItemModelAsset::ItemModelAsset() :
    ItemModelAsset(-1)
{
    //ctor
}


ItemModelAsset::ItemModelAsset(const pou::AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;

    m_attributes.type = NBR_GEAR_TYPES;
}

ItemModelAsset::~ItemModelAsset()
{
    //dtor
}

bool ItemModelAsset::removeFromCharacter(Character *character)
{
    if(character == nullptr)
        return (false);

    bool r = true;

    /*for(auto skeleton : m_limbs)
    for(auto limb : skeleton.second)
        if(!character->removeLimbFromSkeleton(&limb, skeleton.first))
            r = false;*/

    for(auto &skeleton :  m_skeletonAssetsModels)
    {
        for(auto &limb : *(skeleton.second.getLimbs()))
            if(!character->removeLimbFromSkeleton(&limb, skeleton.first))
                r = false;

        for(auto &sound : *(skeleton.second.getSounds()))
            if(!character->removeSoundFromSkeleton(&sound, skeleton.first))
                r = false;
    }

    return (r);
}

bool ItemModelAsset::generateOnCharacter(Character *character)
{
    if(character == nullptr)
        return (false);

    bool r = true;

    /*for(auto skeleton : m_limbs)
    for(auto limb : skeleton.second)
        if(!character->addLimbToSkeleton(&limb, skeleton.first))
            r = false;*/

    for(auto &skeleton :  m_skeletonAssetsModels)
    {
        for(auto &limb : *(skeleton.second.getLimbs()))
            if(!character->addLimbToSkeleton(&limb, skeleton.first))
                r = false;

        for(auto &sound : *(skeleton.second.getSounds()))
            if(!character->addSoundToSkeleton(&sound, skeleton.first))
                r = false;
    }

    return (r);
}

const ItemAttributes &ItemModelAsset::getAttributes() const
{
    return (m_attributes);
}

const std::list<Hitbox> *ItemModelAsset::getHitboxes() const
{
    return &m_hitboxes;
}

/*const std::map<std::string, std::list<LimbModel> > *getLimbs() const
{
    return &m_limbs;
}*/

bool ItemModelAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load item model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool ItemModelAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element = hdl->FirstChildElement("spritesheet").Element();
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


    TiXmlElement* attributesElement = hdl->FirstChildElement("attributes").Element();
    if(attributesElement != nullptr)
        if(!this->loadAttributes(attributesElement))
            loaded = false;

    if(loaded)
        pou::Logger::write("Item model loaded from file: "+m_filePath);

    return (loaded);
}


bool ItemModelAsset::loadSpriteSheet(TiXmlElement *element)
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
        pou::Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in item model:"+m_filePath);

    return (true);
}


bool ItemModelAsset::loadLightModel(TiXmlElement *element)
{
    std::string lightName = "light"+std::to_string(m_lightModels.size());

    auto att = element->Attribute("name");
    if(att != nullptr)
        lightName = std::string(att);

    pou::LightModel lightModel;

    att = element->Attribute("type");
    if(att != nullptr)
    {
        if(std::string(att) == "omni")
            lightModel.type = pou::LightType_Omni;
        else if(std::string(att) == "directional")
            lightModel.type = pou::LightType_Directional;
        else if(std::string(att) == "spot")
            lightModel.type = pou::LightType_Spot;
    }

    att = element->Attribute("radius");
    if(att != nullptr)
        lightModel.radius = pou::Parser::parseFloat(att);

    att = element->Attribute("intensity");
    if(att != nullptr)
        lightModel.intensity = pou::Parser::parseFloat(att);

    att = element->Attribute("castShadow");
    if(att != nullptr)
        lightModel.castShadow = pou::Parser::parseBool(att);

    auto colorChild = element->FirstChildElement("color");
    if(colorChild != nullptr)
    {
        auto colorElement = colorChild->ToElement();
        att = colorElement->Attribute("r");
        if(att != nullptr)
            lightModel.color.r = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("g");
        if(att != nullptr)
            lightModel.color.g = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("b");
        if(att != nullptr)
            lightModel.color.b = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("a");
        if(att != nullptr)
            lightModel.color.a = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("red");
        if(att != nullptr)
            lightModel.color.r = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("green");
        if(att != nullptr)
            lightModel.color.g = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("blue");
        if(att != nullptr)
            lightModel.color.b = pou::Parser::parseFloat(att);
        att = colorElement->Attribute("alpha");
        if(att != nullptr)
            lightModel.color.a = pou::Parser::parseFloat(att);
    }


    auto directionChild = element->FirstChildElement("direction");
    if(directionChild != nullptr)
    {
        auto directionElement = directionChild->ToElement();
        att = directionElement->Attribute("x");
        if(att != nullptr)
            lightModel.direction.x = pou::Parser::parseFloat(att);
        att = directionElement->Attribute("y");
        if(att != nullptr)
            lightModel.direction.y = pou::Parser::parseFloat(att);
        att = directionElement->Attribute("z");
        if(att != nullptr)
            lightModel.direction.z = pou::Parser::parseFloat(att);
    }

    if(!m_lightModels.insert({lightName,lightModel}).second)
        pou::Logger::warning("Multiple lights with name \""+lightName+"\" in character model:"+m_filePath);

    return (true);
}


bool ItemModelAsset::loadSoundBank(TiXmlElement *element)
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


bool ItemModelAsset::loadSkeleton(TiXmlElement *element)
{
    std::string skeletonName = "skeleton"+std::to_string(m_skeletonAssetsModels.size());

    /*auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);*/

    auto nameAtt = element->Attribute("name");
    if(nameAtt != nullptr)
        skeletonName = std::string(nameAtt);

    auto skelPair = m_skeletonAssetsModels.insert({skeletonName, AssetsForSkeletonModel (&m_spriteSheets,&m_lightModels,m_fileDirectory)});
    if(!skelPair.second)
        pou::Logger::warning("Multiple skeletons with name \""+skeletonName+"\" in item model:"+m_filePath);

    skelPair.first->second.loadFromXML(element);

    /*auto &limbs = skelPair.first->second;

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
                limbs.push_back({std::string(nodeAtt), spriteModel});
                if(spriteModel == nullptr)
                    pou::Logger::warning("Sprite named \""+std::string(spriteAtt)+"\" not found in spritesheet \""
                                    +std::string(spriteSheetAtt)+"\" in: "+m_filePath);

            } else
                pou::Logger::warning("Spritesheet named \""+std::string(nodeAtt)+"\" not found in: "+m_filePath);
        } else
            pou::Logger::warning("Incomplete limb in item model: "+m_filePath);

        limbChild = limbChild->NextSiblingElement("limb");
    }*/

    return (true);
}

bool ItemModelAsset::loadHitboxes(TiXmlElement *element, std::list<Hitbox> &boxList)
{
    auto boxChild = element->FirstChildElement("box");
    while(boxChild != nullptr)
    {
        boxList.push_back(Hitbox ());

        if(!boxList.back().loadFromXML(boxChild->ToElement()))
            pou::Logger::warning("Incomplete hitbox in item model: "+m_filePath);

        boxChild = boxChild->NextSiblingElement("box");
    }

    return (true);
}


bool ItemModelAsset::loadAttributes(TiXmlElement *element)
{
    auto e = element->FirstChildElement("type");
    if(e != nullptr)
    {
        std::string type(e->GetText());

        if(type == "weapon" || type== "Weapon")
            m_attributes.type = GearType_Weapon;
    }

    e = element->FirstChildElement("attackDelay");
    if(e != nullptr)
        m_attributes.attackDelay = pou::Parser::parseFloat(e->GetText());

    e = element->FirstChildElement("attackDamages");
    if(e != nullptr)
        m_attributes.attackDamages = pou::Parser::parseFloat(e->GetText());

    return (true);
}

