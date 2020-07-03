#include "world/GameWorld_Sync.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "logic/GameMessageTypes.h"

const int    GameWorld_Sync::NODEID_BITS            = 16;
const int    GameWorld_Sync::SPRITESHEETID_BITS     = 10;
const int    GameWorld_Sync::SPRITEENTITYID_BITS    = 16;
const int    GameWorld_Sync::CHARACTERMODELSID_BITS = 12;
const int    GameWorld_Sync::CHARACTERSID_BITS      = 16;
const int    GameWorld_Sync::ITEMMODELSID_BITS      = 12;

GameWorld_Sync::GameWorld_Sync() :
    m_curLocalTime(0),
    m_lastSyncTime(-1),
    m_updatedCharactersBuffer(0)
{
    m_syncNodes.setMax(pow(2,GameWorld_Sync::NODEID_BITS));
    m_syncSpriteSheets.setMax(pow(2,GameWorld_Sync::SPRITESHEETID_BITS));
    m_syncSpriteEntities.setMax(pow(2,GameWorld_Sync::SPRITEENTITYID_BITS));
    m_syncCharacterModels.setMax(pow(2,GameWorld_Sync::CHARACTERMODELSID_BITS));
    m_syncCharacters.setMax(pow(2,GameWorld_Sync::CHARACTERSID_BITS));
    m_syncItemModels.setMax(pow(2,GameWorld_Sync::ITEMMODELSID_BITS));

    m_syncPlayers.setMax(GameWorld::MAX_NBR_PLAYERS);
    m_syncPlayers.enableIdReuse();

    pou::MessageBus::addListener(this, GameMessageType_World_NodeUpdated);
    pou::MessageBus::addListener(this, GameMessageType_World_SpriteUpdated);
    pou::MessageBus::addListener(this, GameMessageType_World_CharacterUpdated);
}

GameWorld_Sync::~GameWorld_Sync()
{

}

void GameWorld_Sync::clear()
{
    m_lastSyncTime = -1;
    m_curLocalTime = 0;

    m_syncNodes.clear();
    m_syncSpriteSheets.clear();
    m_syncSpriteEntities.clear();
    m_syncCharacterModels.clear();
    m_syncItemModels.clear();
    m_syncPlayers.clear();

    m_updatedCharacters[0].clear();
    m_updatedCharacters[1].clear();

    /*for(auto character : m_syncCharacters)
        delete character.second;*/
    m_syncCharacters.clear();
}

void GameWorld_Sync::update(const pou::Time &elapsedTime)
{
    m_curLocalTime++;

    m_updatedCharactersBuffer = !m_updatedCharactersBuffer;

    m_updatedNodes.clear();
    m_updatedSprites.clear();
    m_updatedCharacters[m_updatedCharactersBuffer].clear();

    this->processPlayerEvents();
}


