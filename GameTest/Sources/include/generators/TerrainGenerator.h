#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "world/WorldNode.h"
#include "world/GameWorld_Sync.h"
#include "assets/TerrainLayerModelAsset.h"

#include "PouEngine/tools/RNGesus.h"

class TerrainGenerator
{
    public:
        TerrainGenerator();
        virtual ~TerrainGenerator();

        ///Add Load From XML, etc

        void generatorOnNode(std::shared_ptr<WorldNode> targetNode, int seed, GameWorld_Sync *syncComponent = nullptr);

    protected:
        size_t getGridValue(int x, int y);

        void generateGrid();
        void spawnLayerElement(int x, int y, size_t layer, float spawnProbability, float expandProbability);

        void generateSprites(int x, int y,
                             std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent);
        //void generateSprite(std::list<TerrainGenerator_LayerModelElement> *listModel,
        //                    std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent);


    private:
        pou::RNGenerator m_rng;

        glm::vec2 m_tileSize;
        glm::vec2 m_terrainSize;
        glm::vec2 m_gridSize;

        //int m_nbrLayers;
        std::list<TerrainLayerModelAsset*> m_layerModels;

        std::vector<size_t> m_generatingGrid;

};

#endif // TERRAINGENERATOR_H
