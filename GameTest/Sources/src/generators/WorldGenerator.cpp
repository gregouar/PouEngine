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


void WorldGenerator::generatesOnNode(WorldNode *targetNode, int seed, GameWorld_Sync *syncComponent)
{
    m_generatingSeed = seed;
    m_rng.seed(seed);

    m_terrainGenerator.generatesOnNode(targetNode, &m_rng);
}

void WorldGenerator::playWorldMusic()
{
    std::cout<<"PLAY WORLD MUSIC"<<std::endl;
    std::cout<<m_parameters.musicModel.isEvent()<<std::endl;
    std::cout<<m_parameters.musicModel.getPath()<<std::endl;
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

    return loaded;
}

bool WorldGenerator::loadParameters(TiXmlElement *element)
{
    auto e = element->FirstChildElement("music");
    if(e != nullptr)
        m_parameters.musicModel.loadFromXML(e);

    return (true);
}
