#include "generators/TerrainGenerator_TileModels.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"

#include "assets/TerrainLayerModelAsset.h"


///
///TerrainGenerator_TileModels
///


TerrainGenerator_TileModels::TerrainGenerator_TileModels() :
    m_totalProbability(0)
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

        auto spriteModel = parentLayerModel->getSpriteModel(std::string(spriteSheetAtt), std::string(spriteAtt));
        if(!spriteModel)
            continue;

        auto probability = pou::Parser::parseFloat(std::string(probabilityAtt));

        auto sprite = std::make_shared<WorldSprite>();
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

        int nbrSpriteVariants = 1 + allowFlipX + allowFlipY + (allowFlipX & allowFlipY);

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

        this->addTileModel(probability/nbrSpriteVariants, sprite);

        spriteChild = spriteChild->NextSiblingElement("sprite");
    }

    return (true);
}

void TerrainGenerator_TileModels::addTileModel(float probability, std::shared_ptr<WorldSprite> spriteModel)
{
    if(probability <= 0)
        return;

    m_totalProbability += probability;
    m_spriteMap.insert({m_totalProbability, spriteModel});
}

std::shared_ptr<WorldSprite> TerrainGenerator_TileModels::generateSprite(pou::RNGenerator *rng)
{
    if(m_spriteMap.empty())
        return (nullptr);

    float randValue = pou::RNGesus::uniformFloat(0, m_totalProbability, rng);///glm::linearRand(0.0f, m_totalProbability);
    auto spriteModelIt = m_spriteMap.lower_bound(randValue);
    if(spriteModelIt == m_spriteMap.end())
        return (nullptr);

    return std::dynamic_pointer_cast<WorldSprite>(spriteModelIt->second->createCopy());
}


///
///Protected
///




