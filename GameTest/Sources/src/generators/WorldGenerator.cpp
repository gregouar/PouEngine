#include "generators/WorldGenerator.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


WorldGenerator::WorldGenerator() :
    m_musicEvent(0)
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


void WorldGenerator::generatesOnNode(pou::SceneNode *targetNode, int seed, GameWorld_Sync *syncComponent,
                                     bool generateCharacters)
{
    pou::Logger::write("Initialize procedural world generation...");

    m_generatingSeed = seed;
    m_rng.seed(seed);

    TerrainGenerator_SpawnOnlyZone airBalloonSafeZone;
    airBalloonSafeZone.gridPosition = glm::ivec2(m_terrainGenerator.getGridSize()/2.0f) - glm::ivec2(12);
    airBalloonSafeZone.gridExtent   = glm::ivec2(24);
    airBalloonSafeZone.spawnType    = TerrainGenerator_SpawnType_Safe;
    m_terrainGenerator.addSpawnOnlyZone(airBalloonSafeZone);

    TerrainGenerator_SpawnOnlyZone airBalloonZone;
    airBalloonZone.gridPosition = glm::ivec2(m_terrainGenerator.getGridSize()/2.0f) - glm::ivec2(6);
    airBalloonZone.gridExtent   = glm::ivec2(12);
    airBalloonZone.spawnType    = TerrainGenerator_SpawnType_None;
    m_terrainGenerator.addSpawnOnlyZone(airBalloonZone);

    m_terrainGenerator.resetGrid();

    pou::Logger::write("Generates point of interests...");

    m_pointsOfInterest.generatesOnNode(targetNode, syncComponent, generateCharacters, &m_rng);

    pou::Logger::write("Generates terrain...");

    m_terrainGenerator.generatesOnNode(targetNode, &m_rng);

    pou::Logger::write("Generates entities...");

    for(auto &distribution : m_distributions)
        distribution.generatesOnNode(targetNode, syncComponent, generateCharacters, &m_rng);
}

void WorldGenerator::playWorldMusic()
{
    if(m_parameters.musicModel.isEvent())
    {
        m_musicEvent = pou::AudioEngine::createEvent(m_parameters.musicModel.getPath());
        pou::AudioEngine::playEvent(m_musicEvent);
    }
}

void WorldGenerator::stopWorldMusic()
{
    if(m_parameters.musicModel.isEvent())
    {
        pou::AudioEngine::destroyEvent(m_musicEvent);
        m_musicEvent = 0;
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

    auto poisChild = hdl->FirstChildElement("pois");
    if(poisChild.Element() != nullptr)
    {
        auto poisElement = poisChild.Element();
        if(poisElement)
            m_pointsOfInterest.loadFromXML(m_fileDirectory, poisElement, &m_terrainGenerator);
    }

    auto distributionChild = hdl->FirstChildElement("distribution");
    while(distributionChild.Element() != nullptr)
    {
        auto distributionElement = distributionChild.Element();
        if(distributionElement)
        {
            m_distributions.emplace_back();
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


