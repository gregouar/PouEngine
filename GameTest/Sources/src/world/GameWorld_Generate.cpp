#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"


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

        tree->setModel(treeModel);
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
        lantern->setModel(lanternModel);
        lantern->setPosition(p);
        lantern->rotate(glm::vec3(0,0,glm::linearRand(-180,180)));
        m_scene->getRootNode()->addChildNode(lantern);

        this->syncElement(lantern);
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
    m_syncPlayers.clear();

    m_camera = nullptr;

    for(auto character : m_syncCharacters)
        delete character.second;
    m_syncCharacters.clear();

    if(m_scene)
        delete m_scene;
    m_scene = nullptr;
}


void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    //worldInitMsg->localTime = m_curLocalTime;
    worldInitMsg->dayTime = (int)m_dayTime;

    this->createWorldSyncMsg(worldInitMsg, worldInitMsg->player_id, -1);
}


void GameWorld::createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, float clientTime)
{
    worldSyncMsg->clientTime    = clientTime;
    worldSyncMsg->localTime     = m_curLocalTime;

    worldSyncMsg->nodes.clear();
    for(auto it = m_syncNodes.begin() ; it != m_syncNodes.end() ; ++it)
    {
        auto nodePtr = it->second;
        if(nodePtr->getLastUpdateTime() <= clientTime)
            continue;

        worldSyncMsg->nodes.push_back({it->first, NodeSync()});
        auto &nodeSync = worldSyncMsg->nodes.back().second;

        if(nodePtr->getParent() != m_scene->getRootNode())
            nodeSync.parentNodeId = m_syncNodes.findId((pou::SceneNode*)nodePtr->getParent());
        else
            nodeSync.parentNodeId = 0;

        nodeSync.node = nodePtr;
    }
    //worldSyncMsg->nbr_nodes = worldSyncMsg->nodes.size();


    worldSyncMsg->spriteSheets.clear();
    for(auto it = m_syncTimeSpriteSheets.upper_bound(clientTime) ; it != m_syncTimeSpriteSheets.end() ; ++it)
    {
        auto spriteSheetId = it->second;
        auto spriteSheet = m_syncSpriteSheets.findElement(spriteSheetId);
        auto &spriteSheetPath = spriteSheet->getFilePath();
        worldSyncMsg->spriteSheets.push_back({spriteSheetId,spriteSheetPath});
    }
   // worldSyncMsg->nbr_spriteSheets = worldSyncMsg->spriteSheets.size();


    worldSyncMsg->spriteEntities.clear();
    for(auto it = m_syncSpriteEntities.begin() ; it != m_syncSpriteEntities.end() ; ++it)
    {
        auto spriteEntity = it->second;
        if(spriteEntity->getLastUpdateTime() <= clientTime)
            continue;

        worldSyncMsg->spriteEntities.push_back({it->first,SpriteEntitySync()});
        auto &spriteEntitySync = worldSyncMsg->spriteEntities.back().second;

        auto spriteModel = spriteEntity->getSpriteModel();
        if(spriteModel == nullptr)
            continue;

        auto spriteSheet = spriteModel->getSpriteSheet();
        if(spriteSheet == nullptr)
            continue;

        size_t spriteSheetId = m_syncSpriteSheets.findId(spriteSheet);

        size_t nodeId = 0;

        auto parentNode = spriteEntity->getParentNode();
        if(parentNode != nullptr)
            nodeId = m_syncNodes.findId(parentNode);

        spriteEntitySync.spriteEntity = spriteEntity;
        spriteEntitySync.spriteSheetId = spriteSheetId;
        spriteEntitySync.spriteId = spriteModel->getSpriteId();
        spriteEntitySync.nodeId = nodeId;
    }
    //worldSyncMsg->nbr_spriteEntities = worldSyncMsg->spriteEntities.size();


    worldSyncMsg->characterModels.clear();
    for(auto it = m_syncTimeCharacterModels.upper_bound(clientTime) ; it != m_syncTimeCharacterModels.end() ; ++it)
    {
        auto characterModelId = it->second;
        auto characterModel = m_syncCharacterModels.findElement(characterModelId);
        auto &characterModelPath = characterModel->getFilePath();
        worldSyncMsg->characterModels.push_back({characterModelId, characterModelPath});
    }
    //worldSyncMsg->nbr_characterModels = worldSyncMsg->characterModels.size();


    worldSyncMsg->characters.clear();
    for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
    {
        auto character = it->second;
        if(character->getLastCharacterUpdateTime() <= clientTime)
            continue;

        worldSyncMsg->characters.push_back({it->first, CharacterSync()});
        auto &characterSync = worldSyncMsg->characters.back().second;

        size_t characterModelId = 0;
        auto characterModel = character->getModel();
        if(characterModel != nullptr)
            characterModelId = m_syncCharacterModels.findId(characterModel);

        size_t nodeId = 0;
        nodeId = m_syncNodes.findId((pou::SceneNode*)character);

        characterSync.character = character;
        characterSync.characterModelId = characterModelId;
        characterSync.nodeId = nodeId;
    }
    //worldSyncMsg->nbr_characters = worldSyncMsg->characters.size();


    worldSyncMsg->players.clear();
    for(auto it = m_syncPlayers.begin() ; it != m_syncPlayers.end() ; ++it)
    {
        auto player = it->second;

        if(player->getLastPlayerUpdateTime() <= clientTime)
            continue;

        worldSyncMsg->players.push_back({it->first, PlayerSync()});
        auto &playerSync  = worldSyncMsg->players.back().second;

        size_t characterId = 0;
        characterId = m_syncCharacters.findId((Character*)player);

        playerSync.player = player;
        playerSync.characterId = characterId;
    }
    //worldSyncMsg->nbr_players = worldSyncMsg->players.size();

    if(clientTime != -1)
    {
        worldSyncMsg->desyncNodes.clear();
        for(auto it = m_desyncNodes.lower_bound(clientTime) ; it != m_desyncNodes.end() ; ++it)
            worldSyncMsg->desyncNodes.push_back(it->second);

        worldSyncMsg->desyncCharacters.clear();
        for(auto it = m_desyncCharacters.lower_bound(clientTime) ; it != m_desyncCharacters.end() ; ++it)
            worldSyncMsg->desyncCharacters.push_back(it->second);

        worldSyncMsg->desyncPlayers.clear();
        for(auto it = m_desyncPlayers.lower_bound(clientTime) ; it != m_desyncPlayers.end() ; ++it)
            worldSyncMsg->desyncPlayers.push_back(it->second);
    }
}

