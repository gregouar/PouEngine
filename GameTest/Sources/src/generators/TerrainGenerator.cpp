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

TerrainGenerator::TerrainGenerator() :
    m_rng(nullptr)
{
}

TerrainGenerator::~TerrainGenerator()
{
    //dtor
}

/*bool TerrainGenerator::loadFromFile(const std::string &filePath)
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
}*/

bool TerrainGenerator::loadFromXML(TiXmlHandle *hdl, const std::string &fileDirectory)
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
    this->loadGroundLayer(element, nullptr, fileDirectory);

    return loaded;
}

void TerrainGenerator::generatesOnNode(pou::SceneNode *targetNode, pou::RNGenerator *rng)
{
    m_rng = rng;

    this->generateGrid();
    this->decreasesGridNoise();
    this->decreasesGridNoise();

    auto centeringShift = m_terrainSize * m_tileSize * 0.5f;

    for(auto x = 0 ; x < m_terrainSize.x ; x++)
    for(auto y = 0 ; y < m_terrainSize.y ; y++)
    {
        auto tileNode = std::make_shared<pou::SceneNode>();
        tileNode->transform()->setPosition(x * m_tileSize.x, y * m_tileSize.y);
        tileNode->transform()->move(-centeringShift);
        this->generateSprites(x, y, tileNode.get());

        targetNode->addChildNode(tileNode);
    }
}

void TerrainGenerator::addSpawnOnlyZone(const TerrainGenerator_SpawnOnlyZone &zone)
{
    m_spawnOnlyZones.push_back(zone);
}

/*const std::string &TerrainGenerator::getFilePath()
{
    return m_filePath;
}

int TerrainGenerator::getGeneratingSeed()
{
    return m_generatingSeed;
}*/

glm::vec2 TerrainGenerator::getGridSize()
{
    return m_gridSize;
}

glm::vec2 TerrainGenerator::getTileSize()
{
    return m_tileSize;
}


glm::vec2 TerrainGenerator::getExtent()
{
    return this->getGridSize() * this->getTileSize();
}

const TerrainGenerator_GroundLayer *TerrainGenerator::getGroundLayer(const std::string &name)
{
    for(auto it = m_groundLayers.begin() ; it != m_groundLayers.end() ; ++it)
    {
        if(it->name == name)
            return &(*it);
    }
    return (nullptr);
}

TerrainGenerator_SpawnType TerrainGenerator::getSpawnType(glm::vec2 worldPos)
{
    auto gridPos = this->worldToGridPosition(worldPos);
    return this->getSpawnType(gridPos.x, gridPos.y);
}

TerrainGenerator_SpawnType TerrainGenerator::getSpawnType(int x, int y)
{
    if(x < 0 || y < 0 || x >= m_gridSize.x || y >= m_gridSize.y)
        return (TerrainGenerator_SpawnType_None);
    return m_generatingGrid[y * m_gridSize.x + x].spawnType;
}

void TerrainGenerator::setSpawnType(glm::vec2 worldPos, TerrainGenerator_SpawnType spawnType)
{
    auto gridPos = this->worldToGridPosition(worldPos);
    this->setSpawnType(gridPos.x, gridPos.y, spawnType);
}

void TerrainGenerator::setSpawnType(int x, int y, TerrainGenerator_SpawnType spawnType)
{
    if(x < 0 || y < 0 || x >= m_gridSize.x || y >= m_gridSize.y)
        return;
    m_generatingGrid[y * m_gridSize.x + x].spawnType = spawnType;
}

void TerrainGenerator::setGroundLayer(glm::vec2 worldPos, const TerrainGenerator_GroundLayer *groundLayer)
{
    auto gridPos = this->worldToGridPosition(worldPos);
    this->setGroundLayer(gridPos.x, gridPos.y, groundLayer);
}
void TerrainGenerator::setGroundLayer(int x, int y, const TerrainGenerator_GroundLayer *groundLayer)
{
    if(x < 0 || y < 0 || x >= m_gridSize.x || y >= m_gridSize.y)
        return;
    m_generatingGrid[y * m_gridSize.x + x].groundLayer = groundLayer;
    m_generatingGrid[y * m_gridSize.x + x].preventGroundSpawning = true;
}

glm::vec2 TerrainGenerator::gridToWorldPosition(int x, int y)
{
    return glm::vec2(x,y) * m_tileSize - m_gridSize * m_tileSize * 0.5f;
}

glm::ivec2 TerrainGenerator::worldToGridPosition(glm::vec2 worldPos)
{
    worldPos += (m_gridSize + glm::vec2(1)) * m_tileSize * 0.5f;
    return glm::floor(worldPos/m_tileSize);
}

const TerrainGenerator_GroundLayer* TerrainGenerator::getGridGroundLayer(int x, int y)
{
    if(x < 0 || y < 0 || x >= m_gridSize.x || y >= m_gridSize.y)
        return (nullptr);
    return m_generatingGrid[y * m_gridSize.x + x].groundLayer;
}

