#include "world/GameWorld_Sync.h"

#include "PouEngine/Types.h"
#include "PouEngine/assets/AssetHandler.h"
#include "PouEngine/assets/SpriteSheetAsset.h"
#include "PouEngine/assets/MeshesHandler.h"
#include "PouEngine/renderers/SceneRenderer.h"

#include "assets/PrefabAsset.h"

#include "net/GameServer.h"
#include "net/GameClient.h"
#include "logic/GameMessageTypes.h"
#include "sync/NodeSyncer.h"

const int    GameWorld_Sync::NODEID_BITS            = 16;
/**const int    GameWorld_Sync::SPRITESHEETID_BITS     = 10;
const int    GameWorld_Sync::SPRITEENTITYID_BITS    = 16;
const int    GameWorld_Sync::MESHMODELID_BITS       = 10;
const int    GameWorld_Sync::MESHENTITYID_BITS      = 16;**/
const int    GameWorld_Sync::CHARACTERMODELSID_BITS = 12;
const int    GameWorld_Sync::CHARACTERSID_BITS      = 16;
const int    GameWorld_Sync::ITEMMODELSID_BITS      = 12;
/**const int    GameWorld_Sync::PREFABASSETID_BITS     = 10;
const int    GameWorld_Sync::PREFABINSTANCEID_BITS  = 16;**/

GameWorld_Sync::GameWorld_Sync() :
    m_curLocalTime(0),
    m_lastWorldSyncTime(-1),
    m_updatedCharactersBuffer(0)
{
    m_nodeSyncers.setMax(pow(2,GameWorld_Sync::NODEID_BITS));
    /**m_syncSpriteSheets.setMax(pow(2,GameWorld_Sync::SPRITESHEETID_BITS));
    m_syncSpriteEntities.setMax(pow(2,GameWorld_Sync::SPRITEENTITYID_BITS));
    m_syncMeshModels.setMax(pow(2,GameWorld_Sync::MESHMODELID_BITS));
    m_syncMeshEntities.setMax(pow(2,GameWorld_Sync::MESHENTITYID_BITS));**/
    m_syncCharacterModels.setMax(pow(2,GameWorld_Sync::CHARACTERMODELSID_BITS));
    m_syncCharacters.setMax(pow(2,GameWorld_Sync::CHARACTERSID_BITS));
    m_syncItemModels.setMax(pow(2,GameWorld_Sync::ITEMMODELSID_BITS));

    m_syncPlayers.setMax(GameWorld::MAX_NBR_PLAYERS);
    m_syncPlayers.enableIdReuse();

    /**m_syncPrefabAssets.setMax(pow(2,GameWorld_Sync::PREFABASSETID_BITS));
    m_syncPrefabInstances.setMax(pow(2,GameWorld_Sync::PREFABINSTANCEID_BITS));**/

    pou::MessageBus::addListener(this, GameMessageType_World_NodeSyncerUpdated);
    /**pou::MessageBus::addListener(this, GameMessageType_World_SpriteUpdated);
    pou::MessageBus::addListener(this, GameMessageType_World_MeshUpdated);**/
    pou::MessageBus::addListener(this, GameMessageType_World_CharacterUpdated);
    //pou::MessageBus::addListener(this, GameMessageType_World_PrefabUpdated); //Maybe ?
}

GameWorld_Sync::~GameWorld_Sync()
{
    this->clear();
}

