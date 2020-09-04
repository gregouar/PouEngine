#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "generators/TerrainGenerator.h"
#include "generators/WorldGenerator_Distribution.h"
#include "world/SunModel.h"

#include "audio/SoundModel.h"

struct WorldGenerator_Parameters
{
    SoundModel  musicModel;
    pou::Color  ambientLight;
};

class WorldGenerator
{
    public:
        WorldGenerator();
        virtual ~WorldGenerator();

        bool loadFromFile(const std::string &filePath);

        void generatesOnNode(pou::SceneNode *targetNode, int seed, GameWorld_Sync *syncComponent,
                             bool generateCharacters, bool preventCentralSpawn);

        std::list<std::shared_ptr<pou::LightEntity> > generatesSuns(pou::SceneNode *targetNode);
        void updateSunsAndAmbientLight(std::list<std::shared_ptr<pou::LightEntity> > &suns,
                                       pou::Scene *scene,
                                       float dayTime);

        void addToSpawnGroup(pou::HashedString spawnGroupName, glm::vec2 position);
        std::vector<glm::vec2> getSpawnGroup(pou::HashedString spawnGroupName);

        void playWorldMusic();
        void stopWorldMusic();

        const std::string &getFilePath();
        int getGeneratingSeed();
        TerrainGenerator *terrain();

        std::pair<glm::vec2, glm::vec2> getWorldBounds();

    protected:
        bool loadFromXML(TiXmlHandle *hdl);

        bool loadParameters(TiXmlElement *element);
        //bool loadDistribution(TiXmlElement *element);
        //bool loadCharacters(TiXmlElement *element);
        /*bool loadCharacter(TiXmlElement *element);
        void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_CharacterModel_Modifier &modifier);

        void generateCharacters(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent);
        void generateCharacter(int x, int y, WorldGenerator_CharacterModel &characterModel,
                               pou::SceneNode *targetNode, GameWorld_Sync *syncComponent);
        glm::vec4 generateRandomValue(WorldGenerator_CharacterModel_Modifier &modifier);*/

    private:
        pou::RNGenerator m_rng;

        std::string m_filePath;
        std::string m_fileDirectory;

        int m_generatingSeed;

        WorldGenerator_Parameters m_parameters;
        TerrainGenerator m_terrainGenerator;

        std::list<SunModel> m_sunModels;

        //std::vector<WorldGenerator_CharacterModel> m_characterModels;
        WorldGenerator_Distribution m_pointsOfInterest;
        std::list<WorldGenerator_Distribution> m_distributions;
        std::unordered_multimap<pou::HashedString, glm::vec2> m_spawnGroups;

        pou::SoundTypeId m_musicEvent;
};

#endif // WORLDGENERATOR_H
