#ifndef WORLDGENERATOR_DISTRIBUTION_H
#define WORLDGENERATOR_DISTRIBUTION_H

#include "generators/WorldGenerator_SpawnPoint.h"
#include "generators/PoissonDiskSampler.h"

class WorldGenerator;

enum WorldGenerator_DistributionType
{
    WorldGenerator_DistributionType_Uniform,
    WorldGenerator_DistributionType_Poisson,
    WorldGenerator_DistributionType_Path,
    WorldGenerator_DistributionType_SpawnGroup,
    WorldGenerator_DistributionType_None,
};

struct WorldGenerator_Distribution_Parameters
{
    WorldGenerator_Distribution_Parameters();

    glm::vec2 sectionSize;

    WorldGenerator_DistributionType type;
    TerrainGenerator_SpawnType      spawnType;
    float distance;

    bool startInCenter;
    bool useGridPosition;
    bool useDithering;

    pou::HashedString pathName; //For WorldGenerator_DistributionType_Path
    pou::HashedString spawnGroupName; //For WorldGenerator_DistributionType_SpawnGroup
};

struct WorldGenerator_Distribution_DistributedPoints
{
    WorldGenerator_Distribution_DistributedPoints() :
        distributedPositions(nullptr),
        distributedRotations(nullptr)
        {}

    const std::vector<glm::vec2> *distributedPositions;
    const std::vector<float> *distributedRotations;
};

class WorldGenerator_Distribution
{
    public:
        WorldGenerator_Distribution();
        virtual ~WorldGenerator_Distribution();

        bool loadFromXML(const std::string &fileDirectory, TiXmlElement *element, WorldGenerator *worldGenerator);

        void generatesOnNode(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                             bool generateCharacters, pou::RNGenerator *rng);

    protected:
        WorldGenerator_Distribution_DistributedPoints generatePointsDistribution(pou::RNGenerator *rng);
        WorldGenerator_Distribution_DistributedPoints generatePathDistribution(pou::RNGenerator *rng);

        bool loadParameters(TiXmlElement *element, WorldGenerator_Distribution_Parameters &parameters);

    private:
        WorldGenerator_Distribution_Parameters m_parameters;

        WorldGenerator *m_worldGenerator;

        std::vector<WorldGenerator_SpawnPoint> m_spawnPoints;

        std::vector<glm::vec2> m_dummyUniformSampler;
        PoissonDiskSampler m_poissonDiskSampler;
        std::vector<glm::vec2> m_otherSampler;

        std::vector<float> m_distributedRotations;

};

#endif // WORLDGENERATOR_DISTRIBUTION_H
