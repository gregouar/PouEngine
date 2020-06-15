#include "net/NetMessageTypes.h"

#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"
#include "character/Character.h"
#include "character/Player.h"

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

    int nbr_nodes = nodes.size();
    stream->serializeBits(nbr_nodes, GameWorld::NODEID_BITS);
    nodes.resize(nbr_nodes);
    for(int i = 0 ; i < nbr_nodes ; ++i)
        this->serializeNode(stream, nodes[i]);
    //std::cout<<"Nbr nodes:"<<nbr_nodes<<std::endl;

    int nbr_spriteSheets = spriteSheets.size();
    stream->serializeBits(nbr_spriteSheets, GameWorld::SPRITESHEETID_BITS);
    spriteSheets.resize(nbr_spriteSheets);
    for(int i = 0 ; i < nbr_spriteSheets ; ++i)
        this->serializeSpriteSheet(stream, spriteSheets[i]);
    //std::cout<<"Nbr SS:"<<nbr_spriteSheets<<std::endl;

    int nbr_spriteEntities = spriteEntities.size();
    stream->serializeBits(nbr_spriteEntities, GameWorld::SPRITEENTITYID_BITS);
    spriteEntities.resize(nbr_spriteEntities);
    for(int i = 0 ; i < nbr_spriteEntities ; ++i)
        this->serializeSpriteEntity(stream, spriteEntities[i]);
    //std::cout<<"Nbr SE:"<<nbr_spriteEntities<<std::endl;

    int nbr_characterModels = characterModels.size();
    stream->serializeBits(nbr_characterModels, GameWorld::CHARACTERMODELSID_BITS);
    characterModels.resize(nbr_characterModels);
    for(int i = 0 ; i < nbr_characterModels ; ++i)
        this->serializeCharacterModel(stream, characterModels[i]);
    //std::cout<<"Nbr CM:"<<nbr_characterModels<<std::endl;

    int nbr_characters = characters.size();
    stream->serializeBits(nbr_characters, GameWorld::CHARACTERSID_BITS);
    characters.resize(nbr_characters);
    for(int i = 0 ; i < nbr_characters ; ++i)
        this->serializeCharacter(stream, characters[i]);
    //std::cout<<"Nbr C:"<<nbr_characters<<std::endl;

    int nbr_itemModels = itemModels.size();
    stream->serializeBits(nbr_itemModels, GameWorld::ITEMMODELSID_BITS);
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

    int nbr_desyncNodes = desyncNodes.size();
    stream->serializeBits(nbr_desyncNodes, GameWorld::NODEID_BITS);
    desyncNodes.resize(nbr_desyncNodes);
    for(int i = 0 ; i < nbr_desyncNodes ; ++i)
        stream->serializeBits(desyncNodes[i], GameWorld::NODEID_BITS);

    int nbr_desyncCharacters = desyncCharacters.size();
    stream->serializeBits(nbr_desyncCharacters, GameWorld::CHARACTERSID_BITS);
    desyncCharacters.resize(nbr_desyncCharacters);
    for(int i = 0 ; i < nbr_desyncCharacters ; ++i)
        stream->serializeBits(desyncCharacters[i], GameWorld::CHARACTERSID_BITS);

    int nbr_desyncPlayers = desyncPlayers.size();
    stream->serializeInt(nbr_desyncPlayers, 0, GameWorld::MAX_NBR_PLAYERS);
    desyncPlayers.resize(nbr_desyncPlayers);
    for(int i = 0 ; i < nbr_desyncPlayers ; ++i)
        stream->serializeInt(desyncPlayers[i], 0, GameWorld::MAX_NBR_PLAYERS);

}

void NetMessage_WorldSync::serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node)
{
    auto& [ nodeId, nodeSync ] = node;
    stream->serializeBits(nodeId, GameWorld::NODEID_BITS);

    auto &nodePtr = nodeSync.node;
    if(stream->isReading())
    {
        nodePtr = new pou::SceneNode(0);
        nodePtr->update(pou::Time(0),localTime);
    }

    bool newParent = false;
    if(!stream->isReading() && uint32less(lastSyncTime,nodePtr->getLastParentUpdateTime()))
        newParent = true;
    stream->serializeBool(newParent);
    if(newParent)
        stream->serializeBits(nodeSync.parentNodeId, GameWorld::NODEID_BITS);
    else
        nodeSync.parentNodeId = 0;

    nodePtr->serializeNode(stream, lastSyncTime);
}

void NetMessage_WorldSync::serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet)
{
    auto& [ spriteSheetId, spriteSheetPath ] = spriteSheet;
    stream->serializeBits(spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeString(spriteSheetPath);
}

void NetMessage_WorldSync::serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity)
{
    auto& [ spriteEntityId, spriteEntitySync ] = spriteEntity;

    auto &spriteEntityPtr = spriteEntitySync.spriteEntity;
    if(stream->isReading())
    {
        spriteEntityPtr = new pou::SpriteEntity();
        spriteEntityPtr->update(pou::Time(0),localTime);
        //spriteEntityPtr->setLocalTime(localTime);
    }

    stream->serializeBits(spriteEntityId, GameWorld::SPRITEENTITYID_BITS);

    bool newSpriteModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,spriteEntityPtr->getLastModelUptateTime()))
        newSpriteModel = true;
    stream->serializeBool(newSpriteModel);
    if(newSpriteModel)
    {
        stream->serializeBits(spriteEntitySync.spriteSheetId, GameWorld::SPRITESHEETID_BITS);
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
        stream->serializeBits(spriteEntitySync.nodeId, GameWorld::NODEID_BITS);
    else
        spriteEntitySync.nodeId = 0;
}

