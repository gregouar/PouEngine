#ifndef WORLDGENERATOR_SPAWNPOINT_H
#define WORLDGENERATOR_SPAWNPOINT_H

#include "generators/TerrainGenerator.h"
#include "assets/PrefabAsset.h"

enum WorldGenerator_SpawnPoint_ModifierTypes
{
    WorldGenerator_SpawnPoint_ModifierType_Position,
    WorldGenerator_SpawnPoint_ModifierType_Rotation,
    WorldGenerator_SpawnPoint_ModifierType_Flip,
    WorldGenerator_SpawnPoint_ModifierType_Color,
    NBR_WorldGenerator_SpawnPoint_ModifierTypes,
};

enum WorldGenerator_RandomType
{
    WorldGenerator_RandomType_None,
    WorldGenerator_RandomType_Uniform,
    WorldGenerator_RandomType_Gaussian,
};

struct WorldGenerator_SpawnPoint_Modifier
{
    WorldGenerator_SpawnPoint_Modifier();

    //WorldGenerator_CharacterModel_ModifierTypes modifierType;
    WorldGenerator_RandomType randomType;

    glm::vec4 minValue;
    glm::vec4 maxValue;

    int usePrecedingValue[4];
};

/*struct WorldGenerator_CharacterModel
{
    WorldGenerator_CharacterModel() : spawnProbability(0),groundLayer(nullptr){};

    CharacterModelAsset *modelAsset;

    float spawnProbability;
    const TerrainGenerator_GroundLayer *groundLayer;
    WorldGenerator_CharacterModel_Modifier randomModifiers[NBR_WorldGenerator_CharacterModel_ModifierTypes];
};*/

struct CharacterSpawn
{
    CharacterSpawn() : minAmount(1), maxAmount(1){}

    CharacterModelAsset *modelAsset;
    int minAmount;
    int maxAmount;
};

class WorldGenerator_SpawnPoint
{
    public:
        WorldGenerator_SpawnPoint();
        virtual ~WorldGenerator_SpawnPoint();

        bool loadFromXML(const std::string &fileDirectory, TiXmlElement *element, TerrainGenerator *terrainGenerator);

        void generatesOnNode(glm::vec2 worldPos, WorldNode *targetNode, GameWorld_Sync *syncComponent,
                             bool generateCharacters, pou::RNGenerator *rng);

        float getSpawnProbability(glm::vec2 worldPos, TerrainGenerator *terrain);

    protected:
        void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_SpawnPoint_Modifier &modifier);

        glm::vec4 generateRandomValue(WorldGenerator_SpawnPoint_Modifier &modifier, pou::RNGenerator *rng);

        void spawnCharacter(CharacterModelAsset *characterModel, glm::vec2 worldPos,
                            WorldNode *targetNode, GameWorld_Sync *syncComponent,
                            pou::RNGenerator *rng);

        void applyRandomModifiers(WorldNode *targetNode, pou::RNGenerator *rng);


    private:
        ///ADD:
        std::vector<CharacterSpawn>         m_characterSpawnModels;
        std::vector<pou::SpriteModel*>      m_spriteModelAssets;
        std::vector<PrefabAsset*>           m_prefabAssets;

        float m_spawnProbability;
        const TerrainGenerator_GroundLayer *m_groundLayer;
        WorldGenerator_SpawnPoint_Modifier m_randomModifiers[NBR_WorldGenerator_SpawnPoint_ModifierTypes];
};

#endif // WORLDGENERATOR_CHARACTERSPAWN_H
