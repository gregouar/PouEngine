#include "world/GameWorld.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/renderers/SceneRenderer.h"

#include "net/GameServer.h"
#include "net/GameClient.h"

void GameWorld::createWorldInitializationMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    //worldInitMsg->localTime = m_curLocalTime;
    worldInitMsg->dayTime = (int)m_dayTime;

    worldInitMsg->worldGrid_nodeId = (int)m_syncNodes.findId(m_worldGrid);

    this->createWorldSyncMsg(worldInitMsg, worldInitMsg->player_id, -1);
}


void GameWorld::createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, uint32_t lastSyncTime)
{
    //if(clientTime == (uint32_t)(-1))
      //  clientTime = 0;

    worldSyncMsg->lastSyncTime      = lastSyncTime;
    worldSyncMsg->localTime         = m_curLocalTime;


    /**for(auto playerIt = m_syncPlayers.begin() ; playerIt != m_syncPlayers.end() ; ++playerIt)
    {
        if(playerIt->second == nullptr)
            continue;


        if(GameServer::USEREWIND)
        {
            if((int)playerIt->first == player_id)
                playerIt->second->setSyncDelay(GameServer::TICKRATE*1.5);
            else
                playerIt->second->setSyncDelay(GameServer::TICKRATE*1.5);
        }
    }**/

    worldSyncMsg->nodes.clear();
    for(auto it = m_syncNodes.begin() ; it != m_syncNodes.end() ; ++it)
    {
        auto nodePtr = it->second;
        if(uint32leq(nodePtr->getLastUpdateTime(), lastSyncTime))
            continue;

        worldSyncMsg->nodes.push_back({it->first, NodeSync()});
        auto &nodeSync = worldSyncMsg->nodes.back().second;

        if(nodePtr->getParent() != m_scene->getRootNode())
            nodeSync.parentNodeId = m_syncNodes.findId((pou::SceneNode*)nodePtr->getParent());
        else
            nodeSync.parentNodeId = 0;

        nodeSync.node = nodePtr;
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
    for(auto it = m_syncSpriteEntities.begin() ; it != m_syncSpriteEntities.end() ; ++it)
    {
        auto spriteEntity = it->second;
        if(uint32leq(spriteEntity->getLastUpdateTime(), lastSyncTime))
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
    for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
    {
        auto character = it->second;
        if(uint32leq(character->getLastCharacterUpdateTime(), lastSyncTime))
        //if(uint32less(character->getLastCharacterUpdateTime(), lastSyncTime))
            continue;

        worldSyncMsg->characters.push_back({it->first, CharacterSync()});
        auto &characterSync = worldSyncMsg->characters.back().second;

        size_t characterModelId = 0;
        auto characterModel = character->getModel();
        if(characterModel != nullptr)
            characterModelId = m_syncCharacterModels.findId(characterModel);

        size_t nodeId = 0;
        nodeId = m_syncNodes.findId((pou::SceneNode*)character.get());

        characterSync.character = character;
        characterSync.characterModelId = characterModelId;
        characterSync.nodeId = nodeId;
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
        characterId = m_syncCharacters.findId(player.get());

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

void GameWorld::generateFromMsg(std::shared_ptr<NetMessage_WorldInit> worldInitMsg)
{
    this->createScene();

    //Need to load this kind of info from WorldTemplate XML
    //m_scene->getRootNode()->attachObject(m_sunLight);

    m_dayTime = worldInitMsg->dayTime;

    m_worldGrid = std::make_shared<WorldGrid>();
    m_worldGrid->setQuadSize(1024);
    m_worldGrid->resizeQuad(glm::ivec2(-100), glm::ivec2(200));
    m_scene->getRootNode()->addChildNode(m_worldGrid);
    m_syncNodes.insert(worldInitMsg->worldGrid_nodeId, m_worldGrid);

    this->syncWorldFromMsg(worldInitMsg, worldInitMsg->player_id,0);
    m_curLocalTime = m_lastSyncTime;

    this->createPlayerCamera(worldInitMsg->player_id);

    auto music = pou::AudioEngine::createEvent("event:/Music");
    pou::AudioEngine::playEvent(music);

    m_scene->update(pou::Time(0),m_curLocalTime);
}


void GameWorld::syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT)
{
    if(uint32leq(worldSyncMsg->localTime, m_lastSyncTime))
        return;

    //if(worldSyncMsg->localTime > m_lastSyncTime)
    uint32_t deltaRTT = (uint32_t)(RTT*GameServer::TICKRATE);
    deltaRTT += pou::NetEngine::getSyncDelay();
    {

        /**uint32_t desiredLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*1),(int64_t)0);
        uint32_t desiredMaxLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*1.5),(int64_t)0);
        uint32_t desiredMinLocalTime = std::max((int64_t)worldSyncMsg->localTime - (int64_t)(deltaRTT*.75),(int64_t)0);**/

        uint32_t desiredLocalTime = std::max((int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1),(int64_t)0);
        uint32_t desiredMinLocalTime = std::max((int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1.5),(int64_t)0);
        uint32_t desiredMaxLocalTime = std::max((int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*.75),(int64_t)0);


        if(m_lastSyncTime == (uint32_t)(-1))
            m_curLocalTime = desiredLocalTime;

        if(desiredMinLocalTime < m_curLocalTime || m_curLocalTime < desiredMaxLocalTime)
        //if(abs(m_curLocalTime - m_lastSyncTime -  pou::NetEngine::getSyncDelay()) > 1)
        {
            /**auto playerActionIt = m_playerActions.find(m_curLocalTime);
            if(playerActionIt != m_playerActions.end())
                m_playerActions.insert({desiredLocalTime, playerActionIt->second});**/

            std::cout<<"Jump time from "<<m_curLocalTime<<" tot "<<  desiredLocalTime<<std::endl;

            m_curLocalTime = desiredLocalTime;//- 0.5; /*- RTT*0.5f*/// - pou::NetEngine::getSyncDelay();//GameServer::SYNCDELAY;
            m_deltaRTT = deltaRTT;
           // std::cout<<"Correcting local time !"<<std::endl;
        }

        m_lastSyncTime = worldSyncMsg->localTime;
        //std::cout<<"WorldSync!"<<std::endl;

    }


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

    for(auto &itemModelIt : worldSyncMsg->itemModels)
    {
        auto& [ itemModelId, itemModelPath ] = itemModelIt;
        auto *itemModelPtr = ItemModelsHandler::loadAssetFromFile(itemModelPath);
        m_syncItemModels.insert(itemModelId, itemModelPtr);
    }


    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;

        auto player = m_syncPlayers.findElement(playerId);
        if(player == nullptr)
        {
            if(playerSync.characterId != 0)
            {
                if(playerId == (int)clientPlayerId)
                {
                    player = std::make_shared<Player>();
                    m_worldGrid->addUpdateProbe(player, 2048);
                }
                else
                {
                    player = std::make_shared<Player>(false);
                    player->disableDamageDealing();
                }

                m_syncPlayers.insert(playerId, player);
                m_syncCharacters.insert(playerSync.characterId, player);

                /**if(playerId != (int)clientPlayerId)
                {

                }
                else
                {
                    player->disableInputSync();
                    player->setMaxRewind(GameClient::MAX_PLAYER_REWIND);
                }**/

                player->setTeam(1);

                for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
                {
                    player->addToNearbyCharacters(it->second);
                    it->second->addToNearbyCharacters(player);
                }
            }
        }

        if(player)
            player->syncFromPlayer(playerSync.player.get());
    }


    /*auto clientPlayer = m_syncPlayers.findElement(clientPlayerId);
    if(clientPlayer)
    {
        for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
        {
            clientPlayer->addToNearbyCharacters(it->second);
            it->second->addToNearbyCharacters(clientPlayer);
        }
    }*/



    for(auto &characterIt : worldSyncMsg->characters)
    {
        auto& [ characterId, characterSync ] = characterIt;

        //If not nullptr, then the Character is a Player
        auto characterPtr = m_syncCharacters.findElement(characterId);

        //bool isNew = false;
        if(characterPtr == nullptr)
        {
            characterPtr = std::make_shared<Character>();
            characterPtr->setSyncId(characterId);
            m_syncCharacters.insert(characterId, characterPtr);
            //characterPtr = characterSync.character;


            for(auto it = m_syncPlayers.begin() ; it != m_syncPlayers.end() ; ++it)
            {
                characterPtr->addToNearbyCharacters(it->second);
                it->second->addToNearbyCharacters(characterPtr);
            }

            ///TEST
            characterPtr->disableInputSync();
            ///

            /*characterPtr->addToNearbyCharacters(clientPlayer);
            clientPlayer->addToNearbyCharacters(clientPlayer);*/

        }



        //else
        characterPtr->syncFromCharacter(characterSync.character.get());
        ///characterPtr->disableDeath();

        ///TEST
        ///characterPtr->setReconciliationDelay(deltaRTT);
        //if(characterId != (int)clientPlayerId)
            //characterPtr->setReconciliationDelay(deltaRTT*2,0);
        characterPtr->setReconciliationDelay(0,m_deltaRTT*1);
        characterPtr->setMaxRewind(GameClient::MAX_PLAYER_REWIND);
        ///


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
            {
                m_scene->getRootNode()->addChildNode(characterPtr);
                m_syncNodes.insert(characterSync.nodeId, characterPtr);
            }
        }
    }

    ///TEST
    for(auto playerIt = m_syncPlayers.begin() ; playerIt != m_syncPlayers.end() ; ++playerIt)
    {
        if(playerIt->first != clientPlayerId)
        {
            uint32_t delay = m_deltaRTT*1.5;
            playerIt->second->setReconciliationDelay(delay,0);

            ///TEST
            playerIt->second->disableInputSync(false);
            ///
        }
        else
        {
            playerIt->second->disableSync();
            playerIt->second->setReconciliationDelay(0,0);
        }
            ///playerIt->second->setReconciliationDelay(0,deltaRTT*1.5);
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
            ///nodePtr = m_scene->getRootNode()->createChildNode();
            nodePtr = std::make_shared<pou::SceneNode>();
            m_syncNodes.insert(nodeId, nodePtr);
        }

        nodePtr->syncFromNode(nodeSync.node.get());
    }

    for(auto &node : worldSyncMsg->nodes)
    {
        auto& [ nodeId, nodeSync ] = node;

        if(nodeSync.parentNodeId != 0)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
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
            spriteEntity = std::make_shared<pou::SpriteEntity>();
            m_syncSpriteEntities.insert(spriteEntityId, spriteEntity);
            //isNew = true;
        }

        spriteEntity->syncFrom(spriteEntitySync.spriteEntity.get());

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
        playerPtr->removeFromParent();
    }

    for(auto desyncCharacter : worldSyncMsg->desyncCharacters)
    {
        auto characterPtr = m_syncCharacters.findElement(desyncCharacter);
        if(!characterPtr)
            continue;

        this->desyncElement(characterPtr.get());

        for(auto it = m_syncCharacters.begin() ; it != m_syncCharacters.end() ; ++it)
            it->second->removeFromNearbyCharacters(characterPtr);
        characterPtr->removeFromParent();
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


void GameWorld::createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                         int player_id, uint32_t lastSyncTime)
{
    playerSyncMsg->lastSyncTime   = getLastSyncTime();
    playerSyncMsg->localTime      = getLocalTime();

    auto player  = m_syncPlayers.findElement(player_id);

    PlayerSync playerSync;
    {
        size_t characterId = m_syncCharacters.findId(player);

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
        nodeId = m_syncNodes.findId(player);

        characterSync.character = player;
        characterSync.characterModelId = 0;
        characterSync.nodeId = nodeId;
    }

    NodeSync nodeSync;
    {
        nodeSync.node = player;
    }


    playerSyncMsg->nodeSync         = nodeSync;
    playerSyncMsg->characterSync    = characterSync;
    playerSyncMsg->playerSync       = playerSync;
}


void GameWorld::syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> worldSyncMsg, size_t clientPlayerId, float RTT)
{
    auto player = m_syncPlayers.findElement(clientPlayerId);

    player->syncFromNode(worldSyncMsg->nodeSync.node.get());
    player->syncFromCharacter(worldSyncMsg->characterSync.character.get());
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

void GameWorld::processPlayerEvent(std::shared_ptr<NetMessage_PlayerEvent> playerEventMsg, size_t clientPlayerId)
{
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



