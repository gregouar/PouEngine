#include "generators/WorldGenerator_SpawnPoint.h"

#include "PouEngine/assets/AssetHandler.h"
#include "assets/CharacterModelAsset.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


WorldGenerator_SpawnPoint_Modifier::WorldGenerator_SpawnPoint_Modifier()
{
    randomType = WorldGenerator_RandomType_None;
    minValue = glm::vec4(0);
    maxValue = glm::vec4(0);
    for(int i = 0 ; i < 4 ; ++i)
        usePrecedingValue[i] = -1;
}


///
///WorldGenerator_SpawnPoint
///

WorldGenerator_SpawnPoint::WorldGenerator_SpawnPoint() :  m_spawnProbability(0), m_groundLayer(nullptr)
{
    //ctor
}

WorldGenerator_SpawnPoint::~WorldGenerator_SpawnPoint()
{
    //dtor
}

bool WorldGenerator_SpawnPoint::loadFromXML(const std::string &fileDirectory, TiXmlElement *element, TerrainGenerator *terrainGenerator)
{
    auto groundLayerAtt = element->Attribute("groundLayer");
    auto spawnProbabilityAtt = element->Attribute("spawnProbability");

    if(groundLayerAtt)
        m_groundLayer = terrainGenerator->getGroundLayer(groundLayerAtt);

    if(spawnProbabilityAtt)
        m_spawnProbability = pou::Parser::parseFloat(spawnProbabilityAtt);

    auto modifierChild = element->FirstChildElement("modifier");
    while(modifierChild != nullptr)
    {
        auto modifierElement = modifierChild->ToElement();

        auto typeAtt = modifierElement->Attribute("type");
        if(typeAtt)
        {
            int type = 0;
            if(std::string(typeAtt) == "position")
                type = WorldGenerator_SpawnPoint_ModifierType_Position;
            else if(std::string(typeAtt) == "rotation")
                type = WorldGenerator_SpawnPoint_ModifierType_Rotation;
            else if(std::string(typeAtt) == "flip")
                type = WorldGenerator_SpawnPoint_ModifierType_Flip;
            else if(std::string(typeAtt) == "color")
                type = WorldGenerator_SpawnPoint_ModifierType_Color;

            auto &randomModifier = m_randomModifiers[type];

            auto rngAtt = modifierElement->Attribute("rng");
            if(rngAtt)
            {
                if(std::string(rngAtt) == "uniform")
                    randomModifier.randomType = WorldGenerator_RandomType_Uniform;
                else if(std::string(rngAtt) == "gaussian")
                    randomModifier.randomType = WorldGenerator_RandomType_Gaussian;
            }

            for(int i = 0 ; i < 4 ; ++i)
                this->loadRandomModifierValue(modifierElement, i, randomModifier);
        }

        modifierChild = modifierChild->NextSiblingElement("modifier");
    }


    auto characterChild = element->FirstChildElement("character");
    while(characterChild != nullptr)
    {
        auto characterElement = characterChild->ToElement();
        characterChild = characterChild->NextSiblingElement("character");

        auto pathAtt = characterElement->Attribute("path");
        auto amountAtt = characterElement->Attribute("amount");

        if(!pathAtt)
            continue;

        auto modelAsset = CharacterModelsHandler::loadAssetFromFile(fileDirectory+pathAtt);
        if(!modelAsset)
            continue;

        m_characterSpawnModels.push_back({});
        auto &characterSpawnModel = m_characterSpawnModels.back();
        characterSpawnModel.modelAsset = modelAsset;

        if(amountAtt)
        {
            auto [minV, maxV] = pou::Parser::parseIntSegment(amountAtt);
            characterSpawnModel.minAmount = minV;
            characterSpawnModel.maxAmount = maxV;
        }
    }

    return (true);
}

void WorldGenerator_SpawnPoint::generatesOnNode(glm::vec2 worldPos, WorldNode *targetNode, GameWorld_Sync *syncComponent,
                                                bool generateCharacters, pou::RNGenerator *rng)
{
    if(generateCharacters)
        for(auto characterSpawnModel : m_characterSpawnModels)
        {
            int amount = pou::RNGesus::uniformInt(characterSpawnModel.minAmount,
                                                  characterSpawnModel.maxAmount);

            for(int i = 0 ; i < amount ; ++i)
                this->spawnCharacter(characterSpawnModel.modelAsset, worldPos, targetNode, syncComponent, rng);
        }
}

float WorldGenerator_SpawnPoint::getSpawnProbability(glm::vec2 worldPos, TerrainGenerator *terrain)
{
    auto gridPos = terrain->worldToGridPosition(worldPos);
    if(m_groundLayer && terrain->getGridValue(gridPos.x, gridPos.y) != m_groundLayer)
        return (0);
    return m_spawnProbability;
}

