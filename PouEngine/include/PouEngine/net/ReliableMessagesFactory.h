#ifndef RELIABLEMESSAGESFACTORY_H
#define RELIABLEMESSAGESFACTORY_H

#include "PouEngine/utils/ReadStream.h"
#include "PouEngine/utils/WriteStream.h"

#include <vector>
#include <memory>

namespace pou
{


struct ReliableMessage
{
    int type;
    int id;

    virtual void serializeImpl(Stream *stream);
    virtual std::shared_ptr<ReliableMessage> msgAllocator();

    int serializeHeader(Stream *stream/*, bool flush = true*/);
    int serialize(Stream *stream, bool computeBytesAndFlush = true);
};

class ReliableMessagesFactory
{
    public:
        ReliableMessagesFactory();
        virtual ~ReliableMessagesFactory();

        void addReliableMessageModel(std::unique_ptr<ReliableMessage> msgModel);
        std::shared_ptr<ReliableMessage> createReliableMessage(int type);
        int getNbrReliableMsgTypes();

    protected:

    private:
        std::vector< std::unique_ptr<ReliableMessage> > m_msgModels;
};

}

#endif // RELIABLEMESSAGESFACTORY_H