void GameWorld_Sync::createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, uint32_t lastSyncTime)
{
    worldSyncMsg->lastSyncTime      = lastSyncTime;
    worldSyncMsg->localTime         = m_curLocalTime;

    worldSyncMsg->nodes.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_syncNodes.begin() ; it != m_syncNodes.end() ; ++it)
            this->createWorldSyncMsg_Node(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else {
        for(auto it = m_updatedNodes.begin() ; it != m_updatedNodes.end() ; ++it)
            this->createWorldSyncMsg_Node(*it, worldSyncMsg, player_id, lastSyncTime);
    }

    {
        worldSyncMsg->spriteSheets.clear();
        auto it = m_syncTimeSpriteSheets.upper_bound(lastSyncTime);
        if(lastSyncTime == (uint32_t)(-1))
            it = m_syncTimeSpriteSheets.begin();
        for( ; it != m_syncTimeSpriteSheets.end() ; ++it)
        {
            auto spriteSheetId = it->second;
            auto spriteSheet = m_syncSpriteSheets.findElement(spriteSheetId);
            auto &spriteSheetPath = spriteSheet->getFilePath();
            worldSyncMsg->spriteSheets.push_back({spriteSheetId,spriteSheetPath});
        }
    }

    worldSyncMsg->spriteEntities.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_syncSpriteEntities.begin() ; it != m_syncSpriteEntities.end() ; ++it)
            this->createWorldSyncMsg_Sprite(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else {
        for(auto it = m_updatedSprites.begin() ; it != m_updatedSprites.end() ; ++it)
            this->createWorldSyncMsg_Sprite(*it, worldSyncMsg, player_id, lastSyncTime);
    }

    {
        worldSyncMsg->characterModels.clear();

        auto it = m_syncTimeCharacterModels.upper_bound(lastSyncTime);
        if(lastSyncTime == (uint32_t)(-1))
            it = m_syncTimeCharacterModels.begin();

        for(; it != m_syncTimeCharacterModels.end() ; ++it)
        {
            auto characterModelId = it->second;
            auto characterModel = m_syncCharacterModels.findElement(characterModelId);
            auto &characterModelPath = characterModel->getFilePath();
            worldSyncMsg->characterModels.push_back({characterModelId, characterModelPath});
        }
    }

    worldSyncMsg->characters.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
            this->createWorldSyncMsg_Character(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else {
        for(auto it = m_updatedCharacters[m_updatedCharactersBuffer].begin() ; it != m_updatedCharacters[m_updatedCharactersBuffer].end() ; ++it)
            this->createWorldSyncMsg_Character(*it, worldSyncMsg, player_id, lastSyncTime);
    }

    {
        worldSyncMsg->itemModels.clear();

        auto it = m_syncTimeItemModels.upper_bound(lastSyncTime);
        if(lastSyncTime == (uint32_t)(-1))
            it = m_syncTimeItemModels.begin();

        for(; it != m_syncTimeItemModels.end() ; ++it)
        {
            auto itemModelId = it->second;
            auto itemModel = m_syncItemModels.findElement(itemModelId);
            auto &itemModelPath = itemModel->getFilePath();
            worldSyncMsg->itemModels.push_back({itemModelId, itemModelPath});
        }
    }

    worldSyncMsg->players.clear();
    for(auto it = m_syncPlayers.begin() ; it != m_syncPlayers.end() ; ++it)
    {
        auto player = it->second;

        if(uint32leq(player->getLastPlayerUpdateTime(), lastSyncTime)
        && (int)it->first != player_id)
            continue;

        worldSyncMsg->players.push_back({it->first, PlayerSync()});
        auto &playerSync  = worldSyncMsg->players.back().second;

        size_t characterId = 0;
        characterId = player->getCharacterSyncId();///m_syncCharacters.findId(player.get());

        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
        {
            playerSync.gearModelsId[i] = 0;

            auto *itemModel = player->getItemModel((GearType)i);
            if(itemModel)
                playerSync.gearModelsId[i] = m_syncItemModels.findId(itemModel);
        }

        playerSync.inventoryItemModelsId.resize(player->getInventorySize());
        for(size_t i = 0 ; i < playerSync.inventoryItemModelsId.size() ; ++i)
        {
            playerSync.inventoryItemModelsId[i] = 0;

            auto *itemModel = player->getItemFromInventory(i);
            if(itemModel)
                playerSync.inventoryItemModelsId[i] = m_syncItemModels.findId(itemModel);
        }

        playerSync.player = player;
        playerSync.characterId = characterId;
    }

    if(lastSyncTime != (uint32_t)(-1))
    {
        worldSyncMsg->desyncNodes.clear();
        for(auto it = m_desyncNodes.lower_bound(lastSyncTime) ; it != m_desyncNodes.end() ; ++it)
            worldSyncMsg->desyncNodes.push_back(it->second);

        worldSyncMsg->desyncCharacters.clear();
        for(auto it = m_desyncCharacters.lower_bound(lastSyncTime) ; it != m_desyncCharacters.end() ; ++it)
            worldSyncMsg->desyncCharacters.push_back(it->second);

        worldSyncMsg->desyncPlayers.clear();
        for(auto it = m_desyncPlayers.lower_bound(lastSyncTime) ; it != m_desyncPlayers.end() ; ++it)
            worldSyncMsg->desyncPlayers.push_back(it->second);
    }
}

void GameWorld_Sync::createWorldSyncMsg_Node(WorldNode *nodePtr, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(nodePtr->getLastUpdateTime(), lastSyncTime))
        return;

    worldSyncMsg->nodes.push_back({nodePtr->getNodeSyncId() /**it->first**/, NodeSync()});
    auto &nodeSync = worldSyncMsg->nodes.back().second;

    nodeSync.parentNodeId = 0;

    if(/*nodePtr->getParent() != m_scene->getRootNode()
    &&*/ uint32less(lastSyncTime, nodePtr->getLastParentUpdateTime())
    && nodePtr->getParent())
    {
        auto parentNode = (WorldNode*)nodePtr->getParent();
        nodeSync.parentNodeId = parentNode->getNodeSyncId();///m_syncNodes.findId((WorldNode*)nodePtr->getParent());
    }

    nodeSync.node = nodePtr;
}

