#ifndef WORLDGENERATOR_SPAWNPOINT_H
#define WORLDGENERATOR_SPAWNPOINT_H

#include "generators/TerrainGenerator.h"
#include "assets/PrefabAsset.h"

class WorldGenerator;

enum WorldGenerator_SpawnPoint_ModifierTypes
{
    WorldGenerator_SpawnPoint_ModifierType_GridPosition,
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

struct WorldGenerator_SpawnPoint_Parameters
{
    WorldGenerator_SpawnPoint_Parameters();

    float   spawnProbability;
    bool    unique;
    //bool    preventOtherSpawns;

    const TerrainGenerator_GroundLayer *groundLayer; ///I should remove this and change by a list of probability

    bool changeSpawnTypeTo;
    TerrainGenerator_SpawnType newSpawnType;

    bool changeGroundLayerTo;
    const TerrainGenerator_GroundLayer *newGroundLayer;
    bool preventGroundSpawning;

};

struct WorldGenerator_SpawnPoint_PathConnection
{
    WorldGenerator_SpawnPoint_PathConnection();

    float   spawnProbability;

    pou::HashedString pathName;
    glm::vec2 position;
};

struct WorldGenerator_SpawnPoint_SubSpawn
{
    WorldGenerator_SpawnPoint_SubSpawn();

    float   spawnProbability;

    pou::HashedString spawnGroupName;
    glm::vec2 position;
};

struct WorldGenerator_SpawnPoint_Ground
{
    WorldGenerator_SpawnPoint_Ground();

    glm::ivec2 position;
    const TerrainGenerator_GroundLayer *groundLayer;
    bool preventGroundSpawning;
    //pou::HashedString groundName;
};

class WorldGenerator_SpawnPoint
{
    public:
        WorldGenerator_SpawnPoint();
        virtual ~WorldGenerator_SpawnPoint();

        bool loadFromXML(const std::string &fileDirectory, TiXmlElement *element, WorldGenerator *worldGenerator);

        void generatesOnNode(glm::vec2 worldPos, float pointRotation,
                             pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                             bool generateCharacters, pou::RNGenerator *rng);

        float getSpawnProbability(glm::vec2 worldPos);

        //bool    preventOtherSpawns();

    protected:
        bool loadParameters(TiXmlElement *element, WorldGenerator_SpawnPoint_Parameters &parameters);
        void loadModifier(TiXmlElement *element);
        void loadCharacter(TiXmlElement *element);
        void loadSprite(TiXmlElement *element);
        void loadPrefab(TiXmlElement *element);
        void loadPathConnection(TiXmlElement *element);
        void loadSubSpawnPoint(TiXmlElement *element);
        void loadGroundElement(TiXmlElement *element);
        void loadDrawGroundElement(TiXmlElement *element);

        void loadRandomModifierValue(TiXmlElement *element, int index, WorldGenerator_SpawnPoint_Modifier &modifier);

        glm::vec4 generateRandomValue(WorldGenerator_SpawnPoint_Modifier &modifier, pou::RNGenerator *rng);

        void spawnCharacter(CharacterModelAsset *characterModel, glm::vec2 worldPos, float pointRotation,
                            pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                            bool generateCharacters, pou::RNGenerator *rng);

        void spawnSprite(pou::SpriteModel *spriteModel, glm::vec2 worldPos, float pointRotation,
                         pou::SceneNode *targetNode, pou::RNGenerator *rng);

        void spawnPrefab(PrefabAsset *prefabAsset, glm::vec2 worldPos, float pointRotation,
                         pou::SceneNode *targetNode, GameWorld_Sync *syncComponent,
                         bool generateCharacters, pou::RNGenerator *rng);

        void spawnPathConnection(WorldGenerator_SpawnPoint_PathConnection &pathConnection, glm::vec2 worldPos,
                                 pou::RNGenerator *rng);

        void spawnSubSpawn(WorldGenerator_SpawnPoint_SubSpawn &subSpawn, glm::vec2 worldPos,
                                 pou::RNGenerator *rng);

        void applyRandomModifiers(pou::SceneNode *targetNode, float pointRotation, pou::RNGenerator *rng);


    private:
        WorldGenerator *m_worldGenerator;
        std::string m_fileDirectory;

        std::vector<CharacterSpawn>         m_characterSpawnModels;
        std::vector<pou::SpriteModel*>      m_spriteModelAssets;
        std::vector<PrefabAsset*>           m_prefabAssets;
        std::vector<WorldGenerator_SpawnPoint_Ground> m_groundElements;

        std::vector<WorldGenerator_SpawnPoint_PathConnection>   m_pathConnections;
        std::vector<WorldGenerator_SpawnPoint_SubSpawn>         m_subSpawnPoints;

        WorldGenerator_SpawnPoint_Parameters m_parameters;
        WorldGenerator_SpawnPoint_Modifier m_randomModifiers[NBR_WorldGenerator_SpawnPoint_ModifierTypes];

        glm::ivec2 m_gridSize;
        float m_radius;

        bool m_hasSpawn;
};

#endif // WORLDGENERATOR_CHARACTERSPAWN_H