void GameWorld_Sync::clear()
{
    m_lastWorldSyncTime = -1;
    m_lastPlayerSyncTime.clear();
    m_curLocalTime = 0;

    m_syncNodes.clear();
    m_nodeSyncers.clear();
    /**m_syncSpriteSheets.clear();
    m_syncSpriteEntities.clear();
    m_syncMeshModels.clear();
    m_syncMeshEntities.clear();**/
    m_syncCharacterModels.clear();
    m_syncItemModels.clear();
    m_syncPlayers.clear();
    /**m_syncPrefabAssets.clear();
    m_syncPrefabInstances.clear();**/

    m_updatedNodeSyncers.clear();
    /**m_updatedSprites.clear();
    m_updatedMeshes.clear();**/
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

    m_updatedNodeSyncers.clear();
    /**m_updatedSprites.clear();
    m_updatedMeshes.clear();**/
    m_updatedCharacters[m_updatedCharactersBuffer].clear();

    for(auto nodeSyncer : m_nodeSyncers)
        nodeSyncer.second->update(elapsedTime, m_curLocalTime);

    this->processPlayerEvents();
    this->processPlayerActions();
}


void GameWorld_Sync::createWorldSyncMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, int player_id, uint32_t lastSyncTime)
{
    worldSyncMsg->lastSyncTime      = lastSyncTime;
    worldSyncMsg->localTime         = m_curLocalTime;

    worldSyncMsg->nodeSyncers.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_nodeSyncers.begin() ; it != m_nodeSyncers.end() ; ++it)
            this->createWorldSyncMsg_Node(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else {
        for(auto it = m_updatedNodeSyncers.begin() ; it != m_updatedNodeSyncers.end() ; ++it)
            this->createWorldSyncMsg_Node(*it, worldSyncMsg, player_id, lastSyncTime);
    }

    /**{
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
        worldSyncMsg->meshModels.clear();
        auto it = m_syncTimeMeshModels.upper_bound(lastSyncTime);
        if(lastSyncTime == (uint32_t)(-1))
            it = m_syncTimeMeshModels.begin();
        for( ; it != m_syncTimeMeshModels.end() ; ++it)
        {
            auto meshModelId = it->second;
            auto meshModel = m_syncMeshModels.findElement(meshModelId);
            auto &mehModelPath = meshModel->getFilePath();
            worldSyncMsg->meshModels.push_back({meshModelId,mehModelPath});
        }
    }

    worldSyncMsg->meshEntities.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_syncMeshEntities.begin() ; it != m_syncMeshEntities.end() ; ++it)
            this->createWorldSyncMsg_Mesh(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else {
        for(auto it = m_updatedMeshes.begin() ; it != m_updatedMeshes.end() ; ++it)
            this->createWorldSyncMsg_Mesh(*it, worldSyncMsg, player_id, lastSyncTime);
    }**/

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
        //|| (int)it->first == player_id)
        && (int)it->first != player_id)
            continue;

        if((int)it->first == player_id && lastSyncTime != (uint32_t)-1)
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

    /**{
        worldSyncMsg->prefabModels.clear();
        auto it = m_syncTimePrefabModels.upper_bound(lastSyncTime);
        if(lastSyncTime == (uint32_t)(-1))
            it = m_syncTimePrefabModels.begin();
        for( ; it != m_syncTimePrefabModels.end() ; ++it)
        {
            auto prefabModelId = it->second;
            auto prefabAsset = m_syncPrefabAssets.findElement(prefabModelId);
            auto &prefabPath = prefabAsset->getFilePath();
            worldSyncMsg->prefabModels.push_back({prefabModelId,prefabPath});
        }
    }

    worldSyncMsg->prefabs.clear();
    if(lastSyncTime == (uint32_t)(-1))
    {
        for(auto it = m_syncPrefabInstances.begin() ; it != m_syncPrefabInstances.end() ; ++it)
            this->createWorldSyncMsg_Prefab(it->second.get(), worldSyncMsg, player_id, lastSyncTime);
    } else { //Maybe ?
        //for(auto it = m_updatedSprites.begin() ; it != m_updatedSprites.end() ; ++it)
          //  this->createWorldSyncMsg_Sprite(*it, worldSyncMsg, player_id, lastSyncTime);
    }**/

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