void GameWorld_Sync::createWorldSyncMsg_Sprite(WorldSprite *spriteEntity, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(spriteEntity->getLastUpdateTime(), lastSyncTime))
        return;

    worldSyncMsg->spriteEntities.push_back({spriteEntity->getSyncId(),SpriteEntitySync()});
    auto &spriteEntitySync = worldSyncMsg->spriteEntities.back().second;

    auto spriteModel = spriteEntity->getSpriteModel();
    if(spriteModel == nullptr)
        return;

    auto spriteSheet = spriteModel->getSpriteSheet();
    if(spriteSheet == nullptr)
        return;

    size_t spriteSheetId = m_syncSpriteSheets.findId(spriteSheet);

    size_t nodeId = 0;

    auto parentNode = (WorldNode*)spriteEntity->getParentNode();
    if(parentNode != nullptr)
        nodeId = parentNode->getNodeSyncId();///m_syncNodes.findId(parentNode);

    spriteEntitySync.spriteEntity = spriteEntity;
    spriteEntitySync.spriteSheetId = spriteSheetId;
    spriteEntitySync.spriteId = spriteModel->getSpriteId();
    spriteEntitySync.nodeId = nodeId;
}

void GameWorld_Sync::createWorldSyncMsg_Character(Character *character, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(character->getLastCharacterUpdateTime(), lastSyncTime))
        return;

    worldSyncMsg->characters.push_back({character->getCharacterSyncId(), CharacterSync()});
    auto &characterSync = worldSyncMsg->characters.back().second;

    size_t characterModelId = 0;
    auto characterModel = character->getModel();
    if(characterModel != nullptr)
        characterModelId = m_syncCharacterModels.findId(characterModel);

    size_t nodeId = 0;
    nodeId = character->getNodeSyncId();///m_syncNodes.findId((WorldNode*)character);

    characterSync.character = character;
    characterSync.characterModelId = characterModelId;
    characterSync.nodeId = nodeId;
}

