#include "net/NetMessageTypes.h"

#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"
#include "character/Character.h"
#include "character/Player.h"
#include "sync/NodeSyncer.h"

void initializeNetMessages()
{
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_Test> (NetMessageType_Test)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldInit> (NetMessageType_WorldInit)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldSync> (NetMessageType_WorldSync)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_PlayerSync> (NetMessageType_PlayerSync)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_PlayerEvent> (NetMessageType_PlayerEvent)));
    //pou::NetEngine::addNetMessageModel(std::move(
      //  std::make_unique<NetMessage_AskForWorldSync> (NetMessageType_AskForWorldSync)));
    //pou::NetEngine::addNetMessageModel(std::move(
      //  std::make_unique<NetMessage_PlayerAction> (NetMessageType_PlayerAction)));
}

///
/// WorldInitialization
///

void NetMessage_WorldInit::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
    stream->serializeInt(player_id, 0, GameWorld::MAX_NBR_PLAYERS);
    stream->serializeInt(dayTime, 0, 360);

    stream->serializeString(worldGeneratorModel);
    stream->serializeBits(worldGeneratorSeed, 32);
    ///stream->serializeBits(worldGrid_nodeId, GameWorld::NODEID_BITS);

    NetMessage_WorldSync::serializeImpl(stream);
}

///
/// WorldSync
///

NetMessage_WorldSync::NetMessage_WorldSync() : NetMessage_WorldSync(-1)
{
}

NetMessage_WorldSync::NetMessage_WorldSync(int t) : NetMessage(t)
    //nbr_nodes(0),
    //nbr_spriteSheets(0),
    //nbr_spriteEntities(0),
    //nbr_characterModels(0),
    //nbr_characters(0),
    //nbr_players(0)
{
}