size_t TerrainGenerator::getGridDepth(int x, int y)
{
    if(x < 0 || y < 0 || x >= m_gridSize.x || y >= m_gridSize.y)
        return (0);
    if(!m_generatingGrid[y * m_gridSize.x + x].groundLayer)
        return (0);
    return m_generatingGrid[y * m_gridSize.x + x].groundLayer->depth;
}

///
///Protected
///

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


bool TerrainGenerator::loadGroundLayer(TiXmlElement *element, TerrainGenerator_GroundLayer *parentLayer, const std::string &fileDirectory)
{
    bool r = true;

    auto pathAtt = element->Attribute("path");
    if(pathAtt == nullptr)
        return (false);

    auto layerModel = pou::AssetHandler<TerrainLayerModelAsset>::loadAssetFromFile(fileDirectory+std::string(pathAtt));
    if(!layerModel)
        return (false);

    m_groundLayers.emplace_back();
    auto *groundLayer = &m_groundLayers.back();
    groundLayer->parentLayer = parentLayer;
    groundLayer->layerModel  = layerModel;


    auto nameAtt = element->Attribute("name");
    if(nameAtt)
        groundLayer->name = std::string(nameAtt);

    bool occulting = true;

    auto att = element->Attribute("occulting");
    if(att)
        occulting = pou::Parser::parseBool(std::string(att));
    groundLayer->occulting = occulting;

    if(parentLayer)
        groundLayer->depth = parentLayer->depth + 1; //(int)occulting;
    else
        groundLayer->depth = 0;

    groundLayer->spawnPointSparsity = 1;
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
        if(!this->loadGroundLayer(childElement, groundLayer, fileDirectory))
            r = false;

        childElement = childElement->NextSiblingElement("groundLayer");
    }

    return r;
}



std::pair<bool, bool> TerrainGenerator::lookForParentLayer(int x, int y, TerrainGenerator_GroundLayer *lookedLayer)
{
    auto gridValue = this->getGridGroundLayer(x,y);
   // if(gridValue == lookedLayer)
     //   return {true, true};

    bool occluded = false;

    while(gridValue /*&& !gridValue->occulting*/)
    {
        if(gridValue == lookedLayer)
            return {!occluded, true};

        if(gridValue->occulting)
            occluded = true;

        gridValue = gridValue->parentLayer;
    }
    return {false, false};
}

/*bool TerrainGenerator::lookForNonOccludedLayer(int x, int y, TerrainGenerator_GroundLayer *layer)
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
}*/


/*void TerrainGenerator::printGrid()
{
    for(auto y = 0 ; y < m_gridSize.y ; ++y)
    {
        for(auto x = 0 ; x < m_gridSize.x ; ++x)
        {
            if(m_generatingGrid[y * m_gridSize.x + x])
                std::cout<<m_generatingGrid[y * m_gridSize.x + x]->depth;
            else
                std::cout<<"#";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}*/

void TerrainGenerator::resetGrid()
{
    assert(!m_groundLayers.empty());

    auto *bottomLayer = &m_groundLayers.front();
    m_generatingGrid.resize(m_gridSize.x * m_gridSize.y, TerrainGenerator_GridCell(bottomLayer));

    for(auto spawnOnlyZone : m_spawnOnlyZones)
    {
        const TerrainGenerator_GroundLayer *groundType = nullptr;
        if(!spawnOnlyZone.forceGroundType.empty())
            groundType = this->getGroundLayer(spawnOnlyZone.forceGroundType);

        for(int y = 0 ; y < spawnOnlyZone.gridExtent.y ; ++y)
        for(int x = 0 ; x < spawnOnlyZone.gridExtent.x ; ++x)
        {
            auto yp = y + spawnOnlyZone.gridPosition.y;
            auto xp = x + spawnOnlyZone.gridPosition.x;

            if(xp < 0 || yp < 0 || xp >= m_gridSize.x || yp >= m_gridSize.y)
                continue;

            auto &gridElement = m_generatingGrid[yp * m_gridSize.x + xp];
            gridElement.spawnType = std::min(gridElement.spawnType, spawnOnlyZone.spawnType);
            if(groundType)
            {
                gridElement.groundLayer = groundType;
                gridElement.preventGroundSpawning = true;
            }
        }
    }
}

void TerrainGenerator::generateGrid()
{
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
}