void GameWorld_Sync::syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT)
{
    if(uint32leq(worldSyncMsg->localTime, m_lastSyncTime))
        return;

    //if(worldSyncMsg->localTime > m_lastSyncTime)
    uint32_t deltaRTT = (uint32_t)(RTT*GameServer::TICKRATE);
    deltaRTT += pou::NetEngine::getSyncDelay();

    if(deltaRTT < 15) //To avoid too small RTT
        deltaRTT = 15;

    {

        /**uint32_t desiredLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*1),(int64_t)0);
        uint32_t desiredMaxLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*1.5),(int64_t)0);
        uint32_t desiredMinLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*.75),(int64_t)0);**/

        uint32_t desiredLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1);
        uint32_t desiredMinLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1.5);
        uint32_t desiredMaxLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*.75);


        if(m_lastSyncTime == (uint32_t)(-1))
            m_curLocalTime = desiredLocalTime;

        if(desiredMinLocalTime < m_curLocalTime || m_curLocalTime < desiredMaxLocalTime)
        {
            std::cout<<"Jump time from "<<m_curLocalTime<<" tot "<<  desiredLocalTime<<std::endl;

            m_curLocalTime = desiredLocalTime;
            m_deltaRTT = deltaRTT;
        }

        m_lastSyncTime = worldSyncMsg->localTime;

    }


    for(auto &spriteSheetIt : worldSyncMsg->spriteSheets)
    {
        auto& [ spriteSheetId, spriteSheetPath ] = spriteSheetIt;
        auto *spriteSheetPtr = pou::SpriteSheetsHandler::loadAssetFromFile(spriteSheetPath);
        this->syncElement(spriteSheetPtr, spriteSheetId);
    }

    for(auto &characterModelIt : worldSyncMsg->characterModels)
    {
        auto& [ characterModelId, characterModelPath ] = characterModelIt;
        auto *characterModelPtr = CharacterModelsHandler::loadAssetFromFile(characterModelPath);
        this->syncElement(characterModelPtr, characterModelId);
    }

    for(auto &itemModelIt : worldSyncMsg->itemModels)
    {
        auto& [ itemModelId, itemModelPath ] = itemModelIt;
        auto *itemModelPtr = ItemModelsHandler::loadAssetFromFile(itemModelPath);
        this->syncElement(itemModelPtr, itemModelId);
    }


                //std::cout<<"PlayerID :"<< clientPlayerId<<std::endl;
    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;

        auto player = m_syncPlayers.findElement(playerId);
        if(player == nullptr)
        {
                //std::cout<<"Sync Player :" <<playerId<<" with char id "<<playerSync.characterId<<std::endl;
            if(playerSync.characterId != 0)
            {
                if(playerId == (int)clientPlayerId)
                {
                    player = std::make_shared<Player>();
                   // m_worldGrid->addUpdateProbe(player/*->node()*/, 2048);
                }
                else
                {
                    player = std::make_shared<Player>(false);
                    player->disableDamageDealing();
                }

                this->syncElement(player, playerId);
                this->syncElement((std::shared_ptr<Character>)player, playerSync.characterId);

                player->setTeam(1);
            }
        }

        if(player)
            player->syncFromPlayer(playerSync.player.get());
    }

    //std::cout<<"Nbr Chars:"<<worldSyncMsg->characters.size()<<std::endl;
    for(auto &characterIt : worldSyncMsg->characters)
    {
        auto& [ characterId, characterSync ] = characterIt;

        //If not nullptr, then the Character is a Player
        auto characterPtr = m_syncCharacters.findElement(characterId);

        //std::cout<<"Sync char with id: "<<characterId<<std::endl;

        //bool isNew = false;
        if(characterPtr == nullptr)
        {
            characterPtr = std::make_shared<Character>();
            this->syncElement(characterPtr, characterId);

            characterPtr->disableInputSync();
        }

        characterPtr->syncFromCharacter(characterSync.character);
        characterPtr->setReconciliationDelay(0,m_deltaRTT*1);
        characterPtr->setMaxRewind(GameClient::MAX_PLAYER_REWIND);

        if(characterSync.characterModelId != 0)
        {
            auto *characterModel = m_syncCharacterModels.findElement(characterSync.characterModelId);
            if(characterModel != nullptr)
                characterPtr->createFromModel(characterModel);
        }

        if(characterSync.nodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(characterSync.nodeId);
            if(nodePtr == nullptr)
                this->syncElement((std::shared_ptr<WorldNode>)characterPtr, characterSync.nodeId);
        }
    }

    ///TEST
    for(auto playerIt = m_syncPlayers.begin() ; playerIt != m_syncPlayers.end() ; ++playerIt)
    {
        if(playerIt->first != clientPlayerId)
        {
            uint32_t delay = m_deltaRTT*1.5;
            playerIt->second->setReconciliationDelay(delay,0);
            playerIt->second->disableInputSync(false);
        }
        else
        {
            playerIt->second->disableSync();
            playerIt->second->setReconciliationDelay(0,0);
        }
    }
    ///


    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;
        auto player = m_syncPlayers.findElement(playerId);

        if(!player)
            continue;

        if(playerIt.first != (int)clientPlayerId)
        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
        {
            if(playerSync.gearModelsId[i] != 0)
            {
                auto *itemModel = m_syncItemModels.findElement(playerSync.gearModelsId[i]);
                if(itemModel != nullptr)
                    player->useGear(itemModel);
            }
        }

        for(size_t i = 0 ; i < playerSync.inventoryItemModelsId.size() ; ++i)
            player->addItemToInventory(m_syncItemModels.findElement(playerSync.inventoryItemModelsId[i]),i);
    }

    //std::cout<<"Sync nbr nodes:"<<worldSyncMsg->nodes.size()<<std::endl;
    for(auto &node : worldSyncMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        auto nodePtr = m_syncNodes.findElement(nodeId);

        if(nodePtr == nullptr)
        {
            nodePtr = std::make_shared<WorldNode>();
            this->syncElement(nodePtr, nodeId);
        }

        nodePtr->syncFrom(nodeSync.node);
    }

    for(auto &node : worldSyncMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        if(nodeSync.parentNodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
            if(!nodePtr)
                continue;

            auto parentptr = m_syncNodes.findElement(nodeSync.parentNodeId);
            if(parentptr)
            {
                nodePtr->removeFromParent();
                auto oldPos = nodePtr->getPosition();
                nodePtr->setPosition(nodeSync.node->getPosition());
                nodePtr->update(pou::Time(0));
                parentptr->addChildNode(nodePtr);
                nodePtr->setPosition(oldPos);
            }
        }
    }

    for(auto &spriteEntityIt : worldSyncMsg->spriteEntities)
    {
        auto& [ spriteEntityId, spriteEntitySync ] = spriteEntityIt;

        auto spriteEntity = m_syncSpriteEntities.findElement(spriteEntityId);

        //bool isNew = false;
        if(spriteEntity == nullptr)
        {
            spriteEntity = std::make_shared<WorldSprite>();
            this->syncElement(spriteEntity, spriteEntityId);
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
            auto node = m_syncNodes.findElement(spriteEntitySync.nodeId);
            if(node != nullptr)
                node->attachObject(spriteEntity);
        }
    }

    for(auto desyncPlayer : worldSyncMsg->desyncPlayers)
    {
        auto playerPtr = m_syncPlayers.findElement(desyncPlayer);
        if(!playerPtr)
            continue;

        this->desyncElement(playerPtr.get());
        playerPtr/*->node()*/->removeFromParent();
    }

    for(auto desyncCharacter : worldSyncMsg->desyncCharacters)
    {
        auto characterPtr = m_syncCharacters.findElement(desyncCharacter);
        if(!characterPtr)
            continue;

        this->desyncElement(characterPtr.get());

        this->removeFromUpdatedCharacters(characterPtr.get());
        characterPtr/*->node()*/->removeFromParent();
    }

    for(auto desyncNode : worldSyncMsg->desyncNodes)
    {
        auto nodePtr = m_syncNodes.findElement(desyncNode);
        if(!nodePtr)
            continue;

        this->desyncElement(nodePtr.get());
        nodePtr->removeFromParent();
    }

}


