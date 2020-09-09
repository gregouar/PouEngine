#include "generators/TerrainGenerator_TileModels.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Hasher.h"

#include "PouEngine/assets/AssetHandler.h"
#include "assets/TerrainLayerModelAsset.h"

#include "character/Character.h"


///
///TerrainGenerator_TileModels
///


TerrainGenerator_TileModels::TerrainGenerator_TileModels() :
    /*m_chosenSpriteMap(0),
    m_totalProbability1(0),
    m_totalProbability2(0)*/
    m_totalSpriteProbability(0),
    m_totalPrefabProbability(0)
{
}

TerrainGenerator_TileModels::~TerrainGenerator_TileModels()
{

}

bool TerrainGenerator_TileModels::loadFromXML(TiXmlElement *element, TerrainLayerModelAsset *parentLayerModel)
{
    auto spriteChild = element->FirstChildElement("sprite");
    while(spriteChild != nullptr)
    {
        auto spriteElement = spriteChild->ToElement();
        this->loadSpriteElement(spriteElement, parentLayerModel);
        spriteChild = spriteChild->NextSiblingElement("sprite");
    }

    auto prefabChild = element->FirstChildElement("prefab");
    while(prefabChild != nullptr)
    {
        auto prefabElement = prefabChild->ToElement();
        this->loadPrefabElement(prefabElement, parentLayerModel);
        prefabChild = prefabChild->NextSiblingElement("prefab");
    }

    return (true);
}

void TerrainGenerator_TileModels::addTileSpriteModel(float probability, TerrainGenerator_TileSpriteModel tileModel)
{
    if(probability <= 0)
        return;

    /**auto *totalProbability = m_chosenSpriteMap ? &m_totalProbability1 : &m_totalProbability2;
    auto *spriteMap = m_chosenSpriteMap ? &m_spriteMap1 : &m_spriteMap2;

    *totalProbability += probability;
    spriteMap->insert({*totalProbability, tileModel});

    m_chosenSpriteMap = !m_chosenSpriteMap;**/

    m_totalSpriteProbability += probability;
    m_spriteMap.insert({m_totalSpriteProbability, tileModel});
}

void TerrainGenerator_TileModels::addTilePrefabModel(float probability, TerrainGenerator_TilePrefabModel tileModel)
{
    if(probability <= 0)
        return;

    m_totalPrefabProbability += probability;
    m_prefabMap.insert({m_totalPrefabProbability, tileModel});
}


void TerrainGenerator_TileModels::generatesOnNode(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                                                  int modValue, pou::RNGenerator *rng)
{
    auto sprite = this->generateSprite(modValue, rng);
    if(sprite)
        targetNode->attachObject(sprite);

    auto prefabInstance = this->generatePrefab(modValue, rng);
    if(prefabInstance)
    {
        targetNode->addChildNode(prefabInstance);
        auto characterList = prefabInstance->generateCharactersAsChilds();
        for(auto character : characterList)
            character->setSyncData(syncComponent, 0);
        /*auto prefabNode = targetNode->addChildNode(prefabInstance);
        auto characterList = prefabInstance->generateCharacters(prefabNode->transform());
        for(auto character : characterList)
            targetNode->addChildNode(character);*/
    }
}



///
///Protected
///

void TerrainGenerator_TileModels::loadSpriteElement(TiXmlElement *spriteElement, TerrainLayerModelAsset *parentLayerModel)
{
    auto probabilityAtt = spriteElement->Attribute("probability");
    auto spriteAtt      = spriteElement->Attribute("sprite");
    auto spriteSheetAtt = spriteElement->Attribute("spritesheet");

    if(!spriteAtt || !spriteSheetAtt || !probabilityAtt)
    {
        pou::Logger::warning("Incomplete tile model in ground layer: "+parentLayerModel->getFilePath());
        return;
    }

    auto hashedSpriteSheet = pou::Hasher::unique_hash(spriteSheetAtt);
    auto hashedSprite = pou::Hasher::unique_hash(spriteAtt);

    auto spriteModel = parentLayerModel->getSpriteModel(hashedSpriteSheet, hashedSprite);
    if(!spriteModel)
        return;

    auto probability = pou::Parser::parseFloat(std::string(probabilityAtt));

    auto sprite = std::make_shared<pou::SpriteEntity>();
    sprite->setSpriteModel(spriteModel);
    ///this->addTileModel(probability, sprite);

    auto rotationAtt = spriteElement->Attribute("rotation");
    if(rotationAtt)
    {
        auto rotation = pou::Parser::parseFloat(std::string(rotationAtt));
        sprite->setRotation(rotation, false);
    }

    bool allowFlipX = false;
    bool allowFlipY = false;

    auto flipXAtt = spriteElement->Attribute("allowRandomFlipX");
    if(flipXAtt)
        allowFlipX = pou::Parser::parseBool(std::string(flipXAtt));

    auto flipYAtt = spriteElement->Attribute("allowRandomFlipY");
    if(flipYAtt)
        allowFlipY = pou::Parser::parseBool(std::string(flipYAtt));

    TerrainGenerator_TileSpriteModel tileModel;
    tileModel.allowFlipX = allowFlipX;
    tileModel.allowFlipY = allowFlipY;
    tileModel.spriteModel = sprite;
    this->addTileSpriteModel(probability, tileModel);
}


