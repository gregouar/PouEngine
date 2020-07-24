#include "generators/TerrainGenerator.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"

TerrainGenerator::TerrainGenerator()
{
    ///Add Flip to sprite (not node) !

    ///For testing purposes:
    m_tileSize = glm::vec2(64,64);
    m_terrainSize = glm::vec2(100,100);
    m_nbrLayers = 2;

    auto groundSpriteSheet =  pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassSpritesheetXML.txt");

    TerrainGenerator_LayerModel dirtLayerModel;
    dirtLayerModel.spriteSheet = groundSpriteSheet;
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("dirt1_2"));
        dirtLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("dirt1_3"));
        dirtLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("dirt1_4"));
        dirtLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
    }
    m_layerModels.push_back(dirtLayerModel);



    TerrainGenerator_LayerModel grassLayerModel;
    grassLayerModel.spriteSheet = groundSpriteSheet;
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_1"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_2"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_3"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_4"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Fill].push_back({1, sprite});
    }

    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_smallcorner1"));
        sprite->setRotation(180, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_SmallCorner_TL].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_smallcorner1"));
        sprite->setRotation(-90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_SmallCorner_TR].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_smallcorner1"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_SmallCorner_BL].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_smallcorner1"));
        grassLayerModel.elements[TerrainGenerator_BorderType_SmallCorner_BR].push_back({1, sprite});
    }


    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom1"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_B].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom2"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_B].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom1"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_L].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom2"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_L].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom1"));
        sprite->setRotation(180, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_T].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom2"));
        sprite->setRotation(180, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_T].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom1"));
        sprite->setRotation(-90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_R].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bottom2"));
        sprite->setRotation(-90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Side_R].push_back({1, sprite});
    }


    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner1"));
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_TR].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner2"));
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_TR].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner1"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_BR].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner2"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_BR].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner1"));
        sprite->setRotation(180, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_BL].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner2"));
        sprite->setRotation(180, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_BL].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner1"));
        sprite->setRotation(-90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_TL].push_back({1, sprite});
        sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_bigcorner2"));
        sprite->setRotation(-90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_BigCorner_TL].push_back({1, sprite});
    }

    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_diag1"));
        grassLayerModel.elements[TerrainGenerator_BorderType_Diag_TL_BR].push_back({1, sprite});
    }
    {
        auto sprite = std::make_shared<WorldSprite>();
        sprite->setSpriteModel(groundSpriteSheet->getSpriteModel("grass1_diag1"));
        sprite->setRotation(90, false);
        grassLayerModel.elements[TerrainGenerator_BorderType_Diag_BL_TR].push_back({1, sprite});
    }


    m_layerModels.push_back(grassLayerModel);

}

TerrainGenerator::~TerrainGenerator()
{
    //dtor
}


