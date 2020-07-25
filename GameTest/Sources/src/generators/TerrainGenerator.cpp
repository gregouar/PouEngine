#include "generators/TerrainGenerator.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"
#include "PouEngine/tools/Parser.h"
#include "PouEngine/tools/Logger.h"

#include <stack>


///
///TerrainGenerator
///

TerrainGenerator::TerrainGenerator()
{
}

TerrainGenerator::~TerrainGenerator()
{
    //dtor
}

bool TerrainGenerator::loadFromFile(const std::string &filePath)
{
    TiXmlDocument file(filePath.c_str());

    if(!file.LoadFile())
    {
        pou::Logger::error("Cannot load terrain model from file: "+filePath);
        std::ostringstream errorReport;
        errorReport << "Because: "<<file.ErrorDesc();
        pou::Logger::error(errorReport);
        return (false);
    }

    TiXmlHandle hdl(&file);
    hdl = hdl.FirstChildElement();

    m_filePath = filePath;
    m_fileDirectory = pou::Parser::findFileDirectory(m_filePath);

    return this->loadFromXML(&hdl);
}

void TerrainGenerator::generatesOnNode(std::shared_ptr<WorldNode> targetNode, int seed, GameWorld_Sync *syncComponent)
{
    m_generatingSeed = seed;
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

const std::string &TerrainGenerator::getFilePath()
{
    return m_filePath;
}

int TerrainGenerator::getGeneratingSeed()
{
    return m_generatingSeed;
}

///
///Protected
///
bool TerrainGenerator::loadFromXML(TiXmlHandle *hdl)
{
    bool loaded = true;

    if(hdl == nullptr)
        return (false);

    TiXmlElement* element;

    element = hdl->FirstChildElement("parameters").Element();
    if(!element)
        return (false);
    this->loadParameters(element);


    element = hdl->FirstChildElement("groundLayer").Element();
    if(!element)
        return (false);
    this->loadGroundLayer(element, nullptr);

    return loaded;

}

bool TerrainGenerator::loadParameters(TiXmlElement *element)
{
    auto e = element->FirstChildElement("tileSize");
    if(e != nullptr)
    {
        if(e->Attribute("x") != nullptr)
            m_tileSize.x = pou::Parser::parseFloat(std::string(e->Attribute("x")));
        if(e->Attribute("y") != nullptr)
            m_tileSize.y = pou::Parser::parseFloat(std::string(e->Attribute("y")));
    }

    e = element->FirstChildElement("terrainSize");
    if(e != nullptr)
    {
        if(e->Attribute("x") != nullptr)
            m_terrainSize.x = pou::Parser::parseInt(std::string(e->Attribute("x")));
        if(e->Attribute("y") != nullptr)
            m_terrainSize.y = pou::Parser::parseInt(std::string(e->Attribute("y")));

        m_gridSize = m_terrainSize + glm::vec2(1);
    }

    return (true);
}


bool TerrainGenerator::loadGroundLayer(TiXmlElement *element, TerrainGenerator_GroundLayer *parentLayer)
{
    bool r = true;

    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto layerModel = pou::AssetHandler<TerrainLayerModelAsset>::loadAssetFromFile(m_fileDirectory+std::string(pathAtt));
    if(!layerModel)
        return (false);

    m_groundLayers.push_back({});
    auto *groundLayer = &m_groundLayers.back();
    groundLayer->parentLayer = parentLayer;
    groundLayer->layerModel  = layerModel;


    bool occulting = true;

    auto att = element->Attribute("occulting");
    if(att)
        occulting = pou::Parser::parseBool(std::string(att));
    groundLayer->occulting = occulting;

    if(parentLayer)
        groundLayer->depth = parentLayer->depth + 1; //(int)occulting;
    else
        groundLayer->depth = 0;


    att = element->Attribute("spawnSparsity");
    if(att)
        groundLayer->spawnPointSparsity = pou::Parser::parseInt(std::string(att));

    {
        float probability = 0.0f;
        att = element->Attribute("spawnProbability");
        if(att)
            probability = glm::clamp(pou::Parser::parseFloat(std::string(att)), 0.0f, 1.0f);
        groundLayer->spawnProbability = probability;
    }

    {
        float probability = 0.0f;
        att = element->Attribute("expandProbability");
        if(att)
            probability = glm::clamp(pou::Parser::parseFloat(std::string(att)), 0.0f, 1.0f);
        groundLayer->expandProbability = probability;
    }

    auto child = element->FirstChildElement("groundLayer");
    if(child == nullptr)
        return r;

    TiXmlElement* childElement = child->ToElement();
    while(childElement != nullptr)
    {
        if(!this->loadGroundLayer(childElement, groundLayer))
            r = false;

        childElement = childElement->NextSiblingElement("groundLayer");
    }

    return r;
}

TerrainGenerator_GroundLayer* TerrainGenerator::getGridValue(int x, int y)
{
    return m_generatingGrid[y * m_gridSize.x + x];
}

size_t TerrainGenerator::getGridDepth(int x, int y)
{
    return m_generatingGrid[y * m_gridSize.x + x]->depth;
}

bool TerrainGenerator::lookForNonOccludedLayer(int x, int y, TerrainGenerator_GroundLayer *layer)
{
    auto gridValue = this->getGridValue(x,y);
    if(gridValue == layer)
        return (true);

    while(gridValue && !gridValue->occulting)
    {
        gridValue = gridValue->parentLayer;
        if(gridValue == layer)
            return (true);
    }
    return (false);
}

void TerrainGenerator::generateGrid()
{
    assert(!m_groundLayers.empty());

    auto *bottomLayer = &m_groundLayers.front();
    m_generatingGrid.resize(m_gridSize.x * m_gridSize.y, bottomLayer);

    if(m_groundLayers.size() <= 1)
        return;

    for(auto layerIt = std::next(m_groundLayers.begin()) ; layerIt != m_groundLayers.end() ; ++layerIt)
    {
        int spawnSparsity = layerIt->spawnPointSparsity;

        ///This is not a very good way to determine spawn points...
        for(auto y = 0 ; y < m_gridSize.y ; y += spawnSparsity)
        for(auto x = 0 ; x < m_gridSize.x ; x += spawnSparsity)
            this->spawnLayerElement(x,y,&(*layerIt));
    }

    /**if(m_layerModels.size() <= 1)
        return;

    //for(size_t l = 1 ; l < m_layerModels.size() ; ++l)
    size_t l = 1;
    for(auto layerIt = std::next(m_layerModels.begin()) ; layerIt != m_layerModels.end() ; ++layerIt, ++l)
    {
        float spawnSparsity = (*layerIt)->getSpawnPointSparsity();
        for(auto y = 0 ; y < m_gridSize.y ; y += spawnSparsity)
        for(auto x = 0 ; x < m_gridSize.x ; x += spawnSparsity)
            this->spawnLayerElement(x,y,l,(*layerIt)->getSpawnProbability(), (*layerIt)->getExpandProbability());
    }**/
}


void TerrainGenerator::spawnLayerElement(int x, int y, TerrainGenerator_GroundLayer *groundLayer)
{
    float spawnProbability = groundLayer->spawnProbability;

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

        if(m_generatingGrid[pos.y * m_gridSize.x + pos.x] !=  groundLayer->parentLayer)
            continue;

        float spawnRandValue = pou::RNGesus::uniformFloat(0,1,&m_rng);
        if(spawnRandValue > spawnProbability)
            continue;

        spawnProbability = groundLayer->expandProbability; //After first try, we want to use expandProbability

        m_generatingGrid[pos.y * m_gridSize.x + pos.x] = groundLayer;

        newSpawningList.push({pos.x-1, pos.y});
        newSpawningList.push({pos.x+1, pos.y});
        newSpawningList.push({pos.x, pos.y-1});
        newSpawningList.push({pos.x, pos.y+1});
    }
}

