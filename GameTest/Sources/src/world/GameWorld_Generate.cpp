#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"
#include "PouEngine/renderers/SceneRenderer.h"

//For tests
#include "PouEngine/system/Clock.h"
#include "PouEngine/scene/CollisionObject.h"
#include "generators/PoissonDiskSampler.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "logic/GameMessageTypes.h"


void GameWorld::init()
{
    this->destroy();
    this->createScene();

    m_worldRootNode = std::make_shared<pou::SceneNode>();
    m_scene->getRootNode()->addChildNode(m_worldRootNode);
    m_syncComponent.syncElement(m_worldRootNode);

    m_worldGrid = std::make_shared<pou::SceneGrid>();
    m_worldGrid->setQuadSize(512);
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
    pou::Logger::write("Creating new world...");

    m_dayTime = pou::RNGesus::uniformInt(0,360);

    auto loadType = pou::LoadType_Now;

    auto worldSeed = pou::RNGesus::rand();
    m_worldGenerator.loadFromFile("../data/grasslands/grasslandsBiomeXML.txt");
    m_worldGenerator.generatesOnNode(m_worldGrid.get(), worldSeed, &m_syncComponent);

    auto airBalloonModel = CharacterModelsHandler::loadAssetFromFile("../data/airBalloon/airBalloonXML.txt",loadType);
    m_syncComponent.syncElement(airBalloonModel);
    {
        auto airBalloon = std::make_shared<Character>();
        airBalloon->createFromModel(airBalloonModel);
        //airBalloon->pou::SceneNode::setPosition(0);
        //airBalloon->transform()->rotateInDegrees(glm::vec3(0,0,pou::RNGesus::uniformInt(-2,2)*90));
        //airBalloon->transform()->move(glm::vec3(160,0,0));
        //airBalloon->transform()->rotateInDegrees(glm::vec3(0,0,pou::RNGesus::uniformInt(-180,180)));
        m_worldGrid->addChildNode(airBalloon);
        m_syncComponent.syncElement(airBalloon);
    }

    m_scene->update(pou::Time(0));
    m_worldReady = true;

    pou::Logger::write("World generated !");
}

void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    //worldInitMsg->localTime = m_curLocalTime;
    worldInitMsg->dayTime = (int)m_dayTime;

    worldInitMsg->worldGeneratorModel = m_worldGenerator.getFilePath();
    worldInitMsg->worldGeneratorSeed  = m_worldGenerator.getGeneratingSeed();

    //worldInitMsg->worldGrid_nodeId = (int)m_syncNodes.findId(m_worldGrid);
    m_syncComponent.createWorldSyncMsg(worldInitMsg, worldInitMsg->player_id, -1);
}

void GameWorld::generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg, bool useLockStepMode, bool generateInThread)
{
    this->init();
    m_worldReady = false;

    if(generateInThread)
        m_generatingThread = std::thread(&GameWorld::generateFromMsgImpl, this, worldInitMsg, useLockStepMode);
    else
        this->generateFromMsgImpl(worldInitMsg, useLockStepMode);
}

void GameWorld::generateFromMsgImpl(std::shared_ptr<NetMessage_WorldInit> worldInitMsg, bool useLockStepMode)
{
    m_dayTime = worldInitMsg->dayTime;

    m_worldGenerator.loadFromFile(worldInitMsg->worldGeneratorModel);
    m_worldGenerator.generatesOnNode(m_worldGrid.get(), worldInitMsg->worldGeneratorSeed, &m_syncComponent, false);

    m_syncComponent.syncWorldFromMsg(worldInitMsg, worldInitMsg->player_id,0,useLockStepMode);
    ///m_curLocalTime = m_syncComponent.getLastSyncTime();

    auto player = m_syncComponent.getPlayer(worldInitMsg->player_id);
    this->createPlayerCamera(player.get());
    ///m_worldGrid->addUpdateProbe(player.get()/*->node()*/, 2048);

    m_scene->update(pou::Time(0),m_syncComponent.getLocalTime());

    m_worldReady = true;

    pou::Logger::write("World generated !");
}

bool GameWorld::initPlayer(size_t player_id, std::shared_ptr<PlayerSave> playerSave)
{
    auto player = m_syncComponent.getPlayer(player_id);
    if(!player)
        return (false);

    //pou::Clock clock;
    //clock.restart();

    ///player->update(pou::Time(0), 0);

    this->respawnPlayer(player.get());

    player->update(pou::Time(0), m_syncComponent.getLocalTime());

    CharacterModelAsset *playerModel(nullptr);
    if(playerSave->getPlayerType() == 0)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/player/player_1_XML.txt"/*, pou::LoadType_InThread*/);
    else if(playerSave->getPlayerType() == 1)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/player/player_mokou_XML.txt");
    else if(playerSave->getPlayerType() == 2)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/player/player_sith_XML.txt");
    else if(playerSave->getPlayerType()  == 3)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/player/player_leather_XML.txt");
    else if(playerSave->getPlayerType()  == 4)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/player/player_bone_XML.txt");

    m_syncComponent.syncElement(playerModel);
    player->setModel(playerModel);

    playerSave->loadToPlayer(player.get());

    m_worldGrid->addChildNode(player/*->node()*/);
    //m_scene->getRootNode()->addChildNode(player/*->node()*/);

    ///player/*->node()*/->pou::SceneNode::setPosition(pos);

    ItemModelAsset *playerWeapon;

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/player/swordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,1);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/player/axeXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,2);

    /*playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/player/mokouSwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,3);*/

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/player/laserSwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,3);

    /*playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/player/energySwordXML.txt");
    m_syncComponent.syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,5);*/

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


void GameWorld::respawnPlayer(Player *player)
{
    player->resurrect();

    glm::vec2 pos(pou::RNGesus::uniformFloat(-200,200), pou::RNGesus::uniformFloat(-200,200));
    player->transform()->setPosition(pos);
}



