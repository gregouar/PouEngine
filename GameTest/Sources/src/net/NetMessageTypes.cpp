#include "net/NetMessageTypes.h"

#include "PouEngine/net/NetEngine.h"

#include "world/GameWorld.h"

void initializeNetMessages()
{
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_Test> (NetMessageType_Test)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldInitialization> (NetMessageType_WorldInitialization)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_AskForWorldSync> (NetMessageType_AskForWorldSync)));
}

///
/// WorldInitialization
///

void NetMessage_WorldInitialization::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);

    //std::cout<<"Nbr Nodes:"<<nbr_nodes<<std::endl;

    stream->serializeBits(nbr_nodes, GameWorld::NODEID_BITS);
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
}

void NetMessage_WorldInitialization::serializeNode(pou::Stream *stream, std::pair<int, NodeSync> &node)
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

    if(stream->isReading())
    {
        nodePtr->setPosition(pos);
        nodePtr->setRotation(rot);
        nodePtr->setScale(scale);
    }
}

void NetMessage_WorldInitialization::serializeSpriteSheet(pou::Stream *stream, std::pair<int, std::string > &spriteSheet)
{
    auto& [ spriteSheetId, spriteSheetPath ] = spriteSheet;
    stream->serializeBits(spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeString(spriteSheetPath);
}

void NetMessage_WorldInitialization::serializeSpriteEntity(pou::Stream *stream, std::pair<int, SpriteEntitySync> &spriteEntity)
{
    auto& [ spriteEntityId, spriteEntitySync ] = spriteEntity;
    stream->serializeBits(spriteEntityId, GameWorld::SPRITEENTITYID_BITS);
    stream->serializeBits(spriteEntitySync.spriteSheetId, GameWorld::SPRITESHEETID_BITS);
    stream->serializeBits(spriteEntitySync.spriteId, 8);
    stream->serializeBits(spriteEntitySync.nodeId, GameWorld::NODEID_BITS);
}

///
/// AskForWorldSync
///

void NetMessage_AskForWorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
}
