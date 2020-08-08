#ifndef WORLDGENERATOR_DISTRIBUTION_H
#define WORLDGENERATOR_DISTRIBUTION_H

#include "generators/WorldGenerator_SpawnPoint.h"
#include "generators/PoissonDiskSampler.h"

enum WorldGenerator_DistributionType
{
    WorldGenerator_DistributionType_Uniform,
    WorldGenerator_DistributionType_Poisson,
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
        const std::vector<glm::vec2> &generatePointsDistribution();
        //bool loadCharacter(TiXmlElement *element);
        //void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_CharacterModel_Modifier &modifier);

        /*void generateCharacters(WorldNode *targetNode, GameWorld_Sync *syncComponent);
        void generateCharacter(int x, int y, WorldGenerator_CharacterModel &characterModel,
                               WorldNode *targetNode, GameWorld_Sync *syncComponent);
        glm::vec4 generateRandomValue(WorldGenerator_CharacterModel_Modifier &modifier);*/

    private:
        WorldGenerator_DistributionType m_type;
        float m_distance;
        glm::vec2 m_sectionSize;

        TerrainGenerator *m_terrain;

        std::vector<WorldGenerator_SpawnPoint> m_spawnPoints;

        std::vector<glm::vec2> m_dummyUniformSampler;
        PoissonDiskSampler m_poissonDiskSampler;
};

#endif // WORLDGENERATOR_DISTRIBUTION_H
