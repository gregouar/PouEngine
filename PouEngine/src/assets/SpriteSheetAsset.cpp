#include "PouEngine/assets/SpriteSheetAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"
#include "PouEngine/assets/MaterialAsset.h"

#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Parser.h"

namespace pou
{


SpriteSheetAsset::SpriteSheetAsset() : SpriteSheetAsset(-1)
{

}

SpriteSheetAsset::SpriteSheetAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;

    m_texture   = nullptr;
    m_material  = nullptr;
    m_useMaterial = false;
    m_textureScale = glm::vec2(1.0f,1.0f);

    m_waitingForTextureLoading = false;
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

    std::string textureName;

    auto materialAtt = hdl->Element()->Attribute("material");
    if(materialAtt != nullptr)
    {
        m_useMaterial = true;
        textureName = std::string(materialAtt);
    } else {
        auto textureAtt = hdl->Element()->Attribute("texture");
        if(textureAtt == nullptr)
            return (false);
        textureName = std::string(textureAtt);
    }

    auto scaleXAtt = hdl->Element()->Attribute("scaleX");
    if(scaleXAtt != nullptr)
        m_textureScale.x = Parser::parseFloat(scaleXAtt);

    auto scaleYAtt = hdl->Element()->Attribute("scaleY");
    if(scaleYAtt != nullptr)
        m_textureScale.y = Parser::parseFloat(scaleYAtt);

    if(!m_waitingForTextureLoading)
    {
        if(m_useMaterial)
        {
            m_material = MaterialsHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textureName,m_loadType);
            this->startListeningTo(m_material);
        } else {
            m_texture = TexturesHandler::instance()
                            ->loadAssetFromFile(m_fileDirectory+textureName,LoadType_InThread/**m_loadType**/);
            this->startListeningTo(m_texture);
        }
    }

    if(!m_waitingForTextureLoading)
    {
        TiXmlElement* spriteElement = hdl->FirstChildElement("sprite").Element();
        int i = 0;
        while(spriteElement != nullptr)
        {
            std::unique_ptr<SpriteModel> spriteModel(new SpriteModel(this));

            std::string spriteName = std::to_string(i);
            glm::vec2 spriteSize(0,0);
            glm::vec2 spriteCenter(0,0);
            glm::vec2 spritePosition(0,0);
           // bool customSize = false;
            bool customCenter = false;
            bool customPosition = false;
            glm::vec3 spriteRme(1.0,0.0,0.0);
            bool customRme = false;

            auto nameElement = spriteElement->Attribute("name");
            if(nameElement != nullptr)
                spriteName = std::string(nameElement);

            auto castShadowElement = spriteElement->Attribute("castShadow");
            if(castShadowElement != nullptr)
            {
                bool castShadow = Parser::parseBool(castShadowElement);
                if(castShadow)
                    spriteModel->setShadowCastingType(ShadowCasting_OnlyDirectional);
                else
                    spriteModel->setShadowCastingType(ShadowCasting_None);
            }

            auto sizeElement = spriteElement->FirstChildElement("size");
            if(sizeElement != nullptr)
            {
                if(sizeElement->Attribute("x") != nullptr)
                    spriteSize.x = Parser::parseFloat(std::string(sizeElement->Attribute("x")));
                if(sizeElement->Attribute("y") != nullptr)
                    spriteSize.y = Parser::parseFloat(std::string(sizeElement->Attribute("y")));

               // customSize = true;
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

            auto surfaceElement = spriteElement->FirstChildElement("surface");
            if(surfaceElement != nullptr)
            {
                if(surfaceElement->Attribute("roughness") != nullptr)
                    spriteRme.x = Parser::parseFloat(std::string(surfaceElement->Attribute("roughness")));
                if(surfaceElement->Attribute("metalness") != nullptr)
                    spriteRme.y = Parser::parseFloat(std::string(surfaceElement->Attribute("metalness")));
                if(surfaceElement->Attribute("emissivity") != nullptr)
                    spriteRme.z = Parser::parseFloat(std::string(surfaceElement->Attribute("emissivity")));
                if(surfaceElement->Attribute("r") != nullptr)
                    spriteRme.x = Parser::parseFloat(std::string(surfaceElement->Attribute("r")));
                if(surfaceElement->Attribute("m") != nullptr)
                    spriteRme.y = Parser::parseFloat(std::string(surfaceElement->Attribute("m")));
                if(surfaceElement->Attribute("e") != nullptr)
                    spriteRme.z = Parser::parseFloat(std::string(surfaceElement->Attribute("e")));
                customRme = true;
            }


            auto nextSpriteElement = spriteElement->FirstChildElement("nextSprite");
            if(nextSpriteElement != nullptr)
            {
                float delay = -1;
                int nextSprite = -1;
                if(nextSpriteElement->Attribute("delay") != nullptr)
                    delay = Parser::parseFloat(std::string(nextSpriteElement->Attribute("delay")));
                if(nextSpriteElement->Attribute("name") != nullptr)
                    nextSprite = this->generateSpriteId(std::string(nextSpriteElement->Attribute("name")));

                if(nextSprite != -1 && delay != -1)
                    spriteModel->setNextSprite(nextSprite, delay);
            }

            if(m_useMaterial)
                spriteModel->setMaterial(m_material);
            else
                spriteModel->setTexture(m_texture);

            /*if(!customSize && m_texture->isLoaded())
                spriteSize = m_texture->getExtent();*/

            if(!customCenter)
                spriteCenter = {spriteSize.x*0.5,
                                spriteSize.y*0.5};

            spriteModel->setSize(spriteSize*m_textureScale);
            spriteModel->setCenter(spriteCenter*m_textureScale);

            if(customRme)
                spriteModel->setRme(spriteRme);

            if(customPosition)
                spriteModel->setTextureRect(spritePosition /* /m_textureScale */,spriteSize /*/m_textureScale*/,false);

            int spriteId = this->generateSpriteId(spriteName);
            spriteModel->setSpriteId(spriteId);

            if(!m_spritesById.insert({spriteId, std::move(spriteModel)}).second)
                  Logger::warning("Multiple sprites named \""+spriteName+"\" in the sprite sheet : "+m_filePath);


            spriteElement = spriteElement->NextSiblingElement("sprite");
            ++i;
        }
    }
    if(m_useMaterial && ! m_material->isLoaded())
    {
        loaded = false;
        m_waitingForTextureLoading = true;
    }
    else if(!m_useMaterial && !m_texture->isLoaded())
    {
        loaded = false;
        m_waitingForTextureLoading = true;
    }

    if(loaded)
    {
        Logger::write("Sprite sheet loaded from file: "+m_filePath);
        m_waitingForTextureLoading = false;
    }

    return (loaded);
}


