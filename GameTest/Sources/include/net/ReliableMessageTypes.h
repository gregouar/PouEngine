#ifndef RELIABLEMESSAGETYPES_H
#define RELIABLEMESSAGETYPES_H

#include "PouEngine/net/ReliableMessagesFactory.h"

enum ReliableMessageType
{
    ReliableMessageType_Default,
    ReliableMessageType_Test,
    NBR_RELIABLEMESSAGETYPES,
};

struct ReliableMessage_test : pou::ReliableMessage
{
    int test_value;

    virtual void serializeImpl(pou::Stream *stream)
    {
        stream->serializeBits(test_value, 16);
    }

    virtual std::shared_ptr<pou::ReliableMessage> msgAllocator()
    {
        return std::make_shared<ReliableMessage_test>();
    }
};


#endif // RELIABLEMESSAGETYPES_H

