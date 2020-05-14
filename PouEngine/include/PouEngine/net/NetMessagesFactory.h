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
    int type;
    bool isReliable;
    int id;

    virtual void serializeImpl(Stream *stream);
    virtual std::shared_ptr<NetMessage> msgAllocator();

    int serializeHeader(Stream *stream/*, bool flush = true*/);
    int serialize(Stream *stream, bool computeBytesAndFlush = true);
};


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
};

}

#endif // NETMESSAGESFACTORY_H
