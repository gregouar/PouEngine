#include "PouEngine/assets/SpriteSheetAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{


SpriteSheetAsset::SpriteSheetAsset() : SpriteSheetAsset(-1)
{

}

SpriteSheetAsset::SpriteSheetAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;

    m_texture = nullptr;
}

SpriteSheetAsset::~SpriteSheetAsset()
{
    //dtor
}


bool SpriteSheetAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        Logger::error("Cannot load sprite sheet from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool SpriteSheetAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    if(hdl->Element()->Attribute("texture") == nullptr)
        return (false);

    std::string textureName = std::string(hdl->Element()->Attribute("texture"));

    m_texture = TexturesHandler::instance()
                    ->loadAssetFromFile(m_fileDirectory+textureName,m_loadType);
    this->startListeningTo(m_texture);
    if(!m_texture->isLoaded())
        loaded = false;


    TiXmlElement* spriteElement = hdl->FirstChildElement("sprite").Element();
    int i = 0;
    while(spriteElement != nullptr)
    {
        std::unique_ptr<SpriteModel> spriteModel(new SpriteModel());

        std::string spriteName = std::to_string(i);
        glm::vec2 spriteSize(0,0);
        glm::vec2 spriteCenter(0,0);
        glm::vec2 spritePosition(0,0);
        bool customSize = false;
        bool customCenter = false;
        bool customPosition = false;

        if(spriteElement->Attribute("name") != nullptr)
            spriteName = std::string(spriteElement->Attribute("name"));

        auto sizeElement = spriteElement->FirstChildElement("size");
        if(sizeElement != nullptr)
        {
            if(sizeElement->Attribute("x") != nullptr)
                spriteSize.x = Parser::parseFloat(std::string(sizeElement->Attribute("x")));
            if(sizeElement->Attribute("y") != nullptr)
                spriteSize.y = Parser::parseFloat(std::string(sizeElement->Attribute("y")));

            customSize = true;
        }

        auto centerElement = spriteElement->FirstChildElement("center");
        if(centerElement != nullptr)
        {

            if(centerElement->Attribute("x") != nullptr)
                spriteCenter.x = Parser::parseFloat(std::string(centerElement->Attribute("x")));
            if(centerElement->Attribute("y") != nullptr)
                spriteCenter.y = Parser::parseFloat(std::string(centerElement->Attribute("y")));

            customCenter = true;
        }

        auto positionElement = spriteElement->FirstChildElement("position");
        if(positionElement != nullptr)
        {
            if(positionElement->Attribute("x") != nullptr)
                spritePosition.x = Parser::parseFloat(std::string(positionElement->Attribute("x")));
            if(positionElement->Attribute("y") != nullptr)
                spritePosition.y = Parser::parseFloat(std::string(positionElement->Attribute("y")));

            customPosition = true;
        }


        spriteModel->setTexture(m_texture);

        if(!customSize)
            spriteSize = m_texture->getExtent();
        if(!customCenter)
            spriteCenter = {spriteSize.x*0.5,
                            spriteSize.y*0.5};

        spriteModel->setSize(spriteSize);
        spriteModel->setCenter(spriteCenter);

        if(customPosition)
            spriteModel->setTextureRect(spritePosition,spriteSize,false);

        m_sprites.insert(std::make_pair(spriteName,std::move(spriteModel)));

        spriteElement = spriteElement->NextSiblingElement("sprite");
        ++i;
    }

    /*if(hdl->FirstChildElement("name").Element() != nullptr)
        m_name = hdl->FirstChildElement("name").Element()->GetText();

    if(hdl->FirstChildElement("height").Element() != nullptr)
        m_heightFactor = Parser::parseFloat(hdl->FirstChildElement("height").Element()->GetText());

    if(hdl->FirstChildElement("roughness").Element() != nullptr)
        m_rmtFactor.r = Parser::parseFloat(hdl->FirstChildElement("roughness").Element()->GetText());

    if(hdl->FirstChildElement("metalness").Element() != nullptr)
        m_rmtFactor.g = Parser::parseFloat(hdl->FirstChildElement("metalness").Element()->GetText());

    if(hdl->FirstChildElement("translucency").Element() != nullptr)
        m_rmtFactor.b = Parser::parseFloat(hdl->FirstChildElement("translucency").Element()->GetText());

    TiXmlElement* textElem = hdl->FirstChildElement("texture").Element();
    while(textElem != nullptr)
    {
        if(std::string(textElem->Attribute("type")).compare("albedo") == 0)
        {
            m_albedoMap = TexturesHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_albedoMap);
            if(!m_albedoMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("normal") == 0)
        {
            m_normalMap = TexturesHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_normalMap);
            if(!m_normalMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("height") == 0)
        {
            m_heightMap = TexturesHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_heightMap);
            if(!m_heightMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("rmt") == 0)
        {
            m_rmtMap = TexturesHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_rmtMap);
            if(!m_rmtMap->isLoaded())
                loaded = false;
        }
        textElem = textElem->NextSiblingElement("texture");
    }*/

    if(loaded)
        Logger::write("Sprite sheet loaded from file: "+m_filePath);

    return (loaded);
}


SpriteModel* SpriteSheetAsset::getSpriteModel(const std::string &spriteName)
{
    auto spriteModel = m_sprites.find(spriteName);
    return spriteModel->second.get();
}


void SpriteSheetAsset::notify(NotificationSender* sender, NotificationType notification,
                           size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded)
        if(sender == m_texture)
        {
            m_loaded = true, Asset::loadNow();
            Logger::write("Sprite sheet loaded from file: "+m_filePath);
        }

    if(notification == Notification_SenderDestroyed)
    {
        if(sender == m_texture)
            m_texture = nullptr;
    }
}

}