void GameWorld_Sync::createWorldSyncMsg_Node(NodeSyncer *nodeSyncerPtr, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(nodeSyncerPtr->getLastUpdateTime(), lastSyncTime))
        return;
    worldSyncMsg->nodeSyncers.push_back(nodeSyncerPtr);
    /*auto &nodeSync = worldSyncMsg->nodes.back().second;

    nodeSync.parentNodeId = 0;

    if(nodeSyncerPtr->node()->getParent()
    && nodeSyncerPtr->getNodeSyncId() != 1 && nodeSyncerPtr->getNodeSyncId() != 2) //ID 1 and 2 are reserved for the rootNode and gridNode, which both have parent a SceneNode and not a WorldNode
    {
        auto parentNode = nodeSyncerPtr->node()->getParent();
        nodeSync.parentNodeId = parentNode->getNodeSyncId();///m_nodeSyncers.findId((WorldNode*)nodePtr->getParent());
    }

    nodeSync.node = nodeSyncerPtr;*/
}

/**void GameWorld_Sync::createWorldSyncMsg_Sprite(WorldSprite *spriteEntity, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
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

    auto parentNode = spriteEntity->getParentNode();
    if(parentNode != nullptr)
        nodeId = parentNode->getNodeId();///m_nodeSyncers.findId(parentNode);

    spriteEntitySync.spriteEntity = spriteEntity;
    spriteEntitySync.spriteSheetId = spriteSheetId;
    ///NEED TO REIMPLEMENT SPRITE ID INSIDE OF SPRITESHEED IF WE WANT TO DO THAT
    spriteEntitySync.spriteId = spriteModel->getSpriteId();
    spriteEntitySync.nodeId = nodeId;
}

void GameWorld_Sync::createWorldSyncMsg_Mesh(WorldMesh *meshEntity, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(meshEntity->getLastUpdateTime(), lastSyncTime))
        return;

    worldSyncMsg->meshEntities.push_back({meshEntity->getSyncId(),MeshEntitySync()});
    auto &meshEntitySync = worldSyncMsg->meshEntities.back().second;

    auto meshModel = meshEntity->getMeshModel();
    if(meshModel == nullptr)
        return;

    size_t meshModelId = m_syncMeshModels.findId(meshModel);

    size_t nodeId = 0;

    auto parentNode = meshEntity->getParentNode();
    if(parentNode != nullptr)
        nodeId = parentNode->getNodeId();///m_nodeSyncers.findId(parentNode);

    meshEntitySync.meshEntity = meshEntity;
    meshEntitySync.meshModelId = meshModelId;
    meshEntitySync.nodeId = nodeId;
}**/

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
    nodeId = character->getNodeId();

    characterSync.character = character;
    characterSync.characterModelId = characterModelId;
    characterSync.nodeId = nodeId;
}

/**void GameWorld_Sync::createWorldSyncMsg_Prefab(PrefabInstance *prefab, std::shared_ptr<NetMessage_WorldSync> worldSyncMsg,
                            int player_id, uint32_t lastSyncTime)
{
    if(uint32leq(prefab->getLastPrefabUpdateTime(), lastSyncTime))
        return;

    worldSyncMsg->prefabs.push_back({prefab->getPrefabSyncId(),PrefabSync()});
    auto &prefabSync = worldSyncMsg->prefabs.back().second;

    auto prefabModel = prefab->getPrefabModel();
    if(prefabModel == nullptr)
        return;

    size_t prefabModelId = m_syncPrefabAssets.findId(prefabModel);

    size_t nodeId = 0;
    nodeId = prefab->getNodeId();

    prefabSync.prefab = prefab;
    prefabSync.prefabModelId = prefabModelId;
    prefabSync.nodeId = nodeId;
}**/

