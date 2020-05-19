#include "PouEngine/assets/MaterialAsset.h"

#include "PouEngine/Types.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/TextureAsset.h"

#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Parser.h"

namespace pou
{

MaterialAsset::MaterialAsset() : MaterialAsset(-1)
{
}

MaterialAsset::MaterialAsset(const AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;

    m_heightFactor  = 1;
    m_rmeFactor     = glm::vec3(1.0,0.0,0.0);

    m_albedoMap = nullptr;
    m_normalMap = nullptr;
    m_heightMap = nullptr;
    m_rmeMap    = nullptr;
}

MaterialAsset::~MaterialAsset()
{
    //dtor
}


VTexture MaterialAsset::getAlbedoMap()
{
    if(!m_albedoMap)
        return VTexture();
    return m_albedoMap->getVTexture();
}

VTexture MaterialAsset::getNormalMap()
{
    if(!m_normalMap)
        return VTexture();
    return m_normalMap->getVTexture();
}

VTexture MaterialAsset::getHeightMap()
{
    if(!m_heightMap)
        return VTexture();
    return m_heightMap->getVTexture();
}

VTexture MaterialAsset::getRmeMap()
{
    if(!m_rmeMap)
        return VTexture();
    return m_rmeMap->getVTexture();
}

float MaterialAsset::getHeightFactor()
{
    return m_heightFactor;
}

glm::vec3 MaterialAsset::getRmeFactor()
{
    return m_rmeFactor;
}

glm::vec2 MaterialAsset::getExtent()
{
    if(m_albedoMap == nullptr)
        return {0,0};
    return m_albedoMap->getExtent();
}


bool MaterialAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        Logger::error("Cannot load material from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    return this->loadFromXML(&hdl);
}


bool MaterialAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    if(hdl->FirstChildElement("name").Element() != nullptr)
        m_name = hdl->FirstChildElement("name").Element()->GetText();

    if(hdl->FirstChildElement("height").Element() != nullptr)
        m_heightFactor = Parser::parseFloat(hdl->FirstChildElement("height").Element()->GetText());

    if(hdl->FirstChildElement("roughness").Element() != nullptr)
        m_rmeFactor.r = Parser::parseFloat(hdl->FirstChildElement("roughness").Element()->GetText());

    if(hdl->FirstChildElement("metalness").Element() != nullptr)
        m_rmeFactor.g = Parser::parseFloat(hdl->FirstChildElement("metalness").Element()->GetText());

    if(hdl->FirstChildElement("emissivity").Element() != nullptr)
        m_rmeFactor.b = Parser::parseFloat(hdl->FirstChildElement("emissivity").Element()->GetText());

    TiXmlElement* textElem = hdl->FirstChildElement("texture").Element();
    while(textElem != nullptr)
    {
        if(std::string(textElem->Attribute("type")).compare("albedo") == 0)
        {
            m_albedoMap = TexturesHandler::loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_albedoMap);
            if(!m_albedoMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("normal") == 0)
        {
            m_normalMap = TexturesHandler::loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_normalMap);
            if(!m_normalMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("height") == 0)
        {
            m_heightMap = TexturesHandler::loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_heightMap);
            if(!m_heightMap->isLoaded())
                loaded = false;
        }
        else if(std::string(textElem->Attribute("type")).compare("rme") == 0)
        {
            m_rmeMap = TexturesHandler::loadAssetFromFile(m_fileDirectory+textElem->GetText(),m_loadType);
            this->startListeningTo(m_rmeMap);
            if(!m_rmeMap->isLoaded())
                loaded = false;
        }
        textElem = textElem->NextSiblingElement("texture");
    }

    if(loaded)
        Logger::write("Material loaded from file: "+m_filePath);

    return (loaded);
}


void MaterialAsset::notify(NotificationSender* sender, NotificationType notification,
                           size_t dataSize, char* data)
{
    if(notification == Notification_AssetLoaded)
    if(sender == m_albedoMap || sender == m_heightMap
       || sender == m_normalMap || sender == m_rmeMap)
    {
        if(m_albedoMap == nullptr || m_albedoMap->isLoaded())
        if(m_heightMap == nullptr || m_heightMap->isLoaded())
        if(m_normalMap == nullptr || m_normalMap->isLoaded())
        if(m_rmeMap == nullptr || m_rmeMap->isLoaded())
        {
            m_loaded = true, Asset::loadNow();
            Logger::write("Material loaded from file: "+m_filePath);
        }
    }

    if(notification == Notification_SenderDestroyed)
    {
        if(sender == m_albedoMap)
            m_albedoMap = nullptr;
        else if(sender == m_heightMap)
            m_heightMap = nullptr;
        else if(sender == m_normalMap)
            m_normalMap = nullptr;
        else if(sender == m_rmeMap)
            m_rmeMap = nullptr;
    }
}

}