void GameWorld::generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    this->createScene();

    //Need to load this kind of info from WorldTemplate XML
    m_scene->getRootNode()->attachObject(m_sunLight);

    m_dayTime = worldInitMsg->dayTime;

    this->syncFromMsg(worldInitMsg);
    m_curLocalTime = m_lastSyncTime;

    this->createPlayerCamera(worldInitMsg->player_id);

    m_scene->update(pou::Time(0),m_curLocalTime);
}


void GameWorld::syncFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg)
{
    m_lastSyncTime = worldSyncMsg->localTime;
    if(m_curLocalTime > m_lastSyncTime)
        m_curLocalTime = m_lastSyncTime;

    for(auto &spriteSheetIt : worldSyncMsg->spriteSheets)
    {
        auto& [ spriteSheetId, spriteSheetPath ] = spriteSheetIt;
        auto *spriteSheetPtr = pou::SpriteSheetsHandler::loadAssetFromFile(spriteSheetPath);
        m_syncSpriteSheets.insert(spriteSheetId, spriteSheetPtr);
    }

    for(auto &characterModelIt : worldSyncMsg->characterModels)
    {
        auto& [ characterModelId, characterModelPath ] = characterModelIt;
        auto *characterModelPtr = CharacterModelsHandler::loadAssetFromFile(characterModelPath);
        m_syncCharacterModels.insert(characterModelId, characterModelPtr);
    }

    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;

        auto player = m_syncPlayers.findElement(playerId);
        if(player == nullptr)
        {
            if(playerSync.characterId != 0)
            {
                player = new PlayableCharacter();
                m_syncPlayers.insert(playerId, player);
                m_syncCharacters.insert(playerSync.characterId, player);
            }
            //continue;
        }

        if(player)
            player->syncFromPlayer(playerSync.player);

        delete playerSync.player;
    }

    for(auto &characterIt : worldSyncMsg->characters)
    {
        auto& [ characterId, characterSync ] = characterIt;

        //If not nullptr, then the Character is a Player
        auto characterPtr = m_syncCharacters.findElement(characterId);

        //bool isNew = false;
        if(characterPtr == nullptr)
        {
            characterPtr = new Character();
            m_syncCharacters.insert(characterId, characterPtr);
            //characterPtr = characterSync.character;
            //isNew = true;
        }
        //else
        characterPtr->syncFromCharacter(characterSync.character);

        if(characterSync.characterModelId != 0)
        {
            auto *characterModel = m_syncCharacterModels.findElement(characterSync.characterModelId);
            if(characterModel != nullptr)
                characterPtr->setModel(characterModel);
        }

        if(characterSync.nodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(characterSync.nodeId);
            if(nodePtr == nullptr)
            {
                m_scene->getRootNode()->addChildNode(characterPtr);
                m_syncNodes.insert(characterSync.nodeId, characterPtr);
            }
        }

        //if(!isNew)
        delete characterSync.character;
    }

    for(auto &node : worldSyncMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        //If not nullptr, then the Node is a Character
        auto nodePtr = m_syncNodes.findElement(nodeId);

        if(nodePtr == nullptr)
        {
            nodePtr = m_scene->getRootNode()->createChildNode();
            m_syncNodes.insert(nodeId, nodePtr);
        }

        nodePtr->syncFromNode(nodeSync.node);

        /*auto player = m_syncPlayers.findElement(1);
        if(player == nodePtr)
        std::cout<<player->getPosition().x<<" vs "<<nodeSync.node->getPosition().x<<std::endl;*/

        delete nodeSync.node;
    }

    for(auto &node : worldSyncMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        if(nodeSync.parentNodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
            auto parentptr = m_syncNodes.findElement(nodeSync.parentNodeId);
            if(parentptr)
                parentptr->addChildNode(nodePtr);
        }
    }

    for(auto &spriteEntityIt : worldSyncMsg->spriteEntities)
    {
        auto& [ spriteEntityId, spriteEntitySync ] = spriteEntityIt;

        auto spriteEntity = m_syncSpriteEntities.findElement(spriteEntityId);

        //bool isNew = false;
        if(spriteEntity == nullptr)
        {
            spriteEntity = m_scene->createSpriteEntity();
            m_syncSpriteEntities.insert(spriteEntityId, spriteEntity);
            //isNew = true;
        }

        spriteEntity->syncFrom(spriteEntitySync.spriteEntity);

        if(spriteEntitySync.spriteSheetId != 0 && spriteEntitySync.spriteId != 0)
        {
            auto *spriteSheetPtr = m_syncSpriteSheets.findElement(spriteEntitySync.spriteSheetId);
            if(spriteSheetPtr != nullptr)
            {
                auto *spriteEntityModel = spriteSheetPtr->getSpriteModel(spriteEntitySync.spriteId);
                spriteEntity->setSpriteModel(spriteEntityModel);
            }
        }

        if(spriteEntitySync.nodeId != 0)
        {
            auto *node = m_syncNodes.findElement(spriteEntitySync.nodeId);
            if(node != nullptr)
                node->attachObject(spriteEntity);
        }

        delete spriteEntitySync.spriteEntity;
    }

    for(auto desyncPlayer : worldSyncMsg->desyncPlayers)
    {
        auto playerPtr = m_syncPlayers.findElement(desyncPlayer);
        if(!playerPtr)
            continue;

        this->desyncElement(playerPtr);
        delete playerPtr;
    }

    for(auto desyncCharacter : worldSyncMsg->desyncCharacters)
    {
        auto characterPtr = m_syncCharacters.findElement(desyncCharacter);
        if(!characterPtr)
            continue;

        this->desyncElement(characterPtr);
        delete characterPtr;
    }

    for(auto desyncNode : worldSyncMsg->desyncNodes)
    {
        auto nodePtr = m_syncNodes.findElement(desyncNode);
        if(!nodePtr)
            continue;

        this->desyncElement(nodePtr);
        delete nodePtr;
    }
}


bool GameWorld::initPlayer(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);
    if(!player)
        return (false);
    //auto player = new PlayableCharacter();
    player->setLocalTime(m_curLocalTime);

    /*auto player_id = this->syncElement(player);
    if(player_id == 0)
    {
        delete player;
        return (0);
    }*/


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

    glm::vec2 pos(glm::linearRand(-100,-100), glm::linearRand(-100,100));
    player->setPosition(pos);

    std::cout<<"New player with id:"<<player_id<<std::endl;

    return player_id;
}

bool GameWorld::removePlayer(size_t player_id)
{
    auto player = m_syncPlayers.findElement(player_id);

    if(player == nullptr)
        return (false);

    this->desyncElement(player);

   // m_syncNodes.freeElement(player);
   // m_syncCharacters.freeElement(player);

   // m_scene->getRootNode()->removeChildNode(player);
    delete player;

    return m_syncPlayers.freeId(player_id);
}



