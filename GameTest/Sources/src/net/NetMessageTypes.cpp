#include "net/NetMessageTypes.h"

#include "PouEngine/net/NetEngine.h"

void initializeNetMessages()
{
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_Test> (NetMessageType_Test)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_WorldInitialization> (NetMessageType_WorldInitialization)));
    pou::NetEngine::addNetMessageModel(std::move(
        std::make_unique<NetMessage_AskForWorldSync> (NetMessageType_AskForWorldSync)));
}


void NetMessage_WorldInitialization::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
}

void NetMessage_AskForWorldSync::serializeImpl(pou::Stream *stream)
{
    stream->serializeBits(world_id, 8);
}
