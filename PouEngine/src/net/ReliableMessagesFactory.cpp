#include "PouEngine/net/ReliableMessagesFactory.h"

namespace pou
{


void ReliableMessage::serializeImpl(Stream *stream)
{
}

std::shared_ptr<ReliableMessage> ReliableMessage::msgAllocator()
{
    return std::move(std::make_shared<ReliableMessage>());
}

int ReliableMessage::serializeHeader(Stream *stream/*, bool flush = true*/)
{
    //stream->serializeInt(type, 0, nbr_messageTypes-1);
    stream->serializeBits(id, 16);

    /*if(flush)
    {
        stream->flush();
        return stream->computeBytes();
    }*/
    return 0;
}

int ReliableMessage::serialize(Stream *stream, bool computeBytesAndFlush)
{
    this->serializeHeader(stream/*, false*/);
    this->serializeImpl(stream);
    //stream->serializeBits(serial_check, 32);
    //if(stream->isWriting()) stream->flush();
    if(computeBytesAndFlush)
        return stream->computeBytes();
    return 0;
}

ReliableMessagesFactory::ReliableMessagesFactory()
{
    auto msg = std::make_unique<ReliableMessage>();
    msg.get()->type = 0;
    this->addReliableMessageModel(std::move(msg));
}

ReliableMessagesFactory::~ReliableMessagesFactory()
{
    //dtor
}

void ReliableMessagesFactory::addReliableMessageModel(std::unique_ptr<ReliableMessage> msgModel)
{
    m_msgModels.resize(msgModel.get()->type+1);
    m_msgModels[msgModel->type] = std::move(msgModel);
}

std::shared_ptr<ReliableMessage> ReliableMessagesFactory::createReliableMessage(int type)
{
    if(type < 0 || type >= (int)m_msgModels.size())
        return (nullptr);
    auto ptr = m_msgModels[type].get()->msgAllocator();
    ptr.get()->type = type;
    return ptr;
}

int ReliableMessagesFactory::getNbrReliableMsgTypes()
{
    return m_msgModels.size();
}

}
