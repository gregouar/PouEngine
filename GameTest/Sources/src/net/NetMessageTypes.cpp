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

    bool hasParent = false;
    if(!stream->isReading())
    {
        if(clientTime < nodePtr->getLastParentUpdateTime())
            hasParent = true;
    }

    stream->serializeBool(hasParent);
    if(hasParent)
        stream->serializeBits(nodeSync.parentNodeId, GameWorld::NODEID_BITS);

    nodePtr->serialize(stream, clientTime);

    /*stream->serializeBool(hasParent);
    if(hasParent)
        stream->serializeBits(nodeSync.parentNodeId, GameWorld::NODEID_BITS);

    stream->serializeBool(hasPos);
    glm::vec3 pos = nodePtr->getPosition();
    stream->serializeFloat(pos.x, -GameWorld::GAMEWORLD_MAX_SIZE.x, GameWorld::GAMEWORLD_MAX_SIZE.x, 0);
    stream->serializeFloat(pos.y, -GameWorld::GAMEWORLD_MAX_SIZE.y, GameWorld::GAMEWORLD_MAX_SIZE.y, 0);
    stream->serializeFloat(pos.z, -GameWorld::GAMEWORLD_MAX_SIZE.z, GameWorld::GAMEWORLD_MAX_SIZE.z, 2);


    glm::vec3 rot = nodePtr->getEulerRotation();
    bool isRot = (rot != glm::vec3(0.0));
    stream->serializeBool(isRot);
    if(isRot)
    {
        stream->serializeFloat(rot.x, -glm::pi<float>(), glm::pi<float>(), 2);
        stream->serializeFloat(rot.y, -glm::pi<float>(), glm::pi<float>(), 2);
        stream->serializeFloat(rot.z, -glm::pi<float>(), glm::pi<float>(), 2);
    } else
        rot = glm::vec3(0.0);

    glm::vec3 scale = nodePtr->getScale();
    bool isScale = (scale != glm::vec3(1.0));
    stream->serializeBool(isScale);
    if(isScale)
    {
        stream->serializeFloat(scale.x, -GameWorld::NODE_MAX_SCALE, GameWorld::NODE_MAX_SCALE, GameWorld::NODE_SCALE_DECIMALS);
        stream->serializeFloat(scale.y, -GameWorld::NODE_MAX_SCALE, GameWorld::NODE_MAX_SCALE, GameWorld::NODE_SCALE_DECIMALS);
        stream->serializeFloat(scale.z, -GameWorld::NODE_MAX_SCALE, GameWorld::NODE_MAX_SCALE, GameWorld::NODE_SCALE_DECIMALS);
    } else
        scale = glm::vec3(1.0);


    glm::vec4 color = nodePtr->getColor();
    bool isColor = (color != glm::vec4(1.0));
    stream->serializeBool(isColor);
    if(isColor)
    {
        stream->serializeFloat(color.r, 0, 10, 2);
        stream->serializeFloat(color.g, 0, 10, 2);
        stream->serializeFloat(color.b, 0, 10, 2);
        stream->serializeFloat(color.a, 0, 1, 2);
    } else
        color = glm::vec4(1.0);


    if(stream->isReading())
    {
        nodePtr->setPosition(pos);
        nodePtr->setRotation(rot);
        nodePtr->setScale(scale);
        nodePtr->setColor(color);
    }*/
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
    stream->serializeBits(spriteEntityId, GameWorld::SPRITEENTITYID_BITS);
    stream->serializeBits(spriteEntitySync.spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeBits(spriteEntitySync.spriteId, 8);
    stream->serializeBits(spriteEntitySync.nodeId, GameWorld::NODEID_BITS);

    auto &spriteEntityPtr = spriteEntitySync.spriteEntity;

    if(stream->isReading())
    {
        spriteEntityPtr = new pou::SpriteEntity();
        spriteEntityPtr->setLocalTime(localTime);
    }
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
    stream->serializeBits(characterId, GameWorld::CHARACTERSID_BITS);
    stream->serializeBits(characterSync.characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    stream->serializeBits(characterSync.nodeId, GameWorld::NODEID_BITS);

    auto &characterPtr = characterSync.character;

    if(stream->isReading())
    {
        characterPtr = new Character();
        characterPtr->setLocalTime(localTime);
    }

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