void NetMessage_WorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeUint32(localTime); //This is the server time
    stream->serializeUint32(clientTime); //This is the last known client  time

    int nbr_nodes = nodeSyncers.size();
    stream->serializeBits(nbr_nodes, GameWorld_Sync::NODEID_BITS);
    nodeSyncers.resize(nbr_nodes);
    for(int i = 0 ; i < nbr_nodes ; ++i)
        this->serializeNode(stream, nodeSyncers[i]);
    //std::cout<<"Nbr nodes:"<<nbr_nodes<<std::endl;

    /**int nbr_spriteSheets = spriteSheets.size();
    stream->serializeBits(nbr_spriteSheets, GameWorld_Sync::SPRITESHEETID_BITS);
    spriteSheets.resize(nbr_spriteSheets);
    for(int i = 0 ; i < nbr_spriteSheets ; ++i)
        this->serializeSpriteSheet(stream, spriteSheets[i]);
    //std::cout<<"Nbr SS:"<<nbr_spriteSheets<<std::endl;

    int nbr_spriteEntities = spriteEntities.size();
    stream->serializeBits(nbr_spriteEntities, GameWorld_Sync::SPRITEENTITYID_BITS);
    spriteEntities.resize(nbr_spriteEntities);
    for(int i = 0 ; i < nbr_spriteEntities ; ++i)
        this->serializeSpriteEntity(stream, spriteEntities[i]);
    //std::cout<<"Nbr SE:"<<nbr_spriteEntities<<std::endl;

    int nbr_meshModels = meshModels.size();
    stream->serializeBits(nbr_meshModels, GameWorld_Sync::MESHMODELID_BITS);
    meshModels.resize(nbr_meshModels);
    for(int i = 0 ; i < nbr_meshModels ; ++i)
        this->serializeMeshModel(stream, meshModels[i]);

    int nbr_meshEntities = meshEntities.size();
    stream->serializeBits(nbr_meshEntities, GameWorld_Sync::MESHENTITYID_BITS);
    meshEntities.resize(nbr_meshEntities);
    for(int i = 0 ; i < nbr_meshEntities ; ++i)
        this->serializeMeshEntity(stream, meshEntities[i]);**/

    int nbr_characterModels = characterModels.size();
    stream->serializeBits(nbr_characterModels, GameWorld_Sync::CHARACTERMODELSID_BITS);
    characterModels.resize(nbr_characterModels);
    for(int i = 0 ; i < nbr_characterModels ; ++i)
        this->serializeCharacterModel(stream, characterModels[i]);
    //$std::cout<<"Nbr CM:"<<nbr_characterModels<<std::endl;

    int nbr_characters = characters.size();
    stream->serializeBits(nbr_characters, GameWorld_Sync::CHARACTERSID_BITS);
    characters.resize(nbr_characters);
    for(int i = 0 ; i < nbr_characters ; ++i)
        this->serializeCharacter(stream, characters[i]);
    //std::cout<<"Nbr C:"<<nbr_characters<<std::endl;

    int nbr_itemModels = itemModels.size();
    stream->serializeBits(nbr_itemModels, GameWorld_Sync::ITEMMODELSID_BITS);
    itemModels.resize(nbr_itemModels);
    for(int i = 0 ; i < nbr_itemModels ; ++i)
        this->serializeItemModel(stream, itemModels[i]);
    //std::cout<<"Nbr IM:"<<nbr_itemModels<<std::endl;

    int nbr_players = players.size();
    stream->serializeInt(nbr_players, 0, GameWorld::MAX_NBR_PLAYERS);
    players.resize(nbr_players);
    for(int i = 0 ; i < nbr_players ; ++i)
        this->serializePlayer(stream, players[i]);
    //std::cout<<"Nbr P:"<<nbr_players<<std::endl;

    /**int nbr_prefabModels = prefabModels.size();
    stream->serializeBits(nbr_prefabModels, GameWorld_Sync::PREFABASSETID_BITS);
    prefabModels.resize(nbr_prefabModels);
    for(int i = 0 ; i < nbr_prefabModels ; ++i)
        this->serializePrefabAsset(stream, prefabModels[i]);
    //std::cout<<"Nbr SS:"<<nbr_prefabModels<<std::endl;

    int nbr_prefabs = prefabs.size();
    stream->serializeBits(nbr_prefabs, GameWorld_Sync::PREFABINSTANCEID_BITS);
    prefabs.resize(nbr_prefabs);
    for(int i = 0 ; i < nbr_prefabs ; ++i)
        this->serializePrefabInstance(stream, prefabs[i]);
    //std::cout<<"Nbr SE:"<<nbr_prefabs<<std::endl;**/

    int nbr_desyncNodes = desyncNodes.size();
    stream->serializeBits(nbr_desyncNodes, GameWorld_Sync::NODEID_BITS);
    desyncNodes.resize(nbr_desyncNodes);
    for(int i = 0 ; i < nbr_desyncNodes ; ++i)
        stream->serializeBits(desyncNodes[i], GameWorld_Sync::NODEID_BITS);

    int nbr_desyncCharacters = desyncCharacters.size();
    stream->serializeBits(nbr_desyncCharacters, GameWorld_Sync::CHARACTERSID_BITS);
    desyncCharacters.resize(nbr_desyncCharacters);
    for(int i = 0 ; i < nbr_desyncCharacters ; ++i)
        stream->serializeBits(desyncCharacters[i], GameWorld_Sync::CHARACTERSID_BITS);

    int nbr_desyncPlayers = desyncPlayers.size();
    stream->serializeInt(nbr_desyncPlayers, 0, GameWorld::MAX_NBR_PLAYERS);
    desyncPlayers.resize(nbr_desyncPlayers);
    for(int i = 0 ; i < nbr_desyncPlayers ; ++i)
        stream->serializeInt(desyncPlayers[i], 0, GameWorld::MAX_NBR_PLAYERS);

}

void NetMessage_WorldSync::serializeNode(pou::Stream *stream, NodeSyncer *(&nodeSyncerPtr))
{
    if(stream->isReading())
    {
        auto nodeSyncer =  std::make_shared<NodeSyncer>();
        nodesBuffer.push_back(nodeSyncer);
        nodeSyncerPtr = nodeSyncer.get();
        nodeSyncerPtr->update(pou::Time(0),localTime);
    }

    /*auto nodeId = (int)nodeSyncerPtr->getNodeSyncId();
    stream->serializeBits(nodeId, GameWorld_Sync::NODEID_BITS);

    if(stream->isReading())
    {
        auto nodeSyncer =  std::make_shared<NodeSyncer>();
        nodesBuffer.push_back(nodeSyncer);
        nodeSyncerPtr = nodeSyncer.get();
        nodeSyncerPtr->update(pou::Time(0),localTime);
    }

    bool newParent = false;
    if(!stream->isReading() && uint32less(lastSyncTime,nodeSyncerPtr->getLastParentUpdateTime()))
        newParent = true;
    stream->serializeBool(newParent);
    if(newParent)
    {
        auto parentNodeId = (int)nodeSyncerPtr->getParentNodeSyncId();
        stream->serializeBits(nodeSyncerPtr->getParentNodeSyncId(), GameWorld_Sync::NODEID_BITS);
        if(stream->isReading())
            nodeSyncerPtr->setParentNodeSyncId(parentNodeId);
    }
    else
        nodeSync.setParentNodeSyncId(0);*/

    nodeSyncerPtr->serialize(stream, lastSyncTime);
}

