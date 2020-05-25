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
        std::make_unique<NetMessage_AskForWorldInit> (NetMessageType_AskForWorldInit)));
}

///
/// WorldInitialization
///

void NetMessage_WorldInit::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
    stream->serializeFloat(localTime);
    stream->serializeInt(dayTime, 0, 360);
    stream->serializeInt(player_id, 0, GameWorld::MAX_NBR_PLAYERS);

    //std::cout<<"Nbr Nodes:"<<nbr_nodes<<std::endl;

    stream->serializeBits(nbr_nodes, GameWorld::NODEID_BITS);
    std::cout<<"Nbr nodes:"<<nbr_nodes<<std::endl;
    nodes.resize(nbr_nodes);
    for(int i = 0 ; i < nbr_nodes ; ++i)
        this->serializeNode(stream, nodes[i]);

    stream->serializeBits(nbr_spriteSheets, GameWorld::SPRITESHEETID_BITS);
    spriteSheets.resize(nbr_spriteSheets);
    for(int i = 0 ; i < nbr_spriteSheets ; ++i)
        this->serializeSpriteSheet(stream, spriteSheets[i]);

    stream->serializeBits(nbr_spriteEntities, GameWorld::SPRITEENTITYID_BITS);
    spriteEntities.resize(nbr_spriteEntities);
    for(int i = 0 ; i < nbr_spriteEntities ; ++i)
        this->serializeSpriteEntity(stream, spriteEntities[i]);

    stream->serializeBits(nbr_characterModels, GameWorld::CHARACTERMODELSID_BITS);
    characterModels.resize(nbr_characterModels);
    for(int i = 0 ; i < nbr_characterModels ; ++i)
        this->serializeCharacterModel(stream, characterModels[i]);

    stream->serializeBits(nbr_characters, GameWorld::CHARACTERSID_BITS);
    characters.resize(nbr_characters);
    for(int i = 0 ; i < nbr_characters ; ++i)
        this->serializeCharacter(stream, characters[i]);

    stream->serializeInt(nbr_players, 0, GameWorld::MAX_NBR_PLAYERS);
    players.resize(nbr_players);
    for(int i = 0 ; i < nbr_players ; ++i)
        this->serializePlayer(stream, players[i]);
}

void NetMessage_WorldInit::serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node)
{
    auto& [ nodeId, nodeSync ] = node;
    stream->serializeBits(nodeId, GameWorld::NODEID_BITS);
    stream->serializeBits(nodeSync.parentNodeId, GameWorld::NODEID_BITS);

    auto &nodePtr = nodeSync.node;

    if(stream->isReading())
        nodePtr = new pou::SceneNode(0);

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
    }
}

void NetMessage_WorldInit::serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet)
{
    auto& [ spriteSheetId, spriteSheetPath ] = spriteSheet;
    stream->serializeBits(spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeString(spriteSheetPath);
}

void NetMessage_WorldInit::serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity)
{
    auto& [ spriteEntityId, spriteEntitySync ] = spriteEntity;
    stream->serializeBits(spriteEntityId, GameWorld::SPRITEENTITYID_BITS);
    stream->serializeBits(spriteEntitySync.spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeBits(spriteEntitySync.spriteId, 8);
    stream->serializeBits(spriteEntitySync.nodeId, GameWorld::NODEID_BITS);
}

void NetMessage_WorldInit::serializeCharacterModel(pou::Stream *stream, std::pair<int, std::string > &characterModel)
{
    auto& [ characterModelId, characterModelPath ] = characterModel;
    stream->serializeBits(characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    stream->serializeString(characterModelPath);
}

void NetMessage_WorldInit::serializeCharacter(pou::Stream *stream, std::pair<int, CharacterSync> &character)
{
    auto& [ characterId, characterSync ] = character;
    stream->serializeBits(characterId, GameWorld::CHARACTERSID_BITS);
    stream->serializeBits(characterSync.characterModelId, GameWorld::CHARACTERMODELSID_BITS);
    stream->serializeBits(characterSync.nodeId, GameWorld::NODEID_BITS);

    auto &characterPtr = characterSync.character;

    if(stream->isReading())
        characterPtr = new Character();

}

void NetMessage_WorldInit::serializePlayer(pou::Stream *stream, std::pair<int, PlayerSync> &player)
{
    auto& [ playerId, playerSync ] = player;
    stream->serializeInt(playerId, 1,  GameWorld::MAX_NBR_PLAYERS);
    stream->serializeBits(playerSync.characterId, GameWorld::CHARACTERSID_BITS);

    auto &playerPtr = playerSync.player;

    if(stream->isReading())
        playerPtr = new PlayableCharacter();
}

///
/// AskForWorldSync
///

void NetMessage_AskForWorldInit::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
}
