#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "world/WorldNode.h"
#include "world/GameWorld_Sync.h"

enum TerrainGenerator_BorderType
{
    TerrainGenerator_BorderType_Blank = 0,
    TerrainGenerator_BorderType_SmallCorner_TL,
    TerrainGenerator_BorderType_SmallCorner_TR,
    TerrainGenerator_BorderType_Side_T,
    TerrainGenerator_BorderType_SmallCorner_BL,
    TerrainGenerator_BorderType_Side_L,
    TerrainGenerator_BorderType_Diag_BL_TR,
    TerrainGenerator_BorderType_BigCorner_BR,
    TerrainGenerator_BorderType_SmallCorner_BR,
    TerrainGenerator_BorderType_Diag_TL_BR,
    TerrainGenerator_BorderType_Side_R,
    TerrainGenerator_BorderType_BigCorner_BL,
    TerrainGenerator_BorderType_Side_B,
    TerrainGenerator_BorderType_BigCorner_TR,
    TerrainGenerator_BorderType_BigCorner_TL,
    TerrainGenerator_BorderType_Fill,
    NBR_BORDER_TYPES,
};

///..or this ?
struct TerrainGenerator_LayerModelElement
{
    float probability;
    std::shared_ptr<WorldSprite> sprite;
};


///Why not turn this into a class...
struct TerrainGenerator_LayerModel
{
    float spawnProbability;
    float expandProbability;

    pou::SpriteSheetAsset *spriteSheet;

    std::list<TerrainGenerator_LayerModelElement> elements[16];

    /*std::list<TerrainGenerator_LayerModelElement> elements_fill;
    std::list<TerrainGenerator_LayerModelElement> elements_border_u;
    std::list<TerrainGenerator_LayerModelElement> elements_border_l;
    std::list<TerrainGenerator_LayerModelElement> elements_border_d;
    std::list<TerrainGenerator_LayerModelElement> elements_border_r;
    std::list<TerrainGenerator_LayerModelElement> elements_corner_ul;
    std::list<TerrainGenerator_LayerModelElement> elements_corner_ur;
    std::list<TerrainGenerator_LayerModelElement> elements_corner_dr;
    std::list<TerrainGenerator_LayerModelElement> elements_corner_dl;*/
};

class TerrainGenerator
{
    public:
        TerrainGenerator();
        virtual ~TerrainGenerator();

        ///Add Load From XML, etc

        void generatorOnNode(std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent = nullptr);

    protected:
        int getGridValue(int x, int y);

        void generateGrid();
        void generateSprites(int x, int y,
                             std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent);
        void generateSprite(std::list<TerrainGenerator_LayerModelElement> *listModel,
                            std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent);


    private:
        glm::vec2 m_tileSize;
        glm::vec2 m_terrainSize;

        int m_nbrLayers;
        std::list<TerrainGenerator_LayerModel> m_layerModels;

        std::vector<int> m_generatingGrid;

};

#endif // TERRAINGENERATOR_H