void GameWorld_Sync::createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                         int player_id, uint32_t lastSyncTime)
{
    playerSyncMsg->lastSyncTime   = getLastSyncTime();
    playerSyncMsg->localTime      = getLocalTime();

    auto player  = m_syncPlayers.findElement(player_id);

    PlayerSync playerSync;
    {
        size_t characterId = player->getCharacterSyncId(); ///m_syncCharacters.findId(player);

        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
        {
            playerSync.gearModelsId[i] = 0;

            auto *itemModel = player->getItemModel((GearType)i);
            if(itemModel)
                playerSync.gearModelsId[i] = m_syncItemModels.findId(itemModel);
        }

        playerSync.inventoryItemModelsId.resize(player->getInventorySize());
        for(size_t i = 0 ; i < playerSync.inventoryItemModelsId.size() ; ++i)
        {
            playerSync.inventoryItemModelsId[i] = 0;

            auto *itemModel = player->getItemFromInventory(i);
            if(itemModel)
                playerSync.inventoryItemModelsId[i] = m_syncItemModels.findId(itemModel);
        }

        playerSync.player = player;
        playerSync.characterId = characterId;
    }

    CharacterSync characterSync;
    {
        size_t nodeId = 0;
        nodeId = player->getNodeSyncId();///m_syncNodes.findId(player/*->node()*/);

        characterSync.character = player.get();
        characterSync.characterModelId = 0;
        characterSync.nodeId = nodeId;
    }

    NodeSync nodeSync;
    {
        nodeSync.node = player.get()/*->node()*/;
    }


    playerSyncMsg->nodeSync         = nodeSync;
    playerSyncMsg->characterSync    = characterSync;
    playerSyncMsg->playerSync       = playerSync;
}


