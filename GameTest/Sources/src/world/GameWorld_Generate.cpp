#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"
#include "PouEngine/renderers/SceneRenderer.h"

//For tests
#include "PouEngine/system/Clock.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "logic/GameMessageTypes.h"
#include "world/WorldMesh.h"


void GameWorld::init()
{
    this->destroy();
    this->createScene();

    m_worldRootNode = std::make_shared<WorldNode>();
    m_scene->getRootNode()->addChildNode(m_worldRootNode);
    m_syncComponent.syncElement(m_worldRootNode);

    m_worldGrid = std::make_shared<WorldGrid>();
    m_worldGrid->setQuadSize(1024);
    m_worldGrid->resizeQuad(glm::ivec2(-100), glm::ivec2(200));
    m_scene->getRootNode()->addChildNode(m_worldGrid);
    m_syncComponent.syncElement(m_worldGrid);

}

void GameWorld::generate(bool generateInThread)
{
    ///this->createScene();
    this->init();

    m_worldReady = false;

    if(generateInThread)
        m_generatingThread = std::thread(&GameWorld::generateImpl, this);
    else
        this->generateImpl();
}


void GameWorld::generateImpl()
{
    pou::Logger::write("Generating world...");

    m_dayTime = glm::linearRand(0,360);
    std::cout<< glm::linearRand(0,360)<<std::endl;

    auto loadType = pou::LoadType_Now;

    auto *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt",loadType);
    m_syncComponent.syncElement(grassSheet);

    auto *rockSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/rocksSpritesheetXML.txt",loadType);
    m_syncComponent.syncElement(rockSheet);

    for(auto x = -100 ; x < 100 ; x++)
    for(auto y = -100 ; y < 100 ; y++)
    {
        ///auto grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);
        //auto grassNode = m_worldGrid->createChildNode(x*64,y*64);
        auto grassNode = std::make_shared<WorldNode>();
        m_syncComponent.syncElement(grassNode);

        auto spriteEntity = std::make_shared<WorldSprite>();

        int rd = x+y;//glm::linearRand(0,96);
        int modulo = 4;
        if(abs(rd % modulo) == 0)
            spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_2"));
        else if(abs(rd % modulo) == 1)
            spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_1"));
        else if(abs(rd % modulo) == 2)
            spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_3"));
        else if(abs(rd % modulo) == 3)
            spriteEntity->setSpriteModel(grassSheet->getSpriteModel("grass1_4"));

        grassNode->attachObject(spriteEntity);
        m_syncComponent.syncElement(spriteEntity);

        grassNode->setPosition({x*64,y*64,0});
        grassNode->setScale(glm::vec3(
                                glm::linearRand(0,10) > 5 ? 1 : -1,
                                1,//glm::linearRand(0,10) > 5 ? 1 : -1,
                                1));

        m_worldGrid->addChildNode(grassNode);
    }

    auto treeModel = CharacterModelsHandler::loadAssetFromFile("../data/grasslands/treeXML.txt",loadType);
    m_syncComponent.syncElement(treeModel);


    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        glm::vec3 p = glm::vec3(glm::linearRand(-640.0f,640.0f),
                                glm::linearRand(-640.0f,640.0f),
                                glm::linearRand(0.0f,0.1f));

        p += glm::vec3(x*640, y*640,0);

        auto tree = std::make_shared<Character>();

        tree->createFromModel(treeModel);
        tree->setPosition(p);
        tree->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        tree->scale(glm::vec3(
                    glm::linearRand(0,10) > 5 ? 1 : -1,
                    glm::linearRand(0,10) > 5 ? 1 : -1,
                    1));
        float red = glm::linearRand(1.0,1.0);
        float green = glm::linearRand(0.9,1.0);
        float blue = green;//glm::linearRand(0.9,1.0);
        tree->setColor(glm::vec4(red,green,blue,1));
        ///m_scene->getRootNode()->addChildNode(tree);
        m_worldGrid->addChildNode(tree);

        m_syncComponent.syncElement(tree);
    }

    auto lanternModel = CharacterModelsHandler::loadAssetFromFile("../data/poleWithLantern/poleWithLanternXML.txt",loadType);
    m_syncComponent.syncElement(lanternModel);

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        glm::vec2 p = glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));

        p += glm::vec2(x*640, y*640);

        auto lantern = std::make_shared<Character>();
        lantern->createFromModel(lanternModel);
        lantern->pou::SceneNode::setPosition(p);
        lantern->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        ///m_scene->getRootNode()->addChildNode(lantern);
        m_worldGrid->addChildNode(lantern);

        m_syncComponent.syncElement(lantern);
    }

    auto duckModel = CharacterModelsHandler::loadAssetFromFile("../data/duck/duckXML.txt",loadType);
    m_syncComponent.syncElement(duckModel);

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        glm::vec2 p, pp;

        pp = glm::vec2(x*640, y*640);

        do
        {
            p = pp + glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));
        }while(glm::length(p) < 500.0f);

        auto duck = std::make_shared<Character>();
        duck->createFromModel(duckModel);
        duck->pou::SceneNode::setPosition(p);
        ///m_scene->getRootNode()->addChildNode(duck);
        m_worldGrid->addChildNode(duck);

        m_syncComponent.syncElement(duck);
    }

    //pou::MaterialAsset *wallMaterial = pou::MaterialsHandler::loadAssetFromFile("../data/wallXML.txt",loadType);

    pou::MeshAsset *wallModel = pou::MeshesHandler::loadAssetFromFile("../data/wall/wallMeshXML.txt");
    m_syncComponent.syncElement(wallModel);

    {
        glm::vec2 p;
        p = glm::vec2(0,0);

        auto wallNode = std::make_shared<WorldNode>();
        m_syncComponent.syncElement(wallNode);

        auto wallEntity = std::make_shared<WorldMesh>();
        wallEntity->setMeshModel(wallModel);
        wallEntity->setShadowCastingType(pou::ShadowCasting_All);

        wallNode->attachObject(wallEntity);
        m_syncComponent.syncElement(wallEntity);

        wallNode->setPosition(p);

        m_worldGrid->addChildNode(wallNode);
    }


    m_scene->update(pou::Time(0));
    m_worldReady = true;

    pou::Logger::write("World generated !");
}

