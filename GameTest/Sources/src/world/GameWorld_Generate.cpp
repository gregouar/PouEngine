#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

#include "net/GameServer.h"
#include "net/GameClient.h"


void GameWorld::generate()
{
    this->createScene();

    m_dayTime = glm::linearRand(0,360);

    auto *grassSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/grassXML.txt");
    this->syncElement(grassSheet);

    auto *rockSheet = pou::SpriteSheetsHandler::loadAssetFromFile("../data/grasslands/rocksSpritesheetXML.txt");
    this->syncElement(rockSheet);

    for(auto x = -10 ; x < 10 ; x++)
    for(auto y = -10 ; y < 10 ; y++)
    {
        pou::SceneNode *grassNode = m_scene->getRootNode()->createChildNode(x*64,y*64);
        this->syncElement(grassNode);

        pou::SpriteEntity *spriteEntity = nullptr;

        int rd = x+y;//glm::linearRand(0,96);
        int modulo = 4;
        if(abs(rd % modulo) == 0)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_2"));
        else if(abs(rd % modulo) == 1)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_1"));
        else if(abs(rd % modulo) == 2)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_3"));
        else if(abs(rd % modulo) == 3)
            spriteEntity = m_scene->createSpriteEntity(grassSheet->getSpriteModel("grass1_4"));

        grassNode->attachObject(spriteEntity);
        this->syncElement(spriteEntity);

        grassNode->setScale(glm::vec3(
                                glm::linearRand(0,10) > 5 ? 1 : -1,
                                1,//glm::linearRand(0,10) > 5 ? 1 : -1,
                                1));
    }

    auto treeModel = CharacterModelsHandler::loadAssetFromFile("../data/grasslands/treeXML.txt");
    this->syncElement(treeModel);

    for(auto x = -3 ; x < 3 ; x++)
    {
        glm::vec3 p = glm::vec3(glm::linearRand(-640.0f,640.0f),
                                glm::linearRand(-640.0f,640.0f),
                                glm::linearRand(0.0f,0.1f));
        if(x == 0)
            p = glm::vec3(230,0,0);

        auto tree = new Character();

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
        m_scene->getRootNode()->addChildNode(tree);

        this->syncElement(tree);
    }


    auto lanternModel = CharacterModelsHandler::loadAssetFromFile("../data/poleWithLantern/poleWithLanternXML.txt");
    this->syncElement(lanternModel);

    for(auto i = 0 ; i < 3 ; ++i)
    {
        glm::vec2 p = glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));
        auto *lantern = new Character();
        lantern->createFromModel(lanternModel);
        lantern->setPosition(p);
        lantern->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        m_scene->getRootNode()->addChildNode(lantern);

        this->syncElement(lantern);
    }



    auto duckModel = CharacterModelsHandler::loadAssetFromFile("../data/duck/duckXML.txt");
    this->syncElement(duckModel);

    for(auto i = 0 ; i < 3 ; i++)
    {
        glm::vec2 p;

        do
        {
            p = glm::vec2(glm::linearRand(-640,640), glm::linearRand(-640,640));
        }while(glm::length(p) < 500.0f);

        auto *duck = new Character();
        duck->createFromModel(duckModel);
        duck->setPosition(p);
        m_scene->getRootNode()->addChildNode(duck);

        this->syncElement(duck);
    }

    m_scene->update(pou::Time(0));
}

void GameWorld::destroy()
{
    m_curLocalTime  = 0;
    m_lastSyncTime  = -1;

    m_syncNodes.clear();
    m_syncSpriteSheets.clear();
    m_syncSpriteEntities.clear();
    m_syncCharacterModels.clear();
    m_syncItemModels.clear();
    m_syncPlayers.clear();

    m_camera = nullptr;

    for(auto character : m_syncCharacters)
        delete character.second;
    m_syncCharacters.clear();

    if(m_scene)
        delete m_scene;
    m_scene = nullptr;
}

bool GameWorld::initPlayer(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);
    if(!player)
        return (false);

    ///player->setMaxRewind(GameServer::MAX_REWIND_AMOUNT);
    ///player->setLocalTime(0);

    player->update(pou::Time(0), 0);

    glm::vec2 pos(glm::linearRand(-100,100), glm::linearRand(-100,100));
    player->setPosition(pos);

    player->update(pou::Time(0), m_curLocalTime);

    ///player->setLocalTime(m_curLocalTime);

    CharacterModelAsset *playerModel;
    if(player_id % 3 == 0)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/sithXML.txt");
    else if(player_id % 3 == 1)
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/char1XML.txt");
    else
        playerModel = CharacterModelsHandler::loadAssetFromFile("../data/char1/mokouXML.txt");

    this->syncElement(playerModel);

    player->setModel(playerModel);
    m_scene->getRootNode()->addChildNode(player);

    player->setPosition(pos);


    ItemModelAsset *playerWeapon;

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/swordXML.txt");
    this->syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,1);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/axeXML.txt");
    this->syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,2);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/mokouSwordXML.txt");
    this->syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,3);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/laserSwordXML.txt");
    this->syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,4);

    playerWeapon = ItemModelsHandler::loadAssetFromFile("../data/char1/energySwordXML.txt");
    this->syncElement(playerWeapon);
    player->addItemToInventory(playerWeapon,5);

    player->setTeam(1);

    for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
    {
        player->addToNearbyCharacters(it->second);
        it->second->addToNearbyCharacters(player);
    }

    std::cout<<"New player with id:"<<player_id<<std::endl;

    return player_id;
}

bool GameWorld::removePlayer(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);

    if(player == nullptr)
        return (false);

    this->desyncElement(player);

     for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
            it->second->removeFromNearbyCharacters(player);

    delete player;

    return m_syncPlayers.freeId(player_id);
}



