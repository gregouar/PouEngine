#include "assets/TerrainLayerModelAsset.h"

#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/tools/Logger.h"


TerrainLayerModelAsset::TerrainLayerModelAsset() :
    TerrainLayerModelAsset(-1)
{
    //ctor
}


TerrainLayerModelAsset::TerrainLayerModelAsset(const pou::AssetTypeId id) : Asset(id)
{
    m_allowLoadFromFile     = true;
    m_allowLoadFromMemory   = false;
}

TerrainLayerModelAsset::~TerrainLayerModelAsset()
{
    //dtor
}


bool TerrainLayerModelAsset::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load layer model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    m_filePath = filePath;

    return this->loadFromXML(&hdl);
}

//pou::SpriteSheetAsset *TerrainLayerModelAsset::getSpriteSheet(const std::string &spriteSheetName)
pou::SpriteModel *TerrainLayerModelAsset::getSpriteModel(const std::string &spriteSheetName, const std::string &spriteName)
{
    auto spritesheetIt = m_spriteSheets.find(spriteSheetName);

    if(spritesheetIt == m_spriteSheets.end())
    {
        pou::Logger::warning("Spritesheet named \""+spriteSheetName+"\" not found");
        return (nullptr);
    }

    auto spriteModel = spritesheetIt->second->getSpriteModel(spriteName);
    return spriteModel;
}

/*float TerrainLayerModelAsset::getSpawnPointSparsity()
{
    return m_spawnPointSparsity;
}
float TerrainLayerModelAsset::getSpawnProbability()
{
    return m_spawnProbability;
}
float TerrainLayerModelAsset::getExpandProbability()
{
    return m_expandProbability;
}*/

///
///Protected
///

bool TerrainLayerModelAsset::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    element = hdl->FirstChildElement("spritesheet").Element();
    while(element != nullptr)
    {
        if(!this->loadSpriteSheet(element))
            loaded = false;
        element = element->NextSiblingElement("spritesheet");
    }

    element = hdl->FirstChildElement("tileModel").Element();
    while(element != nullptr)
    {
        if(!this->loadTileModels(element))
            loaded = false;
        element = element->NextSiblingElement("tileModel");
    }

    return loaded;
}

bool TerrainLayerModelAsset::loadSpriteSheet(TiXmlElement *element)
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
        pou::Logger::warning("Multiple spritesheets with name \""+spriteSheetName+"\" in tile models:"+m_filePath);

    return (true);
}

bool TerrainLayerModelAsset::loadTileModels(TiXmlElement *element)
{
    auto typeAtt = element->Attribute("type");
    if(typeAtt == nullptr)
        return (false);

    int type = 0;

    auto typeString = std::string(typeAtt);

    if(typeString == "blank")
        type = TerrainGenerator_BorderType_Blank;
    else if(typeString == "smallCornerTopLeft")
        type = TerrainGenerator_BorderType_SmallCorner_TL;
    else if(typeString == "smallCornerTopRight")
        type = TerrainGenerator_BorderType_SmallCorner_TR;
    else if(typeString == "sideTop")
        type = TerrainGenerator_BorderType_Side_T;
    else if(typeString == "smallCornerBottomLeft")
        type = TerrainGenerator_BorderType_SmallCorner_BL;
    else if(typeString == "sideLeft")
        type = TerrainGenerator_BorderType_Side_L;
    else if(typeString == "diagonalBottomLeftToTopRight")
        type = TerrainGenerator_BorderType_Diag_BL_TR;
    else if(typeString == "bigCornerBottomRight")
        type = TerrainGenerator_BorderType_BigCorner_BR;
    else if(typeString == "smallCornerBottomRight")
        type = TerrainGenerator_BorderType_SmallCorner_BR;
    else if(typeString == "diagonalTopLeftToBottomRight")
        type = TerrainGenerator_BorderType_Diag_TL_BR;
    else if(typeString == "sideRight")
        type = TerrainGenerator_BorderType_Side_R;
    else if(typeString == "bigCornerBottomLeft")
        type = TerrainGenerator_BorderType_BigCorner_BL;
    else if(typeString == "sideBottom")
        type = TerrainGenerator_BorderType_Side_B;
    else if(typeString == "bigCornerTopRight")
        type = TerrainGenerator_BorderType_BigCorner_TR;
    else if(typeString == "bigCornerTopLeft")
        type = TerrainGenerator_BorderType_BigCorner_TL;
    else if(typeString == "fill")
        type = TerrainGenerator_BorderType_Fill;

    return m_tileModels[type].loadFromXML(element, this);
}


std::shared_ptr<WorldSprite> TerrainLayerModelAsset::generateSprite(TerrainGenerator_BorderType borderType, pou::RNGenerator *rng)
{
    if(borderType == NBR_BORDER_TYPES)
        return (nullptr);

    return m_tileModels[borderType].generateSprite(rng);
}


