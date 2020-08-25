#include "generators/TerrainGenerator_TileModels.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Hasher.h"

#include "assets/TerrainLayerModelAsset.h"


///
///TerrainGenerator_TileModels
///


TerrainGenerator_TileModels::TerrainGenerator_TileModels() :
    m_chosenSpriteMap(0),
    m_totalProbability1(0),
    m_totalProbability2(0)
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

        auto probabilityAtt = spriteElement->Attribute("probability");
        auto spriteAtt      = spriteElement->Attribute("sprite");
        auto spriteSheetAtt = spriteElement->Attribute("spritesheet");

        if(!spriteAtt || !spriteSheetAtt || !probabilityAtt)
        {
            pou::Logger::warning("Incomplete tile model in ground layer: "+parentLayerModel->getFilePath());
            continue;
        }

        auto hashedSpriteSheet = pou::Hasher::unique_hash(spriteSheetAtt);
        auto hashedSprite = pou::Hasher::unique_hash(spriteAtt);

        auto spriteModel = parentLayerModel->getSpriteModel(hashedSpriteSheet, hashedSprite);
        if(!spriteModel)
            continue;

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

        /**int nbrSpriteVariants = 1 + allowFlipX + allowFlipY + (allowFlipX & allowFlipY);

        if(allowFlipX)
        {
            auto spriteCopy = std::dynamic_pointer_cast<WorldSprite>(sprite->createCopy());
            spriteCopy->setFlip(1,0);
            this->addTileModel(probability/nbrSpriteVariants, spriteCopy);
        }
        if(allowFlipY)
        {
            auto spriteCopy = std::dynamic_pointer_cast<WorldSprite>(sprite->createCopy());
            spriteCopy->setFlip(0,1);
            this->addTileModel(probability/nbrSpriteVariants, spriteCopy);
        }
        if(allowFlipX && allowFlipY)
        {
            auto spriteCopy = std::dynamic_pointer_cast<WorldSprite>(sprite->createCopy());
            spriteCopy->setFlip(1,1);
            this->addTileModel(probability/nbrSpriteVariants, spriteCopy);
        }

        this->addTileModel(probability/nbrSpriteVariants, sprite);**/

        TerrainGenerator_TileModel tileModel;
        tileModel.allowFlipX = allowFlipX;
        tileModel.allowFlipY = allowFlipY;
        tileModel.spriteModel = sprite;
        this->addTileModel(probability, tileModel);

        spriteChild = spriteChild->NextSiblingElement("sprite");
    }

    return (true);
}

void TerrainGenerator_TileModels::addTileModel(float probability, TerrainGenerator_TileModel tileModel)
{
    if(probability <= 0)
        return;

    auto *totalProbability = m_chosenSpriteMap ? &m_totalProbability1 : &m_totalProbability2;
    auto *spriteMap = m_chosenSpriteMap ? &m_spriteMap1 : &m_spriteMap2;

    /*if(chosenSpriteMap)
    {
        m_totalProbability1 += probability;
        m_spriteMap1.insert({m_totalProbability1, tileModel});
    } else {
        m_totalProbability2 += probability;
        m_spriteMap2.insert({m_totalProbability2, tileModel});
    }*/
    *totalProbability += probability;
    spriteMap->insert({*totalProbability, tileModel});

    m_chosenSpriteMap = !m_chosenSpriteMap;
}

std::shared_ptr<pou::SpriteEntity> TerrainGenerator_TileModels::generateSprite(int modValue, pou::RNGenerator *rng)
{
    bool chosen = (modValue % 2);

    auto totalProbability = chosen ? m_totalProbability1 : m_totalProbability2;
    auto *spriteMap = chosen ? &m_spriteMap1 : &m_spriteMap2;

    if(spriteMap->empty())
    {
        totalProbability = chosen ? m_totalProbability2 : m_totalProbability1;
        spriteMap = chosen ? &m_spriteMap2 : &m_spriteMap1;

        if(spriteMap->empty())
            return (nullptr);
    }

    float randValue = pou::RNGesus::uniformFloat(0, totalProbability, rng);
    auto spriteModelIt = spriteMap->lower_bound(randValue);
    if(spriteModelIt == spriteMap->end())
        return (nullptr);


    auto &tileModel = spriteModelIt->second;

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


///
///Protected
///