void GameWorld_Sync::syncWorldFromMsg(std::shared_ptr<NetMessage_WorldSync> worldSyncMsg, size_t clientPlayerId, float RTT, bool useLockStepMode)
{
    if(uint32leq(worldSyncMsg->localTime, m_lastWorldSyncTime))
        return;

    //if(worldSyncMsg->localTime > m_lastSyncTime)
    uint32_t deltaRTT = (uint32_t)(RTT*GameServer::TICKRATE);
    deltaRTT += pou::NetEngine::getSyncDelay();

    bool firstSync = (m_lastWorldSyncTime == (uint32_t)(-1));

   // if(deltaRTT < 15) //To avoid too small RTT
     //   deltaRTT = 15;

    {
        uint32_t desiredLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1);
        uint32_t desiredMinLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*1.5);
        uint32_t desiredMaxLocalTime = (int64_t)worldSyncMsg->localTime + (int64_t)(deltaRTT*.75);

        if(firstSync)
            m_curLocalTime = desiredLocalTime;

        if(desiredMinLocalTime < m_curLocalTime || m_curLocalTime < desiredMaxLocalTime)
        {
            pou::Logger::write("Jump time from "+std::to_string(m_curLocalTime)+" to "+std::to_string(desiredLocalTime));

            m_curLocalTime = desiredLocalTime;
            m_deltaRTT = deltaRTT;
        }

        m_lastWorldSyncTime = worldSyncMsg->localTime;
    }

    m_syncPlayerActions.erase(m_syncPlayerActions.begin(), m_syncPlayerActions.upper_bound(worldSyncMsg->clientTime));


    /**for(auto &spriteSheetIt : worldSyncMsg->spriteSheets)
    {
        auto& [ spriteSheetId, spriteSheetPath ] = spriteSheetIt;
        auto *spriteSheetPtr = pou::SpriteSheetsHandler::loadAssetFromFile(spriteSheetPath);
        this->syncElement(spriteSheetPtr, spriteSheetId);
    }

    for(auto &meshModelIt : worldSyncMsg->meshModels)
    {
        auto& [ meshModelId, meshModelPath ] = meshModelIt;
        auto *meshModelPtr = pou::MeshesHandler::loadAssetFromFile(meshModelPath);
        this->syncElement(meshModelPtr, meshModelId);
    }**/

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

    /**for(auto &prefabModelIt : worldSyncMsg->prefabModels)
    {
        auto& [ prefabModelId, prefabModelPath ] = prefabModelIt;
        auto *prefabModelPtr = PrefabsHandler::loadAssetFromFile(prefabModelPath);
        this->syncElement(prefabModelPtr, prefabModelId);
    }

    for(auto &prefabIt : worldSyncMsg->prefabs)
    {
        auto& [ prefabId, prefabSync ] = prefabIt;
        auto prefabPtr = m_syncPrefabInstances.findElement(prefabId);
        if(prefabPtr == nullptr)
        {
            prefabPtr = std::make_shared<PrefabInstance>();
            this->syncElement(prefabPtr, prefabId);
        }

        if(prefabSync.prefabModelId != 0)
        {
            auto *prefabModel = m_syncPrefabAssets.findElement(prefabSync.prefabModelId);
            if(prefabModel != nullptr)
                prefabPtr->createFromModel(prefabModel);
        }

        if(prefabSync.nodeId != 0)
        {
            auto nodePtr = m_nodeSyncers.findElement(prefabSync.nodeId);
            if(nodePtr == nullptr)
                this->syncElement((std::shared_ptr<pou::SceneNode>)prefabPtr, prefabSync.nodeId);
        }
    }**/

                //std::cout<<"PlayerID :"<< clientPlayerId<<std::endl;
    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;

        auto player = m_syncPlayers.findElement(playerId);
        if(player == nullptr)
        {
            if(playerSync.characterId != 0)
            {
                if(playerId == (int)clientPlayerId && !useLockStepMode)
                {
                    player = std::make_shared<Player>();
                }
                else
                {
                    player = std::make_shared<Player>(false);
                    player->disableDamageDealing();
                }

                this->syncElement(player, playerId);
                this->syncElement((std::shared_ptr<Character>)player, playerSync.characterId);

                player->setTeam(1);

                GameMessage_World_NewPlayer gameMsg;
                gameMsg.player = player.get();
                pou::MessageBus::postMessage(GameMessageType_World_NewPlayer, &gameMsg);
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

            characterPtr->disableInputSync(!useLockStepMode);
        }

        characterPtr->syncFromCharacter(characterSync.character);

        if(useLockStepMode)
        {
            uint32_t delay = m_deltaRTT*1.5;
            characterPtr->setReconciliationDelay(delay,0);
            characterPtr->disableInputSync(false);
            characterPtr->disableDamageReceiving(true);
            characterPtr->disableAI();
        }
        else
        {
            characterPtr->setReconciliationDelay(0,m_deltaRTT*1);
            characterPtr->disableInputSync();
            //characterPtr->disableStateSync();
        }

        characterPtr->getNodeSyncer()->setMaxRewind(GameClient::MAX_PLAYER_REWIND);

        if(characterSync.characterModelId != 0)
        {
            auto *characterModel = m_syncCharacterModels.findElement(characterSync.characterModelId);
            if(characterModel != nullptr)
                characterPtr->createFromModel(characterModel);
        }

        if(characterSync.nodeId != 0)
        {
            auto nodePtr = m_nodeSyncers.findElement(characterSync.nodeId);
            if(nodePtr == nullptr)
                this->syncElement((std::shared_ptr<pou::SceneNode>)characterPtr, characterPtr->getNodeSyncer(), characterSync.nodeId);
        }
    }

    /*for(auto playerIt = m_syncPlayers.begin() ; playerIt != m_syncPlayers.end() ; ++playerIt)
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
    }*/

    for(auto &playerIt : worldSyncMsg->players)
    {
        auto& [ playerId, playerSync ] = playerIt;
        auto player = m_syncPlayers.findElement(playerId);

        if(!player)
            continue;

        if(playerIt.first != (int)clientPlayerId || useLockStepMode)
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
    for(auto &nodeSyncer : worldSyncMsg->nodeSyncers)
    {
        //auto& [ nodeId, nodeSync ] = node;
        auto nodeId = nodeSyncer->getNodeSyncId();
        auto nodeSyncerPtr = m_nodeSyncers.findElement(nodeId);

        if(nodeSyncerPtr == nullptr)
        {
            auto nodePtr = std::make_shared<pou::SceneNode>();
            nodeSyncerPtr = this->syncElement(nodePtr, nodeId);
        }

        nodeSyncerPtr->syncFrom(nodeSyncer);
    }

    for(auto &nodeSyncer : worldSyncMsg->nodeSyncers)
    {
        //auto& [ nodeId, nodeSync ] = node;
        auto nodeId = nodeSyncer->getNodeSyncId();
        auto parentNodeId = nodeSyncer->getParentNodeSyncId();

        if(parentNodeId != 0)
        {
            /*auto nodeSyncerPtr = m_nodeSyncers.findElement(nodeId);
            if(!nodeSyncerPtr)
                continue;*/

            auto nodePtr = m_syncNodes.findElement(nodeId);
            if(!nodePtr)
                continue;

            auto parentNodePtr = m_syncNodes.findElement(parentNodeId);
            if(parentNodePtr)
                parentNodePtr->addChildNode(nodePtr);
        }

        if(firstSync)
        {
            auto nodePtr = m_syncNodes.findElement(nodeId);
            auto nodeSyncPtr = m_nodeSyncers.findElement(nodeId);
            nodePtr->update();
            nodeSyncPtr->update();
        }
    }

    /**for(auto &spriteEntityIt : worldSyncMsg->spriteEntities)
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
            auto nodeSyncer = m_nodeSyncers.findElement(spriteEntitySync.nodeId);
            if(nodeSyncer != nullptr)
                nodeSyncer->node()->attachObject(spriteEntity);
        }
    }


    for(auto &meshEntityIt : worldSyncMsg->meshEntities)
    {
        auto& [ meshEntityId, meshEntitySync ] = meshEntityIt;

        auto meshEntity = m_syncMeshEntities.findElement(meshEntityId);

        //bool isNew = false;
        if(meshEntity == nullptr)
        {
            meshEntity = std::make_shared<WorldMesh>();
            this->syncElement(meshEntity, meshEntityId);
        }

        meshEntity->syncFrom(meshEntitySync.meshEntity);

        if(meshEntitySync.meshModelId != 0)
        {
            auto *meshModelPtr = m_syncMeshModels.findElement(meshEntitySync.meshModelId);
            meshEntity->setMeshModel(meshModelPtr);
        }

        if(meshEntitySync.nodeId != 0)
        {
            auto nodeSyncer = m_nodeSyncers.findElement(meshEntitySync.nodeId);
            if(nodeSyncer != nullptr)
                nodeSyncer->node()->attachObject(meshEntity);
        }
    }**/

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

        this->removeFromUpdatedCharacters(characterPtr.get());
        characterPtr->removeFromParent();
    }

    for(auto desyncNode : worldSyncMsg->desyncNodes)
    {
        auto nodeSyncerPtr = m_nodeSyncers.findElement(desyncNode);
        if(!nodeSyncerPtr)
            continue;

        this->desyncElement(nodeSyncerPtr.get());
        nodeSyncerPtr->node()->removeFromParent();
    }

    for(auto playerIt = m_syncPlayers.begin() ; playerIt != m_syncPlayers.end() ; ++playerIt)
    {
        if(playerIt->first != clientPlayerId || useLockStepMode)
        {
            uint32_t delay = m_deltaRTT*1.5;
            playerIt->second->setReconciliationDelay(delay,0);
            playerIt->second->disableInputSync(false);
            playerIt->second->disableDamageReceiving(true);
        }
        else
        {
            playerIt->second->disableSync();
            playerIt->second->setReconciliationDelay(0,0);
        }
    }
}