/**void NetMessage_WorldSync::serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet)
{
    auto& [ spriteSheetId, spriteSheetPath ] = spriteSheet;
    stream->serializeBits(spriteSheetId, GameWorld_Sync::SPRITESHEETID_BITS);
    stream->serializeString(spriteSheetPath);
}

void NetMessage_WorldSync::serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity)
{
    auto& [ spriteEntityId, spriteEntitySync ] = spriteEntity;

    auto &spriteEntityPtr = spriteEntitySync.spriteEntity;
    if(stream->isReading())
    {
        auto sprite =  std::make_shared<WorldSprite>();
        spritesBuffer.push_back(sprite);
        spriteEntityPtr = sprite.get();
        spriteEntityPtr->update(pou::Time(0),localTime);
        //spriteEntityPtr->setLocalTime(localTime);
    }

    stream->serializeBits(spriteEntityId, GameWorld_Sync::SPRITEENTITYID_BITS);

    bool newSpriteModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,spriteEntityPtr->getLastModelUpdateTime()))
        newSpriteModel = true;
    stream->serializeBool(newSpriteModel);
    if(newSpriteModel)
    {
        stream->serializeBits(spriteEntitySync.spriteSheetId, GameWorld_Sync::SPRITESHEETID_BITS);
        stream->serializeBits(spriteEntitySync.spriteId, 8);
    }
    else
    {
        spriteEntitySync.spriteSheetId = 0;
        spriteEntitySync.spriteId = 0;
    }

    bool newNode = false;
    if(!stream->isReading() && uint32less(lastSyncTime,spriteEntityPtr->getLastNodeUpdateTime()))
        newNode = true;
    stream->serializeBool(newNode);
    if(newNode)
        stream->serializeBits(spriteEntitySync.nodeId, GameWorld_Sync::NODEID_BITS);
    else
        spriteEntitySync.nodeId = 0;
}

void NetMessage_WorldSync::serializeMeshModel(pou::Stream *stream, std::pair<int, std::string > &meshModel)
{
    auto& [ meshModelId, meshModelpath ] = meshModel;
    stream->serializeBits(meshModelId, GameWorld_Sync::MESHMODELID_BITS);
    stream->serializeString(meshModelpath);
}

void NetMessage_WorldSync::serializeMeshEntity(pou::Stream *stream, std::pair<int, MeshEntitySync> &meshEntity)
{
    auto& [ meshEntityId, meshEntitySync ] = meshEntity;

    auto &meshEntityPtr = meshEntitySync.meshEntity;
    if(stream->isReading())
    {
        auto mesh =  std::make_shared<WorldMesh>();
        meshesBuffer.push_back(mesh);
        meshEntityPtr = mesh.get();
        meshEntityPtr->update(pou::Time(0),localTime);
        //spriteEntityPtr->setLocalTime(localTime);
    }

    stream->serializeBits(meshEntityId, GameWorld_Sync::MESHENTITYID_BITS);

    bool newMeshModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,meshEntityPtr->getLastModelUpdateTime()))
        newMeshModel = true;
    stream->serializeBool(newMeshModel);
    if(newMeshModel)
        stream->serializeBits(meshEntitySync.meshModelId, GameWorld_Sync::MESHMODELID_BITS);
    else
        meshEntitySync.meshModelId = 0;

    bool newNode = false;
    if(!stream->isReading() && uint32less(lastSyncTime,meshEntityPtr->getLastNodeUpdateTime()))
        newNode = true;
    stream->serializeBool(newNode);
    if(newNode)
        stream->serializeBits(meshEntitySync.nodeId, GameWorld_Sync::NODEID_BITS);
    else
        meshEntitySync.nodeId = 0;
}**/

void NetMessage_WorldSync::serializeCharacterModel(pou::Stream *stream, std::pair<int, std::string > &characterModel)
{
    auto& [ characterModelId, characterModelPath ] = characterModel;
    stream->serializeBits(characterModelId, GameWorld_Sync::CHARACTERMODELSID_BITS);
    stream->serializeString(characterModelPath);
}

