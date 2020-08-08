#include "generators/WorldGenerator.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


WorldGenerator::WorldGenerator()
{
    //ctor
}

WorldGenerator::~WorldGenerator()
{
    //dtor
}

bool WorldGenerator::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load world model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    m_filePath = filePath;
    m_fileDirectory = pou::Parser::findFileDirectory(m_filePath);

    return this->loadFromXML(&hdl);
}


void WorldGenerator::generatesOnNode(WorldNode *targetNode, int seed, GameWorld_Sync *syncComponent,
                                     bool generateCharacters)
{
    m_generatingSeed = seed;
    m_rng.seed(seed);

    m_terrainGenerator.generatesOnNode(targetNode, &m_rng);

    for(auto &distribution : m_distributions)
        distribution.generatesOnNode(targetNode, syncComponent, generateCharacters, &m_rng);
}

void WorldGenerator::playWorldMusic()
{
    if(m_parameters.musicModel.isEvent())
    {
        auto musicEvent = pou::AudioEngine::createEvent(m_parameters.musicModel.getPath());
        pou::AudioEngine::playEvent(musicEvent);
    }
}

const std::string &WorldGenerator::getFilePath()
{
    return m_filePath;
}

int WorldGenerator::getGeneratingSeed()
{
    return m_generatingSeed;
}

///
///Protected
///

bool WorldGenerator::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    element = hdl->FirstChildElement("parameters").Element();
    if(!element)
        return (false);
    this->loadParameters(element);

    auto terrainHdl = hdl->FirstChildElement("terrain");
    if(!terrainHdl.Element())
        return (false);
    m_terrainGenerator.loadFromXML(&terrainHdl, m_fileDirectory);

    auto distributionChild = hdl->FirstChildElement("distribution");
    while(distributionChild.Element() != nullptr)
    {
        auto distributionElement = distributionChild.Element();
        if(distributionElement)
        {
            m_distributions.push_back({});
            if(!m_distributions.back().loadFromXML(m_fileDirectory, distributionElement, &m_terrainGenerator))
                m_distributions.pop_back();
        }
        distributionChild = distributionChild.Element()->NextSiblingElement("distribution");
    }


    return loaded;
}

bool WorldGenerator::loadParameters(TiXmlElement *element)
{
    auto e = element->FirstChildElement("music");
    if(e != nullptr)
        m_parameters.musicModel.loadFromXML(e);

    return (true);
}

/*bool WorldGenerator::loadCharacters(TiXmlElement *element)
{
    bool loaded = true;

    auto characterChild = element->FirstChildElement("character");
    while(characterChild != nullptr)
    {
        auto characterElement = characterChild->ToElement();
        this->loadCharacter(characterElement);
        characterChild = characterChild->NextSiblingElement("character");
    }

    return loaded;
}*/

/*bool WorldGenerator::loadDistribution(TiXmlElement *element)
{
    bool loaded = true;

    auto characterChild = element->FirstChildElement("character");
    while(characterChild != nullptr)
    {
        auto characterElement = characterChild->ToElement();
        this->loadCharacter(characterElement);
        characterChild = characterChild->NextSiblingElement("character");
    }

    return loaded;
}*/

/*bool WorldGenerator::loadCharacter(TiXmlElement *characterElement)
{
    auto pathAtt = characterElement->Attribute("path");
    auto groundLayerAtt = characterElement->Attribute("groundLayer");
    auto spawnProbabilityAtt = characterElement->Attribute("spawnProbability");

    if(!pathAtt)
        return (false);

    auto modelAsset = CharacterModelsHandler::loadAssetFromFile(m_fileDirectory+pathAtt);
    if(!modelAsset)
        return (false);

    m_characterModels.push_back({});
    auto &characterModel = m_characterModels.back();

    characterModel.modelAsset = modelAsset;

    if(groundLayerAtt)
        characterModel.groundLayer = m_terrainGenerator.getGroundLayer(groundLayerAtt);

    if(spawnProbabilityAtt)
        characterModel.spawnProbability = pou::Parser::parseFloat(spawnProbabilityAtt);

    auto modifierChild = characterElement->FirstChildElement("modifier");
    while(modifierChild != nullptr)
    {
        auto modifierElement = modifierChild->ToElement();

        auto typeAtt = modifierElement->Attribute("type");
        if(typeAtt)
        {
            int type = 0;
            if(std::string(typeAtt) == "position")
                type = WorldGenerator_CharacterModel_ModifierType_Position;
            else if(std::string(typeAtt) == "rotation")
                type = WorldGenerator_CharacterModel_ModifierType_Rotation;
            else if(std::string(typeAtt) == "flip")
                type = WorldGenerator_CharacterModel_ModifierType_Flip;
            else if(std::string(typeAtt) == "color")
                type = WorldGenerator_CharacterModel_ModifierType_Color;

            auto &randomModifier = characterModel.randomModifiers[type];

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

    return (true);
}

void WorldGenerator::loadRandomModifierValue(TiXmlElement *element, int i, WorldGenerator_CharacterModel_Modifier &modifier)
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

    if(pou::Parser::isFloat(valueString))
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
    }
}

void WorldGenerator::generateCharacters(WorldNode *targetNode, GameWorld_Sync *syncComponent)
{
    for(int y = 0 ; y < m_terrainGenerator.getGridSize().y ; y++)
    for(int x = 0 ; x < m_terrainGenerator.getGridSize().x ; x++)
    {
        for(auto &characterModel : m_characterModels)
        if(characterModel.groundLayer == m_terrainGenerator.getGridValue(x,y))
        {
            float randValue = pou::RNGesus::uniformFloat(0,1,&m_rng);
            if(randValue <= characterModel.spawnProbability)
                this->generateCharacter(x, y, characterModel, targetNode, syncComponent);
        }
    }
}

void WorldGenerator::generateCharacter(int x, int y, WorldGenerator_CharacterModel &characterModel,
                                       WorldNode *targetNode, GameWorld_Sync *syncComponent)
{
    auto character = std::make_shared<Character>();
    character->createFromModel(characterModel.modelAsset);

    glm::vec2 pos = m_terrainGenerator.gridToWorldPosition(x,y);
    if(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Position].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Position]);
        pos += glm::vec2(v);
    }
    character->setPosition(pos);

    if(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Rotation].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Rotation]);
        character->setRotation(v,false);
    }

    if(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Flip].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Flip]);
        auto scale = glm::vec3(v.x >= 0 ? 1 : -1, v.y >= 0 ? 1 : -1, 1);
        character->scale(scale);
    }

    if(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Color].randomType != WorldGenerator_RandomType_None)
    {
        auto v = this->generateRandomValue(characterModel.randomModifiers[WorldGenerator_CharacterModel_ModifierType_Color]);
        character->colorize(v);
    }

    targetNode->addChildNode(character);

    syncComponent->syncElement(character);
    syncComponent->syncElement(characterModel.modelAsset);
}

glm::vec4 WorldGenerator::generateRandomValue(WorldGenerator_CharacterModel_Modifier &modifier)
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
            randomValue[i] = pou::RNGesus::uniformFloat(modifier.minValue[i], modifier.maxValue[i], &m_rng);
    }

    return randomValue;
}*/