void GameWorld_Sync::createPlayerSyncMsg(std::shared_ptr<NetMessage_PlayerSync> playerSyncMsg,
                         int player_id, uint32_t lastSyncTime)
{
    playerSyncMsg->lastSyncTime   = this->getLastWorldSyncTime();
    playerSyncMsg->localTime      = this->getLocalTime();

    if(playerSyncMsg->useLockStepMode)
    {
        playerSyncMsg->lastPlayerActions.clear();

        for(auto playerActionIt = m_syncPlayerActions.begin() ;
            playerActionIt !=m_syncPlayerActions.end() ; ++playerActionIt)
            if((int)playerActionIt->second.first == player_id)
                playerSyncMsg->lastPlayerActions.push_back({playerActionIt->first, playerActionIt->second.second});

        return;
    }

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
        nodeId = player->getNodeId();///m_nodeSyncers.findId(player/*->node()*/);

        characterSync.character = player.get();
        characterSync.characterModelId = 0;
        characterSync.nodeId = nodeId;
    }

    /*NodeSync nodeSync;
    {
        nodeSync.node = player.get();
    }*/

    playerSyncMsg->nodeSyncer       = player->getNodeSyncer().get();
    playerSyncMsg->characterSync    = characterSync;
    playerSyncMsg->playerSync       = playerSync;
}


