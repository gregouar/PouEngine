#include "generators/WorldGenerator_Distribution.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


WorldGenerator_Distribution_Parameters::WorldGenerator_Distribution_Parameters() :
    sectionSize(0),
    type(WorldGenerator_DistributionType_Uniform),
    spawnType(TerrainGenerator_SpawnType_Safe),
    distance(1),
    startInCenter(false),
    useGridPosition(false)
    //changeSpawnTypeTo(false),
    //newSpawnType(TerrainGenerator_SpawnType_None)
{

}

WorldGenerator_Distribution::WorldGenerator_Distribution() :
    m_terrain(nullptr)
{
    m_dummyUniformSampler.push_back(glm::vec2(0));
}

WorldGenerator_Distribution::~WorldGenerator_Distribution()
{
    //dtor
}

bool WorldGenerator_Distribution::loadFromXML(const std::string &fileDirectory, TiXmlElement *element, TerrainGenerator *terrainGenerator)
{
    bool loaded = true;

    if(!this->loadParameters(element, m_parameters))
        loaded = false;

    auto spawnPointChild = element->FirstChildElement("spawnPoint");
    while(spawnPointChild != nullptr)
    {
        auto spawnPointElement = spawnPointChild->ToElement();
        if(spawnPointElement)
        {
            m_spawnPoints.emplace_back();
            m_spawnPoints.back().loadFromXML(fileDirectory, spawnPointElement, terrainGenerator);
        }
        spawnPointChild = spawnPointChild->NextSiblingElement("spawnPoint");
    }

    m_terrain = terrainGenerator;

    return loaded;
}


void WorldGenerator_Distribution::generatesOnNode(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                                                  bool generateCharacters, pou::RNGenerator *rng)
{
    pou::Logger::write("Generate distribution...");
    auto pointsDistribution = this->generatePointsDistribution(rng);
    glm::ivec2 nbrSections = glm::ceil(m_terrain->getExtent()/m_parameters.sectionSize);


    pou::Logger::write("Distribute entities...");

    for(float y = 0 ; y < nbrSections.y ; ++y)
    for(float x = 0 ; x < nbrSections.x ; ++x)
    for(auto &point : pointsDistribution)
    {
        auto pointWorldPos = point + glm::vec2(x,y) * m_parameters.sectionSize - (m_terrain->getExtent()) * 0.5f;

        if(m_parameters.useGridPosition)
            pointWorldPos = glm::round(pointWorldPos/m_terrain->getTileSize()) * m_terrain->getTileSize();

        if(m_terrain->getSpawnType(pointWorldPos) < m_parameters.spawnType)
            continue;

        std::map<float, WorldGenerator_SpawnPoint*> spawnPointsWithProb;

        float totalProbability = 0;
        for(auto &spawnPoint : m_spawnPoints)
        {
            auto prob = spawnPoint.getSpawnProbability(pointWorldPos);
            if(prob > 0)
            {
                totalProbability += prob;
                spawnPointsWithProb[totalProbability] = &spawnPoint;
            }
        }

        glm::ivec2 ditherPos(0);
        if(m_parameters.type == WorldGenerator_DistributionType_Poisson)
            ditherPos = glm::round(pointWorldPos/(float)(m_parameters.distance/sqrt(2)));
        else
            ditherPos = m_terrain->worldToGridPosition(pointWorldPos);

        bool ditherFactor = (ditherPos.x + ditherPos.y) % 2;

        float probValue = pou::RNGesus::uniformFloat(0.0f, 0.5f, rng) + ditherFactor * 0.5f;
        if(totalProbability > 1.0f)
            probValue *= totalProbability;

        /**auto nTotalProbability = std::max(totalProbability, 1.0f);
        float probValue = pou::RNGesus::uniformFloat(0, nTotalProbability, rng);

        if(probValue >= totalProbability)
            continue;**/

        auto spawnPointIt = spawnPointsWithProb.lower_bound(probValue);
        if(spawnPointIt != spawnPointsWithProb.end())
            spawnPointIt->second->generatesOnNode(pointWorldPos, targetNode, syncComponent, generateCharacters, rng);
    }

}

