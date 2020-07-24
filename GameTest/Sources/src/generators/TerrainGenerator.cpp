#include "generators/TerrainGenerator.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"

#include <stack>


///
///TerrainGenerator
///

TerrainGenerator::TerrainGenerator()
{
    ///Add Flip to sprite (not node) !

    ///For testing purposes:
    m_tileSize = glm::vec2(64,64);
    m_terrainSize = glm::vec2(200,200);
    m_gridSize = m_terrainSize + glm::vec2(1);
   // m_nbrLayers = 2;

    auto layerModel = pou::AssetHandler<TerrainLayerModelAsset>::loadAssetFromFile("../data/grasslands/dirtLayerModelXML.txt");
    m_layerModels.push_back(layerModel);
    layerModel = pou::AssetHandler<TerrainLayerModelAsset>::loadAssetFromFile("../data/grasslands/grassLayerModelXML.txt");
    m_layerModels.push_back(layerModel);
}

TerrainGenerator::~TerrainGenerator()
{
    //dtor
}


void TerrainGenerator::generatorOnNode(std::shared_ptr<WorldNode> targetNode, int seed, GameWorld_Sync *syncComponent)
{
    m_rng.seed(seed);

    this->generateGrid();

    /**if(syncComponent)
    for(auto &layerModel : m_layerModels)
        syncComponent->syncElement(layerModel.spriteSheet);**/

    auto centeringShift = m_terrainSize * m_tileSize * 0.5f;

    for(auto x = 0 ; x < m_terrainSize.x ; x++)
    for(auto y = 0 ; y < m_terrainSize.y ; y++)
    {
        auto tileNode = std::make_shared<WorldNode>();
        tileNode->setPosition(x * m_tileSize.x, y * m_tileSize.y);
        tileNode->move(-centeringShift);
        this->generateSprites(x, y, tileNode, syncComponent);

        ///I should not sync all nodes... but use a random seed to share.
        //if(syncComponent)
          //  syncComponent->syncElement(tileNode);

        targetNode->addChildNode(tileNode);
    }
}

///
///Protected
///

size_t TerrainGenerator::getGridValue(int x, int y)
{
    return m_generatingGrid[y * m_gridSize.x + x];
}

void TerrainGenerator::generateGrid()
{
    m_generatingGrid.resize(m_gridSize.x * m_gridSize.y, 0);

    if(m_layerModels.size() <= 1)
        return;

    //for(size_t l = 1 ; l < m_layerModels.size() ; ++l)
    size_t l = 1;
    for(auto layerIt = std::next(m_layerModels.begin()) ; layerIt != m_layerModels.end() ; ++layerIt, ++l)
    {
        float spawnSparsity = (*layerIt)->getSpawnPointSparsity();
        for(auto y = 0 ; y < m_gridSize.y ; y += spawnSparsity)
        for(auto x = 0 ; x < m_gridSize.x ; x += spawnSparsity)
            this->spawnLayerElement(x,y,l,(*layerIt)->getSpawnProbability(), (*layerIt)->getExpandProbability());
    }
}

void TerrainGenerator::spawnLayerElement(int x, int y, size_t layer, float spawnProbability, float expandProbability)
{

    std::stack<glm::ivec2>  spawningList,
                            newSpawningList;
    spawningList.push({x,y});

    while(!(spawningList.empty() && newSpawningList.empty()))
    {
        if(spawningList.empty())
            spawningList.swap(newSpawningList);

        auto pos = spawningList.top();
        spawningList.pop();

        if(pos.x < 0 || pos.y < 0 || pos.x >= m_gridSize.x || pos.y >= m_gridSize.y)
            continue;

        if(m_generatingGrid[pos.y * m_gridSize.x + pos.x] >= layer)
            continue;

        float spawnRandValue = pou::RNGesus::uniformFloat(0,1,&m_rng);///glm::linearRand(0.0f, 1.0f);
        if(spawnRandValue > spawnProbability)
            continue;

        spawnProbability = expandProbability; //After first try, we want to use expandProbability

        m_generatingGrid[pos.y * m_gridSize.x + pos.x] = layer;

        newSpawningList.push({pos.x-1, pos.y});
        newSpawningList.push({pos.x+1, pos.y});
        newSpawningList.push({pos.x, pos.y-1});
        newSpawningList.push({pos.x, pos.y+1});
    }

    /*
    if(m_generatingGrid[y * m_gridSize.x + x] >= layer)
        return;

    float spawnRandValue = glm::linearRand(0.0f, 1.0f);
    if(spawnRandValue > spawnProbability)
        return;

    m_generatingGrid[y * m_gridSize.x + x] = layer;

    int xp = x;
    int yp = y;

    xp = x-1;
    if(xp >= 0 && m_generatingGrid[yp * m_gridSize.x + xp] < layer)
        this->spawnLayerElement(xp, yp, layer, expandProbability, expandProbability);

    xp = x+1;
    if(xp < m_gridSize.x && m_generatingGrid[yp * m_gridSize.x + xp] < layer)
        this->spawnLayerElement(xp, yp, layer, expandProbability, expandProbability);

    xp = x;
    yp = y-1;
    if(yp >= 0 && m_generatingGrid[yp * m_gridSize.x + xp] < layer)
        this->spawnLayerElement(xp, yp, layer, expandProbability, expandProbability);

    yp = y+1;
    if(yp < m_gridSize.y && m_generatingGrid[yp * m_gridSize.x + xp] < layer)
        this->spawnLayerElement(xp, yp, layer, expandProbability, expandProbability);*/
}

void TerrainGenerator::generateSprites(int x, int y, std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent)
{
    auto layerModelIt = m_layerModels.begin();
    for(size_t l = 0 ; l < m_layerModels.size() ; ++l, ++layerModelIt)
    {
        if(this->getGridValue(x,    y)      < l
        && this->getGridValue(x+1,  y)      < l
        && this->getGridValue(x,    y+1)    < l
        && this->getGridValue(x+1,  y+1)    < l)
            break;

        if(this->getGridValue(x,    y)      > l
        && this->getGridValue(x+1,  y)      > l
        && this->getGridValue(x,    y+1)    > l
        && this->getGridValue(x+1,  y+1)    > l)
            continue;

        bool boolGrid[4] = {
            this->getGridValue(x,    y)     >= l,
            this->getGridValue(x+1,  y)     >= l,
            this->getGridValue(x,    y+1)   >= l,
            this->getGridValue(x+1,  y+1)   >= l
        };

        int boolGridValue = (boolGrid[0]<<3) | (boolGrid[1]<<2)| (boolGrid[2]<<1) | boolGrid[3];

        auto sprite = (*layerModelIt)->generateSprite((TerrainGenerator_BorderType)boolGridValue, &m_rng);
        if(sprite)
        {
            targetNode->attachObject(sprite);
            if(syncComponent)
                syncComponent->syncElement(sprite);
        }
    }
}



