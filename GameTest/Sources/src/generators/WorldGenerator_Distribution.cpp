#include "generators/WorldGenerator_Distribution.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"


WorldGenerator_Distribution::WorldGenerator_Distribution() :
    m_type(WorldGenerator_DistributionType_Uniform),
    m_distance(1),
    m_sectionSize(0),
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

    auto typeAtt = element->Attribute("type");
    auto distanceAtt = element->Attribute("distance");
    auto sectionSizeAtt = element->Attribute("sectionSize");

    m_type = WorldGenerator_DistributionType_Uniform;
    if(typeAtt)
    {
        auto typeStr = std::string(typeAtt);
        if(typeStr == "uniform")
            m_type = WorldGenerator_DistributionType_Uniform;
        else if(typeStr == "poisson")
            m_type = WorldGenerator_DistributionType_Poisson;
    }

    if(distanceAtt)
        m_distance = pou::Parser::parseFloat(std::string(distanceAtt));

    if(sectionSizeAtt)
        m_sectionSize = glm::vec2(pou::Parser::parseFloat(std::string(sectionSizeAtt)));

    auto spawnPointChild = element->FirstChildElement("spawnPoint");
    while(spawnPointChild != nullptr)
    {
        auto spawnPointElement = spawnPointChild->ToElement();
        if(spawnPointElement)
        {
            m_spawnPoints.push_back({});
            m_spawnPoints.back().loadFromXML(fileDirectory, spawnPointElement, terrainGenerator);
        }
        spawnPointChild = spawnPointChild->NextSiblingElement("spawnPoint");
    }

    m_terrain = terrainGenerator;

    return loaded;
}


void WorldGenerator_Distribution::generatesOnNode(WorldNode *targetNode, GameWorld_Sync *syncComponent,
                                                  bool generateCharacters, pou::RNGenerator *rng)
{
    auto pointsDistribution = this->generatePointsDistribution();

    glm::ivec2 nbrSections = glm::ceil(m_terrain->getExtent()/m_sectionSize);

    for(float y = 0 ; y < nbrSections.y ; ++y)
    for(float x = 0 ; x < nbrSections.x ; ++x)
    for(auto &point : pointsDistribution)
    {
        auto pointWorldPos = point + glm::vec2(x,y) * m_sectionSize - (m_terrain->getExtent()) * 0.5f;

        std::map<float, WorldGenerator_SpawnPoint*> spawnPointsWithProb;

        float totalProbability = 0;
        for(auto &spawnPoint : m_spawnPoints)
        {
            auto prob = spawnPoint.getSpawnProbability(pointWorldPos, m_terrain);
            if(prob > 0)
            {
                totalProbability += prob;
                spawnPointsWithProb[totalProbability] = &spawnPoint;
            }
        }

        auto nTotalProbability = std::max(totalProbability, 1.0f);
        float probValue = pou::RNGesus::uniformFloat(0, nTotalProbability, rng);

        if(probValue >= totalProbability)
            continue;

        auto spawnPointIt = spawnPointsWithProb.lower_bound(probValue);
        spawnPointIt->second->generatesOnNode(pointWorldPos, targetNode, syncComponent, generateCharacters, rng);
    }

}

///
///Protected
///


const std::vector<glm::vec2> & WorldGenerator_Distribution::generatePointsDistribution()
{
    auto sectionSize = m_sectionSize;
    if(sectionSize.x == 0 || sectionSize.y == 0)
        sectionSize = m_terrain->getExtent();

    if(m_type == WorldGenerator_DistributionType_Poisson)
        return m_poissonDiskSampler.generateDistribution(sectionSize, m_distance);

    if(m_type == WorldGenerator_DistributionType_Uniform)
    {
        if(m_distance <= 0)
            m_distance = 1;

        m_sectionSize = m_terrain->getTileSize() * m_distance;
        return m_dummyUniformSampler;
    }

    return m_dummyUniformSampler;
}