///
///Protected
///


const std::vector<glm::vec2> & WorldGenerator_Distribution::generatePointsDistribution(pou::RNGenerator *rng)
{
    //auto sectionSize = m_sectionSize;
    if(m_parameters.sectionSize.x == 0 || m_parameters.sectionSize.y == 0)
        m_parameters.sectionSize = m_terrain->getExtent();

    if(m_parameters.type == WorldGenerator_DistributionType_Poisson)
    {
        m_poissonDiskSampler.setRng(rng);
        if(m_parameters.startInCenter)
            return m_poissonDiskSampler.generateDistributionFrom(m_parameters.sectionSize/2.0f,
                                                                 m_parameters.sectionSize, m_parameters.distance);
        else
            return m_poissonDiskSampler.generateDistribution(m_parameters.sectionSize, m_parameters.distance);
    }

    if(m_parameters.type == WorldGenerator_DistributionType_Uniform)
    {
        if(m_parameters.distance <= 0)
            m_parameters.distance = 1;

        m_parameters.sectionSize = m_terrain->getTileSize() * m_parameters.distance;
        return m_dummyUniformSampler;
    }

    return m_dummyUniformSampler;
}


bool WorldGenerator_Distribution::loadParameters(TiXmlElement *element, WorldGenerator_Distribution_Parameters &parameters)
{
    auto typeAtt        = element->Attribute("type");
    auto distanceAtt    = element->Attribute("distance");
    auto sectionSizeAtt = element->Attribute("sectionSize");
    auto spawnTypeAtt   = element->Attribute("spawnType");
    //auto changeSpawnTypeAtt = element->Attribute("changeSpawnTypeTo");
    auto useGridPositionAtt = element->Attribute("useGridPosition");
    auto startInCenterAtt   = element->Attribute("startInCenter");

    parameters.type = WorldGenerator_DistributionType_Uniform;
    if(typeAtt)
    {
        auto typeStr = std::string(typeAtt);
        if(typeStr == "uniform")
            parameters.type = WorldGenerator_DistributionType_Uniform;
        else if(typeStr == "poisson")
            parameters.type = WorldGenerator_DistributionType_Poisson;
        else if(typeStr == "path")
            parameters.type = WorldGenerator_DistributionType_Path;
    }

    if(distanceAtt)
        parameters.distance = pou::Parser::parseFloat(std::string(distanceAtt));

    if(sectionSizeAtt)
        parameters.sectionSize = glm::vec2(pou::Parser::parseFloat(std::string(sectionSizeAtt)));

    if(spawnTypeAtt)
    {
        auto spawnTypeStr = std::string(spawnTypeAtt);
        if(spawnTypeStr == "safe")
            parameters.spawnType = TerrainGenerator_SpawnType_Safe;
        else if(spawnTypeStr == "unsafe")
            parameters.spawnType = TerrainGenerator_SpawnType_All;
    }

    /*if(changeSpawnTypeAtt)
    {
        parameters.changeSpawnTypeTo = true;
        auto changeSpawnTypeStr = std::string(changeSpawnTypeAtt);
        if(changeSpawnTypeStr == "all")
            parameters.newSpawnType = TerrainGenerator_SpawnType_All;
        else if(changeSpawnTypeStr == "safe")
            parameters.newSpawnType = TerrainGenerator_SpawnType_Safe;
        else if(changeSpawnTypeStr == "none")
            parameters.newSpawnType = TerrainGenerator_SpawnType_None;
        else
            parameters.changeSpawnTypeTo = false;
    }*/

    if(useGridPositionAtt && pou::Parser::isBool(useGridPositionAtt))
        parameters.useGridPosition = pou::Parser::parseBool(useGridPositionAtt);

    if(startInCenterAtt && pou::Parser::isBool(startInCenterAtt))
        parameters.startInCenter = pou::Parser::parseBool(startInCenterAtt);

    return (true);
}

