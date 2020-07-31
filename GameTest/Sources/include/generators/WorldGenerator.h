#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "generators/TerrainGenerator.h"
#include "audio/SoundModel.h"

enum WorldGenerator_CharacterModel_ModifierTypes
{
    WorldGenerator_CharacterModel_ModifierType_Position,
    WorldGenerator_CharacterModel_ModifierType_Rotation,
    WorldGenerator_CharacterModel_ModifierType_Flip,
    WorldGenerator_CharacterModel_ModifierType_Color,
    NBR_WorldGenerator_CharacterModel_ModifierTypes,
};

enum WorldGenerator_RandomType
{
    WorldGenerator_RandomType_None,
    WorldGenerator_RandomType_Uniform,
    WorldGenerator_RandomType_Gaussian,
};

struct WorldGenerator_Parameters
{
    SoundModel musicModel;
};


struct WorldGenerator_CharacterModel_Modifier
{
    WorldGenerator_CharacterModel_Modifier();

    //WorldGenerator_CharacterModel_ModifierTypes modifierType;
    WorldGenerator_RandomType                   randomType;

    glm::vec4 minValue;
    glm::vec4 maxValue;

    int usePrecedingValue[4];
};

struct WorldGenerator_CharacterModel
{
    WorldGenerator_CharacterModel() : spawnProbability(0),groundLayer(nullptr){};

    CharacterModelAsset *modelAsset;

    float spawnProbability;
    const TerrainGenerator_GroundLayer *groundLayer;
    WorldGenerator_CharacterModel_Modifier randomModifiers[NBR_WorldGenerator_CharacterModel_ModifierTypes];
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

        const std::string &getFilePath();
        int getGeneratingSeed();

    protected:
        bool loadFromXML(TiXmlHandle *hdl);

        bool loadParameters(TiXmlElement *element);
        bool loadCharacters(TiXmlElement *element);
        bool loadCharacter(TiXmlElement *element);
        void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_CharacterModel_Modifier &modifier);

        void generateCharacters(WorldNode *targetNode, GameWorld_Sync *syncComponent);
        void generateCharacter(int x, int y, WorldGenerator_CharacterModel &characterModel,
                               WorldNode *targetNode, GameWorld_Sync *syncComponent);
        glm::vec4 generateRandomValue(WorldGenerator_CharacterModel_Modifier &modifier);

    private:
        pou::RNGenerator m_rng;

        std::string m_filePath;
        std::string m_fileDirectory;

        int m_generatingSeed;

        WorldGenerator_Parameters m_parameters;
        TerrainGenerator m_terrainGenerator;

        std::vector<WorldGenerator_CharacterModel> m_characterModels;
};

#endif // WORLDGENERATOR_H