void TerrainGenerator::generatorOnNode(std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent)
{
    this->generateGrid();

    for(auto x = 0 ; x < m_terrainSize.x-1 ; x++)
    for(auto y = 0 ; y < m_terrainSize.y-1 ; y++)
    {
        auto tileNode = std::make_shared<WorldNode>();
        tileNode->setPosition(x * m_tileSize.x, y * m_tileSize.y);
        this->generateSprites(x, y, tileNode, syncComponent);

        if(syncComponent)
            syncComponent->syncElement(tileNode);

        targetNode->addChildNode(tileNode);
    }

    /**auto *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassSpritesheetXML.txt",pou::LoadType_Now);
    if(syncComponent)
        syncComponent->syncElement(grassSheet);

    for(auto x = 0 ; x < m_terrainSize.x ; x++)
    for(auto y = 0 ; y < m_terrainSize.y ; y++)
    {
        auto grassNode = std::make_shared<WorldNode>();
        if(syncComponent)
            syncComponent->syncElement(grassNode);

        auto spriteEntity = std::make_shared<WorldSprite>();

        //int rd = glm::linearRand(0,96);
        int rd = x+y;

        if(y < 2)
        {
            int modulo = 4;
            if(abs(rd % modulo) == 0)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_2"));
            else if(abs(rd % modulo) == 1)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_1"));
            else if(abs(rd % modulo) == 2)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_3"));
            else if(abs(rd % modulo) == 3)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_4"));
        }
        else
        {
            int modulo = 3;
            if(abs(rd % modulo) == 0)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("dirt1_2"));
            else if(abs(rd % modulo) == 1)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("dirt1_3"));
            else if(abs(rd % modulo) == 2)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("dirt1_4"));
            else if(abs(rd % modulo) == 3)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("dirt2_1"));
        }



        grassNode->attachObject(spriteEntity);
        if(syncComponent)
            syncComponent->syncElement(spriteEntity);

        if(y == 2)
        {
            spriteEntity = std::make_shared<WorldSprite>();
            int modulo = 2;
            if(abs(rd % modulo) == 0)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_bottom1"));
            else if(abs(rd % modulo) == 1)
                spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_bottom2"));
            grassNode->attachObject(spriteEntity);

            if(syncComponent)
                syncComponent->syncElement(spriteEntity);
        }

        grassNode->setScale(glm::vec3(
                                glm::linearRand(0,10) > 5 ? 1 : -1,
                                1,//glm::linearRand(0,10) > 5 ? 1 : -1,
                                1));
        if(y > 2)
            grassNode->scale(glm::vec3(1,glm::linearRand(0,10) > 5 ? 1 : -1,1));


        grassNode->setPosition({x*64,y*64,0});

        targetNode->addChildNode(grassNode);
    }**/
}

///
///Protected
///

int TerrainGenerator::getGridValue(int x, int y)
{
    return m_generatingGrid[y * m_terrainSize.x + x];
}

void TerrainGenerator::generateGrid()
{
    m_generatingGrid.resize(m_terrainSize.x * m_terrainSize.y, 0);

    for(size_t y = 0 ; y < m_terrainSize.y ; ++y)
    for(size_t x = 0 ; x < m_terrainSize.x ; ++x)
        m_generatingGrid[y * m_terrainSize.x + x] = std::rand() % m_nbrLayers;

    /*for(size_t y = 0 ; y < m_terrainSize.y ; ++y)
    {
        for(size_t x = 0 ; x < m_terrainSize.x ; ++x)
            std::cout<<m_generatingGrid[y * m_terrainSize.x + x];
        std::cout<<std::endl;
    }*/
}

void TerrainGenerator::generateSprites(int x, int y, std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent)
{
    auto layerModelIt = m_layerModels.begin();
    for(auto l = 0 ; l < m_nbrLayers ; ++l, ++layerModelIt)
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

        //int boolGridValue = (((int)boolGrid)<<3) | (((int)boolGrid)<<2)| (((int)boolGrid)<<1) | ((int)boolGrid);
        int boolGridValue = (boolGrid[0]<<3) | (boolGrid[1]<<2)| (boolGrid[2]<<1) | boolGrid[3];
        this->generateSprite(&layerModelIt->elements[boolGridValue], targetNode, syncComponent);
    }
}

void TerrainGenerator::generateSprite(std::list<TerrainGenerator_LayerModelElement> *listModel,
                    std::shared_ptr<WorldNode> targetNode, GameWorld_Sync *syncComponent)
{
    if(listModel->empty())
        return;

    ///This is super inefficient, please change this ASAP
    float totalProbability = 0;
    for(auto &element : *listModel)
        totalProbability += element.probability;

    float randValue = glm::linearRand(0.0f, totalProbability);
    float curProb = 0;

    auto elementIt = listModel->begin();
    while(true)
    {
        curProb += elementIt->probability;
        if(curProb >= randValue)
            break;
        ++elementIt;
    }

    if(elementIt != listModel->end())
    {
        auto copySprite = elementIt->sprite->createCopy();
        targetNode->attachObject(copySprite);
        if(syncComponent)
            syncComponent->syncElement(std::dynamic_pointer_cast<WorldSprite>(copySprite));
    }
}