void TerrainGenerator_TileModels::loadPrefabElement(TiXmlElement *prefabElement, TerrainLayerModelAsset *parentLayerModel)
{
    auto probabilityAtt = prefabElement->Attribute("probability");
    auto pathAtt        = prefabElement->Attribute("path");

    if(!pathAtt || !probabilityAtt)
    {
        pou::Logger::warning("Incomplete tile model in ground layer: "+parentLayerModel->getFilePath());
        return;
    }

    auto prefabAsset =  PrefabsHandler::loadAssetFromFile(parentLayerModel->getFileDirectory()+pathAtt);
    auto probability = pou::Parser::parseFloat(std::string(probabilityAtt));

    float rotation = 0;

    auto rotationAtt = prefabElement->Attribute("rotation");
    if(rotationAtt)
        rotation = pou::Parser::parseFloat(std::string(rotationAtt));

    bool allowFlipX = false;
    bool allowFlipY = false;

    auto flipXAtt = prefabElement->Attribute("allowRandomFlipX");
    if(flipXAtt)
        allowFlipX = pou::Parser::parseBool(std::string(flipXAtt));

    auto flipYAtt = prefabElement->Attribute("allowRandomFlipY");
    if(flipYAtt)
        allowFlipY = pou::Parser::parseBool(std::string(flipYAtt));

    TerrainGenerator_TilePrefabModel tileModel;
    tileModel.prefabAsset = prefabAsset;
    tileModel.allowFlipX = allowFlipX;
    tileModel.allowFlipY = allowFlipY;
    tileModel.rotation = rotation;
    this->addTilePrefabModel(probability, tileModel);
}


std::shared_ptr<pou::SpriteEntity> TerrainGenerator_TileModels::generateSprite(int modValue, pou::RNGenerator *rng)
{
    /**bool chosen = (modValue % 2);

    auto totalProbability = chosen ? m_totalProbability1 : m_totalProbability2;
    auto *spriteMap = chosen ? &m_spriteMap1 : &m_spriteMap2;


    if(spriteMap->empty())
    {
        totalProbability = chosen ? m_totalProbability2 : m_totalProbability1;
        spriteMap = chosen ? &m_spriteMap2 : &m_spriteMap1;

        if(spriteMap->empty())
            return (nullptr);
    }**/

    auto totalProbability = m_totalSpriteProbability*0.5f;

    float randValue = pou::RNGesus::uniformFloat(0, totalProbability, rng);

    ///Dithering
    if(modValue % 2)
        randValue += m_totalSpriteProbability*0.5f;

    auto tileModelIt = m_spriteMap.lower_bound(randValue);
    if(tileModelIt == m_spriteMap.end())
        return (nullptr);


    auto &tileModel = tileModelIt->second;

    ///auto createdSprite = std::dynamic_pointer_cast<WorldSprite>(tileModel.spriteModel->createCopy());
    auto createdSprite = std::dynamic_pointer_cast<pou::SpriteEntity>(tileModel.spriteModel->createCopy());

    if(tileModel.allowFlipX)
    {
        float randValue = pou::RNGesus::uniformFloat(0.0f, 1.0f, rng);
        if(randValue > .5f)
            createdSprite->flip(1,0);
    }

    if(tileModel.allowFlipY)
    {
        float randValue = pou::RNGesus::uniformFloat(0.0f, 1.0f, rng);
        if(randValue > .5f)
            createdSprite->flip(0,1);
    }

    return createdSprite;
}


std::shared_ptr<PrefabInstance> TerrainGenerator_TileModels::generatePrefab(int modValue, pou::RNGenerator *rng)
{
    auto totalProbability = m_totalPrefabProbability*0.5f;

    float randValue = pou::RNGesus::uniformFloat(0, totalProbability, rng);

    ///Dithering
    if(modValue % 2)
        randValue += m_totalPrefabProbability*0.5f;

    auto tileModelIt = m_prefabMap.lower_bound(randValue);
    if(tileModelIt == m_prefabMap.end())
        return (nullptr);


    auto &tileModel = tileModelIt->second;

    auto prefabInstance = tileModel.prefabAsset->generate();

    if(tileModel.allowFlipX)
    {
        float randValue = pou::RNGesus::uniformFloat(0.0f, 1.0f, rng);
        if(randValue > .5f)
            prefabInstance->transform()->scale({1,0,0});
    }

    if(tileModel.allowFlipY)
    {
        float randValue = pou::RNGesus::uniformFloat(0.0f, 1.0f, rng);
        if(randValue > .5f)
            prefabInstance->transform()->scale({0,1,0});
    }

    prefabInstance->transform()->rotateInDegrees({0,0,tileModel.rotation});

    return prefabInstance;
}