void GameWorld_Sync::syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> worldSyncMsg, size_t clientPlayerId, float RTT)
{
    auto player = m_syncPlayers.findElement(clientPlayerId);

    player/*->node()*/->syncFrom(worldSyncMsg->nodeSync.node);
    player->syncFromCharacter(worldSyncMsg->characterSync.character);
    player->syncFromPlayer(worldSyncMsg->playerSync.player.get());

    for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
    {
        if(worldSyncMsg->playerSync.gearModelsId[i] != 0)
        {
            auto *itemModel = m_syncItemModels.findElement(worldSyncMsg->playerSync.gearModelsId[i]);
            if(itemModel != nullptr)
                player->useGear(itemModel);
        }
    }

    for(size_t i = 0 ; i < worldSyncMsg->playerSync.inventoryItemModelsId.size() ; ++i)
        player->addItemToInventory(m_syncItemModels.findElement(worldSyncMsg->playerSync.inventoryItemModelsId[i]),i);
}

void GameWorld_Sync::addPlayerEvent(std::shared_ptr<NetMessage_PlayerEvent> playerEventMsg/*, size_t clientPlayerId*/)
{
    m_playerEvents.insert({playerEventMsg->localTime, playerEventMsg});
}

/*void GameWorld::createAskForSyncMsg(std::shared_ptr<NetMessage_AskForWorldSync> askForWorldSyncMsg,
                                    int player_id, uint32_t lastSyncTime)
{
    askForWorldSyncMsg->lastSyncTime   = getLastSyncTime();
    askForWorldSyncMsg->localTime      = getLocalTime();


    auto actionIt = m_playerActions.begin();
    if(lastSyncTime != (uint32_t)(-1))
        actionIt = m_playerActions.lower_bound(lastSyncTime);

    while(actionIt != m_playerActions.end())
    {
        auto& [actionPlayerId, playerAction] = actionIt->second;
        if((int)actionPlayerId == player_id)
            askForWorldSyncMsg->lastPlayerActions.push_back({actionIt->first, playerAction});
        ++actionIt;
    }
}*/


size_t GameWorld_Sync::syncElement(std::shared_ptr<WorldNode> node, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncNodes.allocateId(node);
    else
        m_syncNodes.insert(forceId, node);
    node->setNodeSyncId(forceId);
    return forceId;
}

size_t GameWorld_Sync::syncElement(pou::SpriteSheetAsset *spriteSheet, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncSpriteSheets.allocateId(spriteSheet);
    else
        m_syncSpriteSheets.insert(forceId, spriteSheet);
    m_syncTimeSpriteSheets.insert({m_curLocalTime, forceId});
    return forceId;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<WorldSprite> spriteEntity, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncSpriteEntities.allocateId(spriteEntity);
    else
        m_syncSpriteEntities.insert(forceId, spriteEntity);
    spriteEntity->setSyncId(forceId);
    return forceId;
}

size_t GameWorld_Sync::syncElement(CharacterModelAsset *characterModel, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncCharacterModels.allocateId(characterModel);
    else
        m_syncCharacterModels.insert(forceId, characterModel);
    m_syncTimeCharacterModels.insert({m_curLocalTime, forceId});
    return forceId;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<Character> character, uint32_t forceId)
{
    if(forceId == 0)
    {
        forceId = m_syncCharacters.allocateId(character);
        this->syncElement((std::shared_ptr<WorldNode>)character);
    }
    else
        m_syncCharacters.insert(forceId, character);
    character->setSyncData(this, forceId);
    return forceId;
}