void NetMessage_WorldSync::serializeCharacterModel(pou::Stream *stream, std::pair<int, std::string > &characterModel)
{
    auto& [ characterModelId, characterModelPath ] = characterModel;
    stream->serializeBits(characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    stream->serializeString(characterModelPath);
}

void NetMessage_WorldSync::serializeCharacter(pou::Stream *stream, std::pair<int, CharacterSync> &character)
{
    auto& [ characterId, characterSync ] = character;

    auto &characterPtr = characterSync.character;
    if(stream->isReading())
    {
        characterPtr = new Character();
        characterPtr->update(pou::Time(0),localTime);
    }

    stream->serializeBits(characterId, GameWorld::CHARACTERSID_BITS);

    bool newModel = false;
    if(!stream->isReading() && uint32less(lastSyncTime,characterPtr->getLastModelUpdateTime()))
        newModel = true;
    stream->serializeBool(newModel);
    if(newModel)
        stream->serializeBits(characterSync.characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    else
        characterSync.characterModelId = 0;

    stream->serializeBits(characterSync.nodeId, GameWorld::NODEID_BITS);

    characterPtr->serializeCharacter(stream,lastSyncTime);

}

void NetMessage_WorldSync::serializeItemModel(pou::Stream *stream, std::pair<int, std::string > &itemModel)
{
    auto& [ itemModelId, itemModelPath ] = itemModel;
    stream->serializeBits(itemModelId, GameWorld::ITEMMODELSID_BITS);
    stream->serializeString(itemModelPath);
}

void NetMessage_WorldSync::serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player)
{
    auto& [ playerId, playerSync ] = player;
    stream->serializeInt(playerId, 1,  GameWorld::MAX_NBR_PLAYERS);
    stream->serializeBits(playerSync.characterId, GameWorld::CHARACTERSID_BITS);

    auto &playerPtr = playerSync.player;

    if(stream->isReading())
    {
        playerPtr = new Player();
        playerPtr->update(pou::Time(0),localTime);
    }


    bool newGear = false;
    if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastGearUpdateTime()))
        newGear = true;
    stream->serializeBool(newGear);
    if(newGear)
    {
        for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
            stream->serializeBits(playerSync.gearModelsId[i], GameWorld::ITEMMODELSID_BITS);
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
            stream->serializeBits(playerSync.inventoryItemModelsId[i], GameWorld::ITEMMODELSID_BITS);
    }

    playerPtr->serializePlayer(stream,lastSyncTime);
}


///
/// PlayerAction
///

void serializePlayerAction(pou::Stream *stream, PlayerAction &playerAction)
{
    int actionType = (int)playerAction.actionType;
    stream->serializeInt(actionType, 0, NBR_PLAYERACTIONTYPES);
    if(stream->isReading())
        playerAction.actionType = (PlayerActionType)actionType;

    if(playerAction.actionType == PlayerActionType_CursorMove
    || playerAction.actionType == PlayerActionType_Walk
    || playerAction.actionType == PlayerActionType_Look
    || playerAction.actionType == PlayerActionType_Attack
    || playerAction.actionType == PlayerActionType_Dash)
    {
        stream->serializeFloat(playerAction.direction.x,-1,1,2);
        stream->serializeFloat(playerAction.direction.y,-1,1,2);
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

    if(lastSyncTime == (uint32_t)-1)
        return;

    {
        auto &nodePtr = nodeSync.node;
        if(stream->isReading())
        {
            nodePtr = &nodeBuffer;
            nodePtr->update(pou::Time(0),localTime);
        }
        nodePtr->serializeNode(stream, lastSyncTime);
    }

    {
        auto &characterPtr = characterSync.character;
        if(stream->isReading())
        {
            characterPtr = &characterBuffer;
            characterPtr->update(pou::Time(0),localTime);
        }
        characterPtr->serializeCharacter(stream,lastSyncTime);
    }

    {
        auto &playerPtr = playerSync.player;

        if(stream->isReading())
        {
            playerPtr = &playerBuffer;
            playerPtr->update(pou::Time(0),localTime);
        }

        bool newGear = false;
        if(!stream->isReading() && uint32less(lastSyncTime,playerPtr->getLastGearUpdateTime()))
            newGear = true;
        stream->serializeBool(newGear);
        if(newGear)
        {
            for(int i = 0 ; i < NBR_GEAR_TYPES ; ++i)
                stream->serializeBits(playerSync.gearModelsId[i], GameWorld::ITEMMODELSID_BITS);
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
                stream->serializeBits(playerSync.inventoryItemModelsId[i], GameWorld::ITEMMODELSID_BITS);
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

    if(eventType == PlayerEventType_CharacterDamaged)
    {
        if(direction != glm::vec2(0))
            direction = glm::normalize(direction);

        stream->serializeBits(syncId,GameWorld::CHARACTERSID_BITS);
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