void NetMessage_WorldSync::serializeCharacter(pou::Stream *stream, std::pair<int, CharacterSync> &character)
{
    auto& [ characterId, characterSync ] = character;

    auto &characterPtr = characterSync.character;
    if(stream->isReading())
    {
        auto character =  std::make_shared<Character>();
        charactersBuffer.push_back(character);
        characterPtr = character.get();
        characterPtr->update(pou::Time(0),localTime);
    }

    stream->serializeBits(characterId, GameWorld_Sync::CHARACTERSID_BITS);

    bool newModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,characterPtr->getLastModelUpdateTime()))
        newModel = true;
    stream->serializeBool(newModel);
    if(newModel)
        stream->serializeBits(characterSync.characterModelId, GameWorld_Sync::CHARACTERMODELSID_BITS);
    else
        characterSync.characterModelId = 0;

    stream->serializeBits(characterSync.nodeId, GameWorld_Sync::NODEID_BITS);

    characterPtr->serializeCharacter(stream,lastSyncTime);
    ///characterPtr->getCharacterSyncComponent()->serialize(stream,lastSyncTime);

}

void NetMessage_WorldSync::serializeItemModel(pou::Stream *stream, std::pair<int, std::string > &itemModel)
{
    auto& [ itemModelId, itemModelPath ] = itemModel;
    stream->serializeBits(itemModelId, GameWorld_Sync::ITEMMODELSID_BITS);
    stream->serializeString(itemModelPath);
}

void NetMessage_WorldSync::serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player)
{
    auto& [ playerId, playerSync ] = player;
    stream->serializeInt(playerId, 1,  GameWorld::MAX_NBR_PLAYERS);
    stream->serializeBits(playerSync.characterId, GameWorld_Sync::CHARACTERSID_BITS);

    auto &playerPtr = playerSync.player;

    if(stream->isReading())
    {
        playerPtr = std::make_shared<Player>();
        playerPtr->update(pou::Time(0),localTime);
    }


    bool newGear = false;
    if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastGearUpdateTime()))
        newGear = true;
    stream->serializeBool(newGear);
    if(newGear)
    {
        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
            stream->serializeBits(playerSync.gearModelsId[i], GameWorld_Sync::ITEMMODELSID_BITS);
    }
    else
    {
        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
            playerSync.gearModelsId[i] = 0;
    }

    bool newInventory = false;
    if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastInventoryUpdateTime()))
        newInventory = true;
    stream->serializeBool(newInventory);
    if(newInventory)
    {
        int nbrItems = playerSync.inventoryItemModelsId.size();
        stream->serializeBits(nbrItems, 8);
        playerSync.inventoryItemModelsId.resize(nbrItems);
        for(auto i = 0 ; i  < nbrItems ; ++i)
            stream->serializeBits(playerSync.inventoryItemModelsId[i], GameWorld_Sync::ITEMMODELSID_BITS);
    }

    playerPtr->serializePlayer(stream,lastSyncTime);
}


/**void NetMessage_WorldSync::serializePrefabAsset(pou::Stream *stream, std::pair<int, std::string > &prefabModel)
{
    auto& [ prefabModelId, prefabPath ] = prefabModel;
    stream->serializeBits(prefabModelId, GameWorld_Sync::PREFABASSETID_BITS);
    stream->serializeString(prefabPath);
}

void NetMessage_WorldSync::serializePrefabInstance(pou::Stream *stream, std::pair<int, PrefabSync> &prefab)
{
    auto& [ prefabId, prefabSync ] = prefab;

    auto &prefabPtr = prefabSync.prefab;
    if(stream->isReading())
    {
        auto prefabBuffer =  std::make_shared<PrefabInstance>();
        prefabsBuffer.push_back(prefabBuffer);
        prefabPtr = prefabBuffer.get();
        prefabPtr->update(pou::Time(0),localTime);
    }

    stream->serializeBits(prefabId, GameWorld_Sync::PREFABINSTANCEID_BITS);

    bool newPrefabModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,prefabPtr->getLastPrefabModelUpdateTime()))
        newPrefabModel = true;
    stream->serializeBool(newPrefabModel);
    if(newPrefabModel)
        stream->serializeBits(prefabSync.prefabModelId, GameWorld_Sync::PREFABASSETID_BITS);
    else
        prefabSync.prefabModelId = 0;

    stream->serializeBits(prefabSync.nodeId, GameWorld_Sync::NODEID_BITS);
}**/



///
/// PlayerAction
///

void serializePlayerAction(pou::Stream *stream, PlayerAction &playerAction)
{
    int actionType = (int)playerAction.actionType;
    stream->serializeInt(actionType, 0, NBR_PLAYERACTIONTYPES);
    if(stream->isReading())
        playerAction.actionType = (PlayerActionType)actionType;

    if(playerAction.actionType == PlayerActionType_Walk
    || playerAction.actionType == PlayerActionType_Look
    || playerAction.actionType == PlayerActionType_Attack
    || playerAction.actionType == PlayerActionType_Dash)
    {
        stream->serializeFloat(playerAction.direction.x,-1,1,2);
        stream->serializeFloat(playerAction.direction.y,-1,1,2);
    }

    if(playerAction.actionType == PlayerActionType_CursorMove)
    {
        stream->serializeFloat(playerAction.direction.x);
        stream->serializeFloat(playerAction.direction.y);
    }

    if(playerAction.actionType == PlayerActionType_UseItem)
    {
        stream->serializeBits(playerAction.value,16);
    }
}

