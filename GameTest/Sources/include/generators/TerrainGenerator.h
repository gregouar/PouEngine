#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "world/GameWorld_Sync.h"
#include "assets/TerrainLayerModelAsset.h"
#include "generators/TerrainGenerator_PathGraph.h"

#include "PouEngine/scene/SceneNode.h"
#include "PouEngine/tools/RNGesus.h"

#include <stack>

enum TerrainGenerator_SpawnType
{
    TerrainGenerator_SpawnType_None,
    TerrainGenerator_SpawnType_Safe,
    TerrainGenerator_SpawnType_All,
};

struct TerrainGenerator_SpawnOnlyZone
{
    TerrainGenerator_SpawnType spawnType;

    glm::ivec2 gridPosition;
    glm::ivec2 gridExtent;

    pou::HashedString forceGroundType;
};

struct TerrainGenerator_GroundLayer
{
    TerrainGenerator_GroundLayer *parentLayer;
    TerrainLayerModelAsset *layerModel;

    pou::HashedString  name;

    bool    occulting;
    size_t  depth;
    int     smooth;

    int     spawnPointSparsity;
    float   spawnProbability;
    float   expandProbability;
};

struct TerrainGenerator_GridCell
{
    TerrainGenerator_GridCell(TerrainGenerator_GroundLayer *gl = nullptr) :
                                  groundLayer(gl),
                                  spawnType(TerrainGenerator_SpawnType_All),
                                  preventGroundSpawning(false){}

    const TerrainGenerator_GroundLayer*   groundLayer;
    TerrainGenerator_SpawnType      spawnType;
    bool                            preventGroundSpawning;

    std::unordered_set<pou::HashedString> pathTypes; ///To prevent spawning path on PoI without connection
};


class TerrainGenerator
{
    public:
        TerrainGenerator();
        virtual ~TerrainGenerator();

        //bool loadFromFile(const std::string &filePath);
        bool loadFromXML(TiXmlHandle *hdl, const std::string &fileDirectory);

        void resetGrid();
        void generatesTopology(pou::RNGenerator *rng);
        void generatesOnNode(pou::SceneNode *targetNode, GameWorld_Sync *syncComponent/*, pou::RNGenerator *rng*/);

        void addSpawnOnlyZone(const TerrainGenerator_SpawnOnlyZone &zone);
        /**void addSpawnOnlyZone(TerrainGenerator_SpawnType spawnType,
                              glm::ivec2 gridPosition,
                              glm::ivec2 gridExtent,
                              const std::string &groundLayerName = std::string());**/

        void addPathConnection(pou::HashedString pathName, glm::vec2 worldPos);

        //const std::string &getFilePath();
        //int getGeneratingSeed();

        glm::vec2 getGridSize();
        glm::vec2 getTileSize();
        glm::vec2 getExtent();
        const TerrainGenerator_GroundLayer *getGroundLayer(pou::HashedString name);
        TerrainGenerator_SpawnType getSpawnType(glm::vec2 worldPos);
        TerrainGenerator_SpawnType getSpawnType(int x, int y);

        void setSpawnType(glm::vec2 worldPos, TerrainGenerator_SpawnType spawnType);
        void setSpawnType(int x, int y, TerrainGenerator_SpawnType spawnType);

        void setGroundLayer(glm::vec2 worldPos, const TerrainGenerator_GroundLayer *groundLayer, bool preventGroundSpawning);
        void setGroundLayer(int x, int y, const TerrainGenerator_GroundLayer *groundLayer, bool preventGroundSpawning);

        void addPathType(glm::vec2 worldPos, pou::HashedString pathName);
        void addPathType(int x, int y, pou::HashedString pathName);

        glm::vec2 gridToWorldPosition(int x, int y);
        glm::ivec2 worldToGridPosition(glm::vec2 worldPos);
        const TerrainGenerator_GroundLayer* getGridGroundLayer(int x, int y);

        bool containsNoPathOrPath(int x, int y, pou::HashedString pathName);
        const std::list<std::vector<glm::ivec2> > *getRasterizedPaths(pou::HashedString pathName);

    protected:
        bool loadParameters(TiXmlElement *element);
        bool loadGroundLayer(TiXmlElement *element, TerrainGenerator_GroundLayer *parentLayer, const std::string &fileDirectory);
        void loadPath(TiXmlElement *element);

        size_t getGridDepth(int x, int y);
        //bool lookForNonOccludedLayer(int x, int y, TerrainGenerator_GroundLayer *layer);
        std::pair<bool, bool> lookForParentLayer(int x, int y, TerrainGenerator_GroundLayer *lookedLayer); //First is non occluded, second is potentially occluded

        //void printGrid();

        void generatePaths(pou::RNGenerator *rng);
        void generateGrid();
        ///void decreasesGridNoise();
        void decreaseGridNoise(TerrainGenerator_GroundLayer *groundLayer);
        //void spawnLayerElement(int x, int y, size_t layer, float spawnProbability, float expandProbability);
        void spawnLayerElement(int x, int y, TerrainGenerator_GroundLayer *groundLayer);

        void generateSpritesAndCo(int x, int y, GameWorld_Sync *syncComponent,
                                    pou::SceneNode *targetNode);
        //void generateSprite(std::list<TerrainGenerator_LayerModelElement> *listModel,
        //                    std::shared_ptr<pou::SceneNode> targetNode, GameWorld_Sync *syncComponent);


    private:
        //pou::RNGenerator m_rng;
        pou::RNGenerator *m_rng;

        //std::string m_filePath;
        //std::string m_fileDirectory;

        //int m_generatingSeed;

        glm::vec2 m_tileSize;
        glm::vec2 m_terrainSize;
        glm::vec2 m_gridSize;

        //int m_nbrLayers;
        //std::list<TerrainLayerModelAsset*> m_layerModels;
        //std::list<TerrainGenerator_GroundLayer> m_groundLayers;
        std::multimap<size_t, TerrainGenerator_GroundLayer> m_groundLayers;
        std::vector<TerrainGenerator_SpawnOnlyZone> m_spawnOnlyZones;

        std::unordered_map<pou::HashedString, TerrainGenerator_PathGraph> m_pathGraphes;

        //std::vector<size_t> m_generatingGrid;
        std::vector<TerrainGenerator_GridCell> m_generatingGrid;

};

#endif // TERRAINGENERATOR_H