void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    //worldInitMsg->localTime = m_curLocalTime;
    worldInitMsg->dayTime = (int)m_dayTime;
    //worldInitMsg->worldGrid_nodeId = (int)m_syncNodes.findId(m_worldGrid);
    m_syncComponent.createWorldSyncMsg(worldInitMsg, worldInitMsg->player_id, -1);
}

void GameWorld::generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg, bool generateInThread)
{
    this->init();
    m_worldReady = false;

    if(generateInThread)
        m_generatingThread = std::thread(&GameWorld::generateFromMsgImpl, this, worldInitMsg);
    else
        this->generateFromMsgImpl(worldInitMsg);
}

void GameWorld::generateFromMsgImpl(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    m_dayTime = worldInitMsg->dayTime;

    m_syncComponent.syncWorldFromMsg(worldInitMsg, worldInitMsg->player_id,0);
    ///m_curLocalTime = m_syncComponent.getLastSyncTime();

    auto player = m_syncComponent.getPlayer(worldInitMsg->player_id);
    this->createPlayerCamera(player.get());
    ///m_worldGrid->addUpdateProbe(player.get()/*->node()*/, 2048);

    m_scene->update(pou::Time(0),m_syncComponent.getLocalTime());
    m_worldReady = true;
}

bool GameWorld::initPlayer(size_t player_id, std::shared_ptr<PlayerSave> playerSave)
{
    auto player = m_syncComponent.getPlayer(player_id);
    if(!player)
        return (false);

    //pou::Clock clock;
    //clock.restart();

    ///player->update(pou::Time(0), 0);

    glm::vec2 pos(glm::linearRand(-200,200), glm::linearRand(-200,200));
    player->pou::SceneNode::setPosition(pos);

    player->update(pou::Time(0), m_syncComponent.getLocalTime());

    CharacterModelAsset *playerModel;
    if(playerSave->getPlayerType() % 3 == 0)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/char1XML.txt"/*, pou::LoadType_InThread*/);
    else if(playerSave->getPlayerType() % 3 == 1)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/mokouXML.txt");
    else
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/sithXML.txt");

    m_syncComponent.syncElement(playerModel);
    player->setModel(playerModel);

    playerSave->loadToPlayer(player.get());

    m_worldGrid->addChildNode(player/*->node()*/);
    //m_scene->getRootNode()->addChildNode(player/*->node()*/);

    ///player/*->node()*/->pou::SceneNode::setPosition(pos);

    ItemModelAsset *playerWeapon;

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/swordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,1);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/axeXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,2);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/mokouSwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,3);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/laserSwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,4);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/energySwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,5);

    player->setTeam(1);

    ///m_worldGrid->addUpdateProbe(player.get()/*->node()*/, 2048);

    GameMessage_World_NewPlayer gameMsg;
    gameMsg.player = player.get();
    pou::MessageBus::postMessage(GameMessageType_World_NewPlayer, &gameMsg);

    return (true);
}

bool GameWorld::removePlayer(size_t player_id)
{
    auto player = m_syncComponent.getPlayer(player_id);

    if(!player)
        return (false);

     /**for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
            it->second->removeFromNearbyCharacters(player);**/

    //m_worldGrid->removeChildNode(player/*->node()*/);
    player->removeFromParent();
    m_worldGrid->removeUpdateProbe(player/*->node()*/.get());
    m_syncComponent.desyncElement(player.get());

    ///return m_syncPlayers.freeId(player_id);
    return (true);
}



