#ifndef NETMESSAGETYPES_H
#define NETMESSAGETYPES_H

#include "PouEngine/net/NetMessagesFactory.h"

enum NetMessageType
{
    NetMessageType_Default,
    //NetMessageType_Slice,
    NetMessageType_Test,
    NetMessageType_WorldInitialization,
    NetMessageType_AskForWorldSync,
    NBR_RELIABLEMESSAGETYPES,
};

void initializeNetMessages();

struct NetMessage_Test : public pou::NetMessage
{
    NetMessage_Test() : NetMessage(){}
    NetMessage_Test(int t) : NetMessage(t){}

    int test_value;

   // int dummy;

    virtual void serializeImpl(pou::Stream *stream)
    {
        //for(int i = 0 ; i < 0 ; ++i)
          //  stream->serializeBits(dummy, 8);

        stream->serializeBits(test_value, 16);

       // for(int i = 0 ; i < 0 ; ++i)
         //   stream->serializeBits(dummy, 8);
    }

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_Test>();
    }
};

struct NetMessage_WorldInitialization : public pou::NetMessage
{
    NetMessage_WorldInitialization() : NetMessage(){}
    NetMessage_WorldInitialization(int t) : NetMessage(t){}

    int world_id;

    virtual void serializeImpl(pou::Stream *stream);

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_WorldInitialization>();
    }
};


struct NetMessage_AskForWorldSync : public pou::NetMessage
{
    NetMessage_AskForWorldSync() : NetMessage(){}
    NetMessage_AskForWorldSync(int t) : NetMessage(t){}

    int world_id;

    virtual void serializeImpl(pou::Stream *stream);

    virtual std::shared_ptr<pou::NetMessage> msgAllocator()
    {
        return std::make_shared<NetMessage_AskForWorldSync>();
    }
};




#endif // NETMESSAGETYPES_H

