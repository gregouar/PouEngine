#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

enum NetMessageType
{
    NetMessageType_Default,
    NetMessageType_Test,
    NBR_RELIABLEMESSAGETYPES,
};

struct NetMessage_test : pou::NetMessage
{
    int test_value;

    virtual void serializeImpl(pou::Stream *stream)
    {
        stream->serializeBits(test_value, 16);
    }

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_test>();
    }
};


#endif // NETMESSAGETYPES_H

