#include "generators/WorldGenerator_Distribution.h"

#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Hasher.h"


WorldGenerator_Distribution_Parameters::WorldGenerator_Distribution_Parameters() :
    sectionSize(0),
    type(WorldGenerator_DistributionType_None),
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
    auto distributedPoints = this->generatePointsDistribution(rng);
    glm::ivec2 nbrSections = glm::ceil(m_terrain->getExtent()/m_parameters.sectionSize);


    pou::Logger::write("Distribute entities...");

    if(!distributedPoints.distributedPositions)
        return;

    for(float y = 0 ; y < nbrSections.y ; ++y)
    for(float x = 0 ; x < nbrSections.x ; ++x)
    {
        for(size_t curPoint = 0 ; curPoint < distributedPoints.distributedPositions->size() ; ++curPoint)
        {
            auto &pointPos = (*distributedPoints.distributedPositions)[curPoint];
            auto pointWorldPos = pointPos + glm::vec2(x,y) * m_parameters.sectionSize - (m_terrain->getExtent()) * 0.5f;

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
                } else if(prob == -1) {
                    spawnPointsWithProb.clear();
                    spawnPointsWithProb[1.0f] = &spawnPoint;
                    break;
                }
            }

            glm::ivec2 ditherPos(0);
            if(m_parameters.type == WorldGenerator_DistributionType_Poisson)
                ditherPos = glm::round(pointWorldPos/(float)(m_parameters.distance/sqrt(2)));
            else if(m_parameters.type == WorldGenerator_DistributionType_Path)
                ditherPos = {curPoint,0};
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

            float pointRotation = 0;
            if(distributedPoints.distributedRotations
            && curPoint < distributedPoints.distributedRotations->size())
                pointRotation = (*distributedPoints.distributedRotations)[curPoint];

            auto spawnPointIt = spawnPointsWithProb.lower_bound(probValue);
            if(spawnPointIt != spawnPointsWithProb.end())
                spawnPointIt->second->generatesOnNode(pointWorldPos, pointRotation, targetNode,
                                                      syncComponent, generateCharacters, rng);
        }
    }

}

///
///Protected
///


WorldGenerator_Distribution_DistributedPoints WorldGenerator_Distribution::generatePointsDistribution(pou::RNGenerator *rng)
{
    WorldGenerator_Distribution_DistributedPoints distributedPoints;

    //auto sectionSize = m_sectionSize;
    if(m_parameters.sectionSize.x == 0 || m_parameters.sectionSize.y == 0)
        m_parameters.sectionSize = m_terrain->getExtent();

    if(m_parameters.type == WorldGenerator_DistributionType_Poisson)
    {

        m_poissonDiskSampler.setRng(rng);
        if(m_parameters.startInCenter)
            distributedPoints.distributedPositions =
                m_poissonDiskSampler.generateDistributionFrom(m_parameters.sectionSize/2.0f,
                                                              m_parameters.sectionSize, m_parameters.distance);
        else
            distributedPoints.distributedPositions =
                m_poissonDiskSampler.generateDistribution(m_parameters.sectionSize, m_parameters.distance);
    }

    if(m_parameters.type == WorldGenerator_DistributionType_Uniform)
    {
        if(m_parameters.distance <= 0)
            m_parameters.distance = 1;

        m_parameters.sectionSize = m_terrain->getTileSize() * m_parameters.distance;
        distributedPoints.distributedPositions = &m_dummyUniformSampler;
    }

    if(m_parameters.type == WorldGenerator_DistributionType_Path)
        return this->generatePathDistribution(rng);

    if(m_parameters.type == WorldGenerator_DistributionType_SpawnGroup)
    {
    }

    return distributedPoints;
}


WorldGenerator_Distribution_DistributedPoints WorldGenerator_Distribution::generatePathDistribution(pou::RNGenerator *rng)
{
    WorldGenerator_Distribution_DistributedPoints distributedPoints;

    auto rasterizedPaths = m_terrain->getRasterizedPaths(m_parameters.pathName);

    if(!rasterizedPaths)
        return distributedPoints;

    distributedPoints.distributedPositions = &m_pathSampler;
    distributedPoints.distributedRotations = &m_distributedRotations;

    m_pathSampler.clear();
    m_distributedRotations.clear();

    for(auto rasterizedPath : *rasterizedPaths)
    {
        size_t p = m_parameters.distance/2;

        while(p < rasterizedPath.size() - m_parameters.distance/2)
        {
            auto pointPos = glm::vec2(rasterizedPath[p])*m_terrain->getTileSize(); //m_terrain->gridToWorldPosition(rasterizedPath[p].x, rasterizedPath[p].y);
            m_pathSampler.push_back(pointPos);

            ///Compute rotation
            size_t precP = std::max(0, (int)p - (int)ceil(m_parameters.distance/4));
            size_t nextP = std::min((int)rasterizedPath.size()-1, (int)p + (int)ceil(m_parameters.distance/4));

            auto delta = glm::vec2(rasterizedPath[nextP] - rasterizedPath[precP]);
            float angle = glm::atan(delta.y, delta.x);
            m_distributedRotations.push_back(angle);

            p += m_parameters.distance;
        }
    }

    return distributedPoints;
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

    parameters.type = WorldGenerator_DistributionType_None;
    if(typeAtt)
    {
        auto typeStr = std::string(typeAtt);
        if(typeStr == "uniform")
            parameters.type = WorldGenerator_DistributionType_Uniform;
        else if(typeStr == "poisson")
            parameters.type = WorldGenerator_DistributionType_Poisson;
        else if(typeStr == "path")
            parameters.type = WorldGenerator_DistributionType_Path;
        else if(typeStr == "spawnGroup")
            parameters.type = WorldGenerator_DistributionType_SpawnGroup;
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

    if(parameters.type == WorldGenerator_DistributionType_Path)
    {
        auto pathNameAtt  = element->Attribute("pathName");
        parameters.pathName = pou::Hasher::unique_hash(pathNameAtt);
    }

    if(parameters.type == WorldGenerator_DistributionType_SpawnGroup)
    {
        auto spawnGroupAtt  = element->Attribute("spawnGroup");
        parameters.spawnGroupName = pou::Hasher::unique_hash(spawnGroupAtt);
    }

    return (true);
}

