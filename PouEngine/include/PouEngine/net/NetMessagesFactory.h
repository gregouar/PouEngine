#ifndef NETMESSAGESFACTORY_H
#define NETMESSAGESFACTORY_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

#include <vector>
#include <memory>

namespace pou
{
struct NetMessage
{
    NetMessage(){}
    NetMessage(int t) : type(t){}

    int type;
    bool isReliable;
    int id;

    virtual void serializeImpl(Stream *stream);
    virtual std::shared_ptr<NetMessage> msgAllocator();

    int serializeHeader(Stream *stream/*, bool flush = true*/);
    int serialize(Stream *stream, bool computeBytesAndFlush = true);
};

/*struct NetMessage_Slice : NetMessage
{
    int chunkId;
    int nbrSlices;

    static const int MAX_PACKETSIZE = 1024;
};*/


class NetMessagesFactory
{
    public:
        NetMessagesFactory();
        virtual ~NetMessagesFactory();

        void addMessageModel(std::unique_ptr<NetMessage> msgModel);
        std::shared_ptr<NetMessage> createMessage(int type);
        int getNbrMsgTypes();

    protected:

    private:
        std::vector< std::unique_ptr<NetMessage> > m_msgModels;

    public:
        static const int NETMESSAGEID_SIZE;
        static const int NETMESSAGEID_MAX_NBR;
};

}

#endif // NETMESSAGESFACTORY_H