SpriteModel* SpriteSheetAsset::getSpriteModel(const std::string &spriteName)
{
    auto founded = m_spritesIdByName.find(spriteName);
    if(founded == m_spritesIdByName.end())
    {
        pou::Logger::warning("Sprite named \""+spriteName+"\" not found in spritesheet:"+m_filePath);
        return (nullptr);
    }

    return this->getSpriteModel(founded->second);


    /*auto spriteModel = m_sprites.find(spriteName);

    if(spriteModel == m_sprites.end())
        return (nullptr);

    return spriteModel->second.get();*/
}


SpriteModel* SpriteSheetAsset::getSpriteModel(int spriteId)
{
    auto founded = m_spritesById.find(spriteId);
    if(founded == m_spritesById.end())
        return (nullptr);

    return founded->second.get();
}

int SpriteSheetAsset::getSpriteId(const std::string &spriteName) const
{
    auto founded = m_spritesIdByName.find(spriteName);
    if(founded == m_spritesIdByName.end())
        return (-1);

    return founded->second;
}

int SpriteSheetAsset::generateSpriteId(const std::string &spriteName)
{
    int id = 0;
    if(!m_spritesIdByName.empty())
    {
        auto founded = m_spritesIdByName.find(spriteName);
        if(founded != m_spritesIdByName.end())
            return founded->second;
    }

    id = m_spritesIdByName.size()+1;//(--m_soundIdByName.end())->second++;
    m_spritesIdByName.insert({spriteName,id});
    return id;
}




void SpriteSheetAsset::notify(NotificationSender* sender, int notificationType,
                              void* data)
{
    if(notificationType == NotificationType_AssetLoaded)
        if(sender == m_texture)
        {
            m_loaded = true, Asset::loadNow();
            Logger::write("Sprite sheet loaded from file: "+m_filePath);
        }

    if(notificationType == NotificationType_SenderDestroyed)
    {
        if(sender == m_texture)
            m_texture = nullptr;
    }
}

}
