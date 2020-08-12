#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "generators/TerrainGenerator.h"
#include "generators/WorldGenerator_Distribution.h"

#include "audio/SoundModel.h"

struct WorldGenerator_Parameters
{
    SoundModel musicModel;
};

class WorldGenerator
{
    public:
        WorldGenerator();
        virtual ~WorldGenerator();

        bool loadFromFile(const std::string &filePath);

        void generatesOnNode(WorldNode *targetNode, int seed, GameWorld_Sync *syncComponent,
                             bool generateCharacters = true);

        void playWorldMusic();
        void stopWorldMusic();

        const std::string &getFilePath();
        int getGeneratingSeed();

    protected:
        bool loadFromXML(TiXmlHandle *hdl);

        bool loadParameters(TiXmlElement *element);
        //bool loadDistribution(TiXmlElement *element);
        //bool loadCharacters(TiXmlElement *element);
        /*bool loadCharacter(TiXmlElement *element);
        void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_CharacterModel_Modifier &modifier);

        void generateCharacters(WorldNode *targetNode, GameWorld_Sync *syncComponent);
        void generateCharacter(int x, int y, WorldGenerator_CharacterModel &characterModel,
                               WorldNode *targetNode, GameWorld_Sync *syncComponent);
        glm::vec4 generateRandomValue(WorldGenerator_CharacterModel_Modifier &modifier);*/

    private:
        pou::RNGenerator m_rng;

        std::string m_filePath;
        std::string m_fileDirectory;

        int m_generatingSeed;

        WorldGenerator_Parameters m_parameters;
        TerrainGenerator m_terrainGenerator;

        //std::vector<WorldGenerator_CharacterModel> m_characterModels;
        WorldGenerator_Distribution m_pointsOfInterest;
        std::list<WorldGenerator_Distribution> m_distributions;

        pou::SoundTypeId m_musicEvent;
};

#endif // WORLDGENERATOR_H