void TerrainGenerator::decreasesGridNoise()
{
    std::vector<TerrainGenerator_GridCell> newGrid;
    newGrid.resize(m_generatingGrid.size());

    ///NEED TO OPTIMIZE
    for(auto y = 1 ; y < m_gridSize.y-1 ; ++y)
    for(auto x = 1 ; x < m_gridSize.x-1 ; ++x)
    {
        std::map<const TerrainGenerator_GroundLayer*, int> layerTypes;

        newGrid[y * m_gridSize.x + x].spawnType = m_generatingGrid[y * m_gridSize.x + x].spawnType;
        newGrid[y * m_gridSize.x + x].preventGroundSpawning = m_generatingGrid[y * m_gridSize.x + x].preventGroundSpawning;

        if(newGrid[y * m_gridSize.x + x].preventGroundSpawning)
        {
            newGrid[y * m_gridSize.x + x].groundLayer = m_generatingGrid[y * m_gridSize.x + x].groundLayer;
            continue;
        }

        for(auto xp = x - 1 ; xp <= x + 1 ; ++xp)
        for(auto yp = y - 1 ; yp <= y + 1 ; ++yp)
        {
            auto oldGridValue = m_generatingGrid[yp * m_gridSize.x + xp].groundLayer;

            while(oldGridValue)
            {
                auto layerTypesIt = layerTypes.lower_bound(oldGridValue);
                if(layerTypesIt != layerTypes.end() && layerTypesIt->first == oldGridValue) //!(layerTypes.key_comp()(oldGridValue, layerTypesIt->first)))
                    layerTypesIt->second++;
                else
                    layerTypes.insert(layerTypesIt, {oldGridValue, 1});

                oldGridValue = oldGridValue->parentLayer;
            }
        }

        //int maxAmout = 0;
        size_t maxDepth = 0;
        const TerrainGenerator_GroundLayer* maxGroundLayer = nullptr;

        for(auto it : layerTypes)
        {
            if(it.first)
            if(it.second >= 5 && it.first->depth >= maxDepth)
            {
                maxDepth = it.first->depth;
                maxGroundLayer = it.first;
            }

            /*if(it.second > maxAmout)
            {
                maxAmout = it.second;
                maxGroundLayer = it.first;
            }*/
        }

        newGrid[y * m_gridSize.x + x].groundLayer = maxGroundLayer;
    }


    newGrid.swap(m_generatingGrid);
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

        auto &gridElement = m_generatingGrid[pos.y * m_gridSize.x + pos.x];

        if(gridElement.preventGroundSpawning)
            continue;

        if(gridElement.groundLayer !=  groundLayer->parentLayer)
            continue;

        float spawnRandValue = pou::RNGesus::uniformFloat(0,1,m_rng);
        if(spawnRandValue > spawnProbability)
            continue;

        spawnProbability = groundLayer->expandProbability; //After first try, we want to use expandProbability

        gridElement.groundLayer = groundLayer;

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

void TerrainGenerator::generateSprites(int x, int y, pou::SceneNode *targetNode)
{
    for(auto &groundLayer : m_groundLayers)
    {
        //auto layerDepth = groundLayer.depth;

        /*if(this->getGridDepth(x,    y)      < layerDepth
        && this->getGridDepth(x+1,  y)      < layerDepth
        && this->getGridDepth(x,    y+1)    < layerDepth
        && this->getGridDepth(x+1,  y+1)    < layerDepth)
            break;*/

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

        bool isPresent[4], isNotOccluded[4];

        auto result = this->lookForParentLayer(x,y,&groundLayer);
        isNotOccluded[0]    = result.first;
        isPresent[0]        = result.second;

        result = this->lookForParentLayer(x+1,y,&groundLayer);
        isNotOccluded[1]    = result.first;
        isPresent[1]        = result.second;

        result = this->lookForParentLayer(x,y+1,&groundLayer);
        isNotOccluded[2]    = result.first;
        isPresent[2]        = result.second;

        result = this->lookForParentLayer(x+1,y+1,&groundLayer);
        isNotOccluded[3]    = result.first;
        isPresent[3]        = result.second;

        if(!isNotOccluded[0] && !isNotOccluded[1] && !isNotOccluded[2] && !isNotOccluded[3])
            continue;


        /* bool boolGrid[4] = {
             (this->getGridDepth(x,    y)     > layerDepth) | this->lookForNonOccludedLayer(x,   y,  &groundLayer),
             (this->getGridDepth(x+1,  y)     > layerDepth) | this->lookForNonOccludedLayer(x+1, y,  &groundLayer),
             (this->getGridDepth(x,    y+1)   > layerDepth) | this->lookForNonOccludedLayer(x,   y+1,&groundLayer),
             (this->getGridDepth(x+1,  y+1)   > layerDepth) | this->lookForNonOccludedLayer(x+1, y+1,&groundLayer)
         };

        int boolGridValue = (boolGrid[0]<<3) | (boolGrid[1]<<2)| (boolGrid[2]<<1) | boolGrid[3];
        */

        int boolGridValue = (isPresent[0]<<3) | (isPresent[1]<<2)| (isPresent[2]<<1) | isPresent[3];

        auto tileModel = groundLayer.layerModel->getTileModels((TerrainGenerator_BorderType)boolGridValue);
        if(!tileModel)
            continue;

        auto sprite = tileModel->generateSprite(x+y, m_rng);//groundLayer.layerModel->generateSprite((TerrainGenerator_BorderType)boolGridValue, &m_rng);
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