size_t GameWorld_Sync::syncElement(ItemModelAsset *itemModel, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncItemModels.allocateId(itemModel);
    else
        m_syncItemModels.insert(forceId, itemModel);
    m_syncTimeItemModels.insert({m_curLocalTime, forceId});
    return forceId;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<Player> player, uint32_t forceId)
{
    if(forceId == 0)
    {
        forceId = m_syncPlayers.allocateId(player);
        this->syncElement((std::shared_ptr<Character>)player);
    }
    else
        m_syncPlayers.insert(forceId, player);
    return forceId;
}


void GameWorld_Sync::desyncElement(WorldNode *node, bool noDesyncInsert)
{
    if(!noDesyncInsert)
    {
        auto id = m_syncNodes.findId(node);
        m_desyncNodes.insert({m_curLocalTime,id});
    }
    m_syncNodes.freeElement(node);
}

void GameWorld_Sync::desyncElement(Character *character, bool noDesyncInsert)
{
    this->desyncElement((WorldNode*)character, true);

    if(!noDesyncInsert)
    {
        auto id = m_syncCharacters.findId(character);
        m_desyncCharacters.insert({m_curLocalTime,id});
    }
    m_syncCharacters.freeElement(character);
}

void GameWorld_Sync::desyncElement(Player *player, bool noDesyncInsert)
{
    this->desyncElement((Character*)player, true);

    if(!noDesyncInsert)
    {
        auto id = m_syncPlayers.findId(player);
        m_desyncPlayers.insert({m_curLocalTime,id});
    }
    m_syncPlayers.freeElement(player);
}

uint32_t GameWorld_Sync::getLocalTime()
{
    return m_curLocalTime;
}

uint32_t GameWorld_Sync::getLastSyncTime()
{
    return m_lastSyncTime;
}


std::vector<Character*>    *GameWorld_Sync::getUpdatedCharacters()
{
    return &m_updatedCharacters[!m_updatedCharactersBuffer];
}

std::shared_ptr<Character>  GameWorld_Sync::getCharacter(int character_id)
{
    return m_syncCharacters.findElement(character_id);
}

std::shared_ptr<Player>     GameWorld_Sync::getPlayer(int player_id)
{
    return m_syncPlayers.findElement(player_id);
}


void GameWorld_Sync::notify(pou::NotificationSender*, int notificationType, void* data)
{
    if(notificationType == GameMessageType_World_NodeUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_NodeUpdated*>(data);
        m_updatedNodes.push_back(gameMsg->node);
    }

    if(notificationType == GameMessageType_World_SpriteUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_SpriteUpdated*>(data);
        m_updatedSprites.push_back(gameMsg->sprite);
    }

    if(notificationType == GameMessageType_World_CharacterUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_CharacterUpdated*>(data);
        m_updatedCharacters[m_updatedCharactersBuffer].push_back(gameMsg->character);
    }
}

void GameWorld_Sync::removeFromUpdatedCharacters(Character *character)
{
    for(auto i = 0 ; i < 1 ; ++i)
        for(auto it = m_updatedCharacters[i].begin() ; it != m_updatedCharacters[i].end() ; ++it)
            if(*it == character)
                it = m_updatedCharacters[i].erase(it);
}

void GameWorld_Sync::processPlayerEvents()
{
    auto upperBound = m_playerEvents.upper_bound(m_curLocalTime);

    for(auto playerEventIt = m_playerEvents.begin() ;
        playerEventIt != upperBound ;
        ++playerEventIt)
    {
        auto playerEventMsg = playerEventIt->second;
        switch((PlayerEventType)playerEventMsg->eventType)
        {
            case PlayerEventType_CharacterDamaged:{

                auto character = m_syncCharacters.findElement(playerEventMsg->syncId);
                if(!character)
                    break;

                character->damage(playerEventMsg->amount, playerEventMsg->direction);

            }break;


            case NBR_PLAYEREVENTTYPES:{
            }break;
        }
    }

    m_playerEvents.erase(m_playerEvents.begin(), upperBound);
}
