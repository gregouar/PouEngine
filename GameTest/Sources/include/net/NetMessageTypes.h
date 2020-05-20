#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

enum NetMessageType
{
    NetMessageType_Default,
    //NetMessageType_Slice,
    NetMessageType_Test,
    NBR_RELIABLEMESSAGETYPES,
};

struct NetMessage_test : pou::NetMessage
{
    int test_value;

    //int dummy;

    virtual void serializeImpl(pou::Stream *stream)
    {
       // for(int i = 0 ; i < 800000 ; ++i)
           // stream->serializeBits(dummy, 8);

        stream->serializeBits(test_value, 16);

        //for(int i = 0 ; i < 800000 ; ++i)
          //  stream->serializeBits(dummy, 8);
    }

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_test>();
    }
};


#endif // NETMESSAGETYPES_H