///
///Protected
///

void WorldGenerator_SpawnPoint::loadRandomModifierValue(TiXmlElement *element, int i, WorldGenerator_SpawnPoint_Modifier &modifier)
{
    const char *valueAtt(nullptr);

    if(i == 0)
    {
        valueAtt = element->Attribute("x");
        if(!valueAtt)
            valueAtt = element->Attribute("r");
    } else if(i == 1) {
        valueAtt = element->Attribute("y");
        if(!valueAtt)
            valueAtt = element->Attribute("g");
    } else if(i == 2) {
        valueAtt = element->Attribute("z");
        if(!valueAtt)
            valueAtt = element->Attribute("b");
    } else if(i == 3) {
        valueAtt = element->Attribute("w");
        if(!valueAtt)
            valueAtt = element->Attribute("a");
    }

    if(!valueAtt)
        return;

    auto valueString = std::string(valueAtt);

     if(valueString == "x" || valueString == "r")
        modifier.usePrecedingValue[i] = 0;
    else if(valueString == "y" || valueString == "g")
        modifier.usePrecedingValue[i] = 1;
    else if(valueString == "z" || valueString == "b")
        modifier.usePrecedingValue[i] = 2;
    else if(valueString == "w" || valueString == "a")
        modifier.usePrecedingValue[i] = 3;
    else
    {
        auto [minV, maxV] = pou::Parser::parseFloatSegment(valueString);
        if(minV == maxV)
            minV = -maxV;
        modifier.minValue[i] = minV;
        modifier.maxValue[i] = maxV;
    }

    /*if(pou::Parser::isFloat(valueString))
    {
        modifier.maxValue[i] = pou::Parser::parseFloat(valueString);
        modifier.minValue[i] = -modifier.maxValue[i];
    }
    else {
        if(valueString == "x" || valueString == "r")
            modifier.usePrecedingValue[i] = 0;
        else if(valueString == "y" || valueString == "g")
            modifier.usePrecedingValue[i] = 1;
        else if(valueString == "z" || valueString == "b")
            modifier.usePrecedingValue[i] = 2;
        else if(valueString == "w" || valueString == "a")
            modifier.usePrecedingValue[i] = 3;
        else if(valueString.length() > 1 && valueString[0] == '[')
        {
            auto middleDelimiter = valueString.find(',', 1);
            if(middleDelimiter == std::string::npos)
                return;

            auto rightDelimiter = valueString.find(']', middleDelimiter+1);
            auto leftStr = valueString.substr(1, middleDelimiter-1);
            auto rightStr = valueString.substr(middleDelimiter+1, rightDelimiter-middleDelimiter-1);

            modifier.minValue[i] = pou::Parser::parseFloat(leftStr);
            modifier.maxValue[i] = pou::Parser::parseFloat(rightStr);
        }
    }*/
}

void WorldGenerator_SpawnPoint::spawnCharacter(CharacterModelAsset *characterModel, glm::vec2 worldPos,
                                                WorldNode *targetNode, GameWorld_Sync *syncComponent,
                                                pou::RNGenerator *rng)
{
    auto character = std::make_shared<Character>();
    character->createFromModel(characterModel);
    character->setPosition(worldPos);
    this->applyRandomModifiers(character.get(), rng);

    targetNode->addChildNode(character);
    syncComponent->syncElement(character);
    syncComponent->syncElement(characterModel);
}

glm::vec4 WorldGenerator_SpawnPoint::generateRandomValue(WorldGenerator_SpawnPoint_Modifier &modifier, pou::RNGenerator *rng)
{
    glm::vec4 randomValue(0);

    for(int i = 0 ; i < 4 ; ++i)
    {
        if(modifier.usePrecedingValue[i] != -1)
        {
            randomValue[i] = randomValue[modifier.usePrecedingValue[i]];
            continue;
        }

        if(modifier.randomType == WorldGenerator_RandomType_Uniform)
            randomValue[i] = pou::RNGesus::uniformFloat(modifier.minValue[i], modifier.maxValue[i], rng);
    }

    return randomValue;
}

void WorldGenerator_SpawnPoint::applyRandomModifiers(WorldNode *targetNode, pou::RNGenerator *rng)
{
    if(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Position].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Position], rng);
        targetNode->move(glm::vec3(v));
    }

    if(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Rotation].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Rotation], rng);
        targetNode->setRotation(v,false);
    }

    if(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Flip].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Flip], rng);
        auto scale = glm::vec3(v.x >= 0 ? 1 : -1, v.y >= 0 ? 1 : -1, 1);
        targetNode->scale(scale);
    }

    if(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Color].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(m_randomModifiers[WorldGenerator_SpawnPoint_ModifierType_Color], rng);
        targetNode->colorize(v);
    }
}

