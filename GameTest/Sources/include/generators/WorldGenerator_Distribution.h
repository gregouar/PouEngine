#ifndef WORLDGENERATOR_DISTRIBUTION_H
#define WORLDGENERATOR_DISTRIBUTION_H

#include "generators/WorldGenerator_SpawnPoint.h"
#include "generators/PoissonDiskSampler.h"

enum WorldGenerator_DistributionType
{
    WorldGenerator_DistributionType_Uniform,
    WorldGenerator_DistributionType_Poisson,
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
};

class WorldGenerator_Distribution
{
    public:
        WorldGenerator_Distribution();
        virtual ~WorldGenerator_Distribution();

        bool loadFromXML(const std::string &fileDirectory, TiXmlElement *element, TerrainGenerator *terrainGenerator);

        void generatesOnNode(WorldNode *targetNode, GameWorld_Sync *syncComponent,
                             bool generateCharacters, pou::RNGenerator *rng);

    protected:
        const std::vector<glm::vec2> &generatePointsDistribution(pou::RNGenerator *rng);

        bool loadParameters(TiXmlElement *element, WorldGenerator_Distribution_Parameters &parameters);

    private:
        WorldGenerator_Distribution_Parameters m_parameters;

        TerrainGenerator *m_terrain;

        std::vector<WorldGenerator_SpawnPoint> m_spawnPoints;

        std::vector<glm::vec2> m_dummyUniformSampler;
        PoissonDiskSampler m_poissonDiskSampler;

};

#endif // WORLDGENERATOR_DISTRIBUTION_H