/*void NetMessage_PlayerAction::serializeImpl(pou::Stream *stream)
{
    stream->serializeUint32(clientTime);
    serializePlayerAction(stream, playerAction);
}*/

///
/// PlayerSync
///


void NetMessage_PlayerSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeUint32(lastSyncTime); //This is the last server sync time
    stream->serializeUint32(localTime); //This is the client local time
    stream->serializeBool(useLockStepMode);

    if(lastSyncTime == (uint32_t)-1)
    {
        if(stream->isReading())
            playerSave = std::make_shared<PlayerSave>();

        playerSave->serialize(stream);

        return;
    }

    if(useLockStepMode)
    {
        int nbrActions = std::min(lastPlayerActions.size(), size_t(255));
        stream->serializeBits(nbrActions, 8);
        lastPlayerActions.resize(nbrActions);
        for(auto i = 0 ; i < nbrActions ; ++i)
        {
            auto& [actionTime, playerAction] = lastPlayerActions[i];
            stream->serializeUint32(actionTime);
            serializePlayerAction(stream, playerAction);
        }
        return;
    }

    {
        auto &nodeSyncerPtr = nodeSyncer;
        if(stream->isReading())
        {
            nodeBuffer = std::make_shared<NodeSyncer>();
            nodeSyncerPtr = nodeBuffer.get();
            nodeSyncerPtr->update(pou::Time(0),localTime);
        }
        nodeSyncerPtr->serialize(stream, lastSyncTime);
    }

    {
        auto &characterPtr = characterSync.character;
        if(stream->isReading())
        {
            characterBuffer = std::make_shared<Character>();
            characterPtr = characterBuffer.get();
            characterPtr->update(pou::Time(0),localTime);
        }
        characterPtr->serializeCharacter(stream,lastSyncTime);
    }

    {
        auto &playerPtr = playerSync.player;

        if(stream->isReading())
        {
            playerPtr = std::make_shared<Player>();
            playerPtr->update(pou::Time(0),localTime);
        }

        bool newGear = false;
        if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastGearUpdateTime()))
            newGear = true;
        stream->serializeBool(newGear);
        if(newGear)
        {
            for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
                stream->serializeBits(playerSync.gearModelsId[i], GameWorld_Sync::ITEMMODELSID_BITS);
        }
        else
        {
            for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
                playerSync.gearModelsId[i] = 0;
        }

        bool newInventory = false;
        if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastInventoryUpdateTime()))
            newInventory = true;
        stream->serializeBool(newInventory);
        if(newInventory)
        {
            int nbrItems = playerSync.inventoryItemModelsId.size();
            stream->serializeBits(nbrItems, 8);
            playerSync.inventoryItemModelsId.resize(nbrItems);
            for(auto i = 0 ; i  < nbrItems ; ++i)
                stream->serializeBits(playerSync.inventoryItemModelsId[i], GameWorld_Sync::ITEMMODELSID_BITS);
        }

        playerPtr->serializePlayer(stream,lastSyncTime);
    }
}



///
/// PlayerEvent
///

void NetMessage_PlayerEvent::serializeImpl(pou::Stream *stream)
{
    stream->serializeInt(eventType, 0, NBR_PLAYEREVENTTYPES);
    stream->serializeUint32(localTime); //This is the client local time

    if(eventType == PlayerEventType_CharacterDamaged)
    {
        if(direction != glm::vec2(0))
            direction = glm::normalize(direction);

        stream->serializeBits(syncId,GameWorld_Sync::CHARACTERSID_BITS);
        stream->serializeFloat(direction.x, -1, 1, 2);
        stream->serializeFloat(direction.y, -1, 1, 2);
        stream->serializeFloat(amount);
    }
}

///
/// AskForWorldSync
///

/*void NetMessage_AskForWorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeUint32(lastSyncTime); //This is the last server sync time
    stream->serializeUint32(localTime); //This is the client local time

    int nbrActions = std::min(lastPlayerActions.size(), size_t(255));
    stream->serializeBits(nbrActions, 8);
    lastPlayerActions.resize(nbrActions);
    for(auto i = 0 ; i < nbrActions ; ++i)
    {
        auto& [actionTime, playerAction] = lastPlayerActions[i];
        stream->serializeUint32(actionTime);
        serializePlayerAction(stream, playerAction);
    }
}*/