void GameWorld_Sync::syncPlayerFromMsg(std::shared_ptr<NetMessage_PlayerSync> worldSyncMsg, size_t clientPlayerId, float RTT)
{
    auto lastPlayerSyncTimeIt = m_lastPlayerSyncTime.lower_bound(clientPlayerId);
    if(lastPlayerSyncTimeIt == m_lastPlayerSyncTime.end() || lastPlayerSyncTimeIt->first != (int)clientPlayerId)
        lastPlayerSyncTimeIt = m_lastPlayerSyncTime.insert(lastPlayerSyncTimeIt, {clientPlayerId,-1});
    auto &lastPlayerSyncTime = (lastPlayerSyncTimeIt->second);

    if(worldSyncMsg->useLockStepMode)
    {
        if(worldSyncMsg->lastPlayerActions.empty())
            return;

        auto minClientTime = worldSyncMsg->lastPlayerActions.begin()->first;

        /**for(auto playerActionIt = worldSyncMsg->lastPlayerActions.begin() ;
            playerActionIt != worldSyncMsg->lastPlayerActions.end() && playerActionIt->second.actionType == PlayerActionType_CursorMove ;
            ++playerActionIt)**/

        for(auto &playerActionIt : worldSyncMsg->lastPlayerActions)
        {
            if(uint32less(lastPlayerSyncTime, playerActionIt.first))
            {
                auto retimedActionTime = playerActionIt.first - minClientTime + m_curLocalTime;
                this->addPlayerAction(clientPlayerId, playerActionIt.second, retimedActionTime);
            }
        }
        return;
    }

    lastPlayerSyncTime = worldSyncMsg->localTime;

    auto player = m_syncPlayers.findElement(clientPlayerId);

    ///player/*->node()*/->syncFrom(worldSyncMsg->nodeSync.node);
    player->getNodeSyncer()->syncFrom(worldSyncMsg->nodeSyncer);
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

void GameWorld_Sync::addPlayerAction(uint32_t player_id, PlayerAction &playerAction, uint32_t actionTime)
{
    m_playerActions.insert({actionTime,{player_id, playerAction}});
}

void GameWorld_Sync::syncPlayerAction(uint32_t player_id, PlayerAction &playerAction)
{
    m_syncPlayerActions.insert({m_curLocalTime,{player_id, playerAction}});
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


/*size_t GameWorld_Sync::syncElement(std::shared_ptr<WorldNode> node, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_nodeSyncers.allocateId(node);
    else
        m_nodeSyncers.insert(forceId, node);
    node->setNodeSyncId(forceId);
    return forceId;
}*/


std::shared_ptr<NodeSyncer> GameWorld_Sync::syncElement(std::shared_ptr<pou::SceneNode> node, uint32_t forceId)
{
    /*if(forceId == 0)
        forceId = m_syncNodes.allocateId(node);
    else
        m_syncNodes.insert(forceId, node);

    node->setNodeId(forceId);

    auto nodeSyncer = std::make_shared<NodeSyncer>(node.get());
    nodeSyncer->setNodeSyncId(forceId);
    m_nodeSyncers.insert(forceId, nodeSyncer);*/


    auto nodeSyncer = std::make_shared<NodeSyncer>(node.get());
    this->syncElement(node, nodeSyncer, forceId);

    return nodeSyncer;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<pou::SceneNode> node, std::shared_ptr<NodeSyncer> nodeSyncer, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncNodes.allocateId(node);
    else
        m_syncNodes.insert(forceId, node);

    node->setNodeId(forceId);
    nodeSyncer->setNodeSyncId(forceId);

    m_nodeSyncers.insert(forceId, nodeSyncer);

    return forceId;
}

/**size_t GameWorld_Sync::syncElement(pou::SpriteSheetAsset *spriteSheet, uint32_t forceId)
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

size_t GameWorld_Sync::syncElement(pou::MeshAsset *meshModel, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncMeshModels.allocateId(meshModel);
    else
        m_syncMeshModels.insert(forceId, meshModel);
    m_syncTimeMeshModels.insert({m_curLocalTime, forceId});
    return forceId;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<WorldMesh> meshEntity, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncMeshEntities.allocateId(meshEntity);
    else
        m_syncMeshEntities.insert(forceId, meshEntity);
    meshEntity->setSyncId(forceId);
    return forceId;
}**/

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
        //this->syncElement(std::shared_ptr<pou::SceneNode> character, character->getNodeSyncer());

        /**auto nodeId = m_syncNodes.allocateId(character);
        character->setNodeId(nodeId);
        character->getNodeSyncer()->setNodeSyncId(nodeId);
        m_nodeSyncers.insert(nodeId, character->getNodeSyncer());**/

        this->syncElement((std::shared_ptr<pou::SceneNode>)character, character->getNodeSyncer());

        ///this->syncElement((std::shared_ptr<WorldNode>)character);
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

/**size_t GameWorld_Sync::syncElement(PrefabAsset *prefabModel, uint32_t forceId)
{
    if(forceId == 0)
        forceId = m_syncPrefabAssets.allocateId(prefabModel);
    else
        m_syncPrefabAssets.insert(forceId, prefabModel);
    m_syncTimePrefabModels.insert({m_curLocalTime, forceId});
    return forceId;
}

size_t GameWorld_Sync::syncElement(std::shared_ptr<PrefabInstance> prefab, uint32_t forceId)
{
    if(forceId == 0)
    {
        forceId = m_syncPrefabInstances.allocateId(prefab);
        this->syncElement((std::shared_ptr<pou::SceneNode>)prefab);
    }
    else
        m_syncPrefabInstances.insert(forceId, prefab);
    prefab->setSyncData(this, forceId);
    return forceId;
}**/


/*void GameWorld_Sync::desyncElement(WorldNode *node, bool noDesyncInsert)
{
    if(!noDesyncInsert)
    {
        auto id = m_nodeSyncers.findId(node);
        m_desyncNodes.insert({m_curLocalTime,id});
    }
    m_nodeSyncers.freeElement(node);
}*/

void GameWorld_Sync::desyncElement(NodeSyncer* nodeSyncer, bool noDesyncInsert)
{
    auto id = m_nodeSyncers.findId(nodeSyncer);
    if(!noDesyncInsert)
        m_desyncNodes.insert({m_curLocalTime,id});
    m_nodeSyncers.freeElement(nodeSyncer);
    m_syncNodes.freeId(id);
}

void GameWorld_Sync::desyncElement(Character *character, bool noDesyncInsert)
{
    ///this->desyncElement((WorldNode*)character, true);
    this->desyncElement(character->getNodeSyncer().get(), true);

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

uint32_t GameWorld_Sync::getLastWorldSyncTime()
{
    return m_lastWorldSyncTime;
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
    ///I should find way to verify these nodes coincides are in the concerned world.

    if(notificationType == GameMessageType_World_NodeSyncerUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_NodeSyncerUpdated*>(data);
        m_updatedNodeSyncers.push_back(gameMsg->nodeSyncer);
    }

    /**if(notificationType == GameMessageType_World_SpriteUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_SpriteUpdated*>(data);
        m_updatedSprites.push_back(gameMsg->sprite);
    }

    if(notificationType == GameMessageType_World_MeshUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_MeshUpdated*>(data);
        m_updatedMeshes.push_back(gameMsg->mesh);
    }**/

    if(notificationType == GameMessageType_World_CharacterUpdated)
    {
        auto *gameMsg = static_cast<GameMessage_World_CharacterUpdated*>(data);
        m_updatedCharacters[m_updatedCharactersBuffer].push_back(gameMsg->character);
    }
}

void GameWorld_Sync::removeFromUpdatedCharacters(Character *character)
{
    for(auto i = 0 ; i < 1 ; ++i)
        for(auto it = m_updatedCharacters[i].begin() ; it != m_updatedCharacters[i].end() ; )
        {
            if(*it == character)
                it = m_updatedCharacters[i].erase(it);
            else
                ++it;
        }
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

void GameWorld_Sync::processPlayerActions()
{
    for(auto playerActionIt = m_playerActions.begin() ;
        playerActionIt != m_playerActions.end() && uint32leq(playerActionIt->first,m_curLocalTime) ;
        )
    {
        auto player_id = playerActionIt->second.first;

        auto player = this->getPlayer(player_id);
        if(player == nullptr)
            continue;

        auto& playerAction = playerActionIt->second.second;
        player->processAction(playerAction);

        playerActionIt = m_playerActions.erase(playerActionIt);
    }
}

