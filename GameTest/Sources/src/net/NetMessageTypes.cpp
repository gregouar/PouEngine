#include "net/NetMessageTypes.h"

#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"
#include "character/Character.h"
#include "character/PlayableCharacter.h"

void initializeNetMessages()
{
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_Test> (NetMessageType_Test)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldInit> (NetMessageType_WorldInit)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldSync> (NetMessageType_WorldSync)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_AskForWorldSync> (NetMessageType_AskForWorldSync)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_PlayerAction> (NetMessageType_PlayerAction)));
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

NetMessage_WorldSync::NetMessage_WorldSync(int t) : NetMessage(t),
    nbr_nodes(0),
    nbr_spriteSheets(0),
    nbr_spriteEntities(0),
    nbr_characterModels(0),
    nbr_characters(0),
    nbr_players(0)
{
}

void NetMessage_WorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeFloat(localTime);

    stream->serializeBits(nbr_nodes, GameWorld::NODEID_BITS);
    nodes.resize(nbr_nodes);
    for(int i = 0 ; i < nbr_nodes ; ++i)
        this->serializeNode(stream, nodes[i]);
    //std::cout<<"Nbr nodes:"<<nbr_nodes<<std::endl;

    stream->serializeBits(nbr_spriteSheets, GameWorld::SPRITESHEETID_BITS);
    spriteSheets.resize(nbr_spriteSheets);
    for(int i = 0 ; i < nbr_spriteSheets ; ++i)
        this->serializeSpriteSheet(stream, spriteSheets[i]);
    //std::cout<<"Nbr SS:"<<nbr_spriteSheets<<std::endl;

    stream->serializeBits(nbr_spriteEntities, GameWorld::SPRITEENTITYID_BITS);
    spriteEntities.resize(nbr_spriteEntities);
    for(int i = 0 ; i < nbr_spriteEntities ; ++i)
        this->serializeSpriteEntity(stream, spriteEntities[i]);
    //std::cout<<"Nbr SE:"<<nbr_spriteEntities<<std::endl;

    stream->serializeBits(nbr_characterModels, GameWorld::CHARACTERMODELSID_BITS);
    characterModels.resize(nbr_characterModels);
    for(int i = 0 ; i < nbr_characterModels ; ++i)
        this->serializeCharacterModel(stream, characterModels[i]);
    //std::cout<<"Nbr CM:"<<nbr_characterModels<<std::endl;

    stream->serializeBits(nbr_characters, GameWorld::CHARACTERSID_BITS);
    characters.resize(nbr_characters);
    for(int i = 0 ; i < nbr_characters ; ++i)
        this->serializeCharacter(stream, characters[i]);
    //std::cout<<"Nbr C:"<<nbr_characters<<std::endl;

    stream->serializeInt(nbr_players, 0, GameWorld::MAX_NBR_PLAYERS);
    players.resize(nbr_players);
    for(int i = 0 ; i < nbr_players ; ++i)
        this->serializePlayer(stream, players[i]);
    //std::cout<<"Nbr P:"<<nbr_players<<std::endl;
}

void NetMessage_WorldSync::serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node)
{
    auto& [ nodeId, nodeSync ] = node;
    stream->serializeBits(nodeId, GameWorld::NODEID_BITS);

    auto &nodePtr = nodeSync.node;
    if(stream->isReading())
    {
        nodePtr = new pou::SceneNode(0);
        nodePtr->setLocalTime(localTime);
    }

    bool newParent = false;
    if(!stream->isReading() && clientTime < nodePtr->getLastParentUpdateTime())
        newParent = true;
    stream->serializeBool(newParent);
    if(newParent)
        stream->serializeBits(nodeSync.parentNodeId, GameWorld::NODEID_BITS);
    else
        nodeSync.parentNodeId = 0;

    nodePtr->serializeNode(stream, clientTime);
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

    auto spriteEntityPtr = spriteEntitySync.spriteEntity;
    if(stream->isReading())
    {
        spriteEntityPtr = new pou::SpriteEntity();
        spriteEntityPtr->setLocalTime(localTime);
    }

    stream->serializeBits(spriteEntityId, GameWorld::SPRITEENTITYID_BITS);

    bool newSpriteModel = false;
    if(!stream->isReading() && clientTime < spriteEntityPtr->getLastModelUptateTime())
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
    if(!stream->isReading() && clientTime < spriteEntityPtr->getLastNodeUpdateTime())
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
        characterPtr->setLocalTime(localTime);
    }

    stream->serializeBits(characterId, GameWorld::CHARACTERSID_BITS);

    bool newModel = false;
    if(!stream->isReading() && clientTime < characterPtr->getLastModelUpdateTime())
        newModel = true;
    stream->serializeBool(newModel);
    if(newModel)
        stream->serializeBits(characterSync.characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    else
        characterSync.characterModelId = 0;

    stream->serializeBits(characterSync.nodeId, GameWorld::NODEID_BITS);

    characterPtr->serializeCharacter(stream,clientTime);
}

void NetMessage_WorldSync::serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player)
{
    auto& [ playerId, playerSync ] = player;
    stream->serializeInt(playerId, 1,  GameWorld::MAX_NBR_PLAYERS);
    stream->serializeBits(playerSync.characterId, GameWorld::CHARACTERSID_BITS);

    auto &playerPtr = playerSync.player;

    if(stream->isReading())
    {
        playerPtr = new PlayableCharacter();
        playerPtr->setLocalTime(localTime);
    }
    ///characterPtr->serializePlayer(stream,clientTime);
}

///
/// AskForWorldSync
///

void NetMessage_AskForWorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeFloat(clientTime);
}

///
/// PlayerAction
///


void NetMessage_PlayerAction::serializeImpl(pou::Stream *stream)
{
    stream->serializeFloat(clientTime);
    stream->serializeInt(playerActionType, 0, NBR_PLAYERACTIONTYPES);

    if(playerActionType == PlayerActionType_Walk)
    {
        stream->serializeFloat(walkDirection.x,-1,1,2);
        stream->serializeFloat(walkDirection.y,-1,1,2);
    }

}