/**void TerrainGenerator::spawnLayerElement(int x, int y, size_t layer, float spawnProbability, float expandProbability)
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
}**/

void TerrainGenerator::generateSprites(int x, int y, std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent)
{
    for(auto &groundLayer : m_groundLayers)
    {
        auto layerDepth = groundLayer.depth;

        if(this->getGridDepth(x,    y)      < layerDepth
        && this->getGridDepth(x+1,  y)      < layerDepth
        && this->getGridDepth(x,    y+1)    < layerDepth
        && this->getGridDepth(x+1,  y+1)    < layerDepth)
            break;

        /*if(this->getGridDepth(x,    y)      > layerDepth
        && this->getGridDepth(x+1,  y)      > layerDepth
        && this->getGridDepth(x,    y+1)    > layerDepth
        && this->getGridDepth(x+1,  y+1)    > layerDepth)
            continue;

        if(this->getGridValue(x,    y)      != &groundLayer
        && this->getGridValue(x+1,  y)      != &groundLayer
        && this->getGridValue(x,    y+1)    != &groundLayer
        && this->getGridValue(x+1,  y+1)    != &groundLayer)
            continue;

        bool boolGrid[4] = {
            (this->getGridDepth(x,    y)     > layerDepth) | (this->getGridValue(x, y) == &groundLayer),
            (this->getGridDepth(x+1,  y)     > layerDepth) | (this->getGridValue(x+1, y) == &groundLayer),
            (this->getGridDepth(x,    y+1)   > layerDepth) | (this->getGridValue(x, y+1) == &groundLayer),
            (this->getGridDepth(x+1,  y+1)   > layerDepth) | (this->getGridValue(x+1, y+1) == &groundLayer)
        };*/

         bool boolGrid[4] = {
             (this->getGridDepth(x,    y)     > layerDepth) | this->lookForNonOccludedLayer(x,   y,  &groundLayer),
             (this->getGridDepth(x+1,  y)     > layerDepth) | this->lookForNonOccludedLayer(x+1, y,  &groundLayer),
             (this->getGridDepth(x,    y+1)   > layerDepth) | this->lookForNonOccludedLayer(x,   y+1,&groundLayer),
             (this->getGridDepth(x+1,  y+1)   > layerDepth) | this->lookForNonOccludedLayer(x+1, y+1,&groundLayer)
         };

        int boolGridValue = (boolGrid[0]<<3) | (boolGrid[1]<<2)| (boolGrid[2]<<1) | boolGrid[3];

        auto sprite = groundLayer.layerModel->generateSprite((TerrainGenerator_BorderType)boolGridValue, &m_rng);
        if(sprite)
        {
            targetNode->attachObject(sprite);
            /*if(syncComponent)
                syncComponent->syncElement(sprite);*/
        }
    }

    /*auto layerModelIt = m_layerModels.begin();
    for(size_t l = 0 ; l < m_layerModels.size() ; ++l, ++layerModelIt)
    {
        if(this->getGridDepth(x,    y)      < l
        && this->getGridDepth(x+1,  y)      < l
        && this->getGridDepth(x,    y+1)    < l
        && this->getGridDepth(x+1,  y+1)    < l)
            break;

        if(this->getGridDepth(x,    y)      > l
        && this->getGridDepth(x+1,  y)      > l
        && this->getGridDepth(x,    y+1)    > l
        && this->getGridDepth(x+1,  y+1)    > l)
            continue;

        bool boolGrid[4] = {
            this->getGridDepth(x,    y)     >= l,
            this->getGridDepth(x+1,  y)     >= l,
            this->getGridDepth(x,    y+1)   >= l,
            this->getGridDepth(x+1,  y+1)   >= l
        };

        int boolGridValue = (boolGrid[0]<<3) | (boolGrid[1]<<2)| (boolGrid[2]<<1) | boolGrid[3];

        auto sprite = (*layerModelIt)->generateSprite((TerrainGenerator_BorderType)boolGridValue, &m_rng);
        if(sprite)
        {
            targetNode->attachObject(sprite);
            if(syncComponent)
                syncComponent->syncElement(sprite);
        }
    }*/
}



