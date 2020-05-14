#include "PouEngine/net/NetMessagesFactory.h"

namespace pou
{


void NetMessage::serializeImpl(Stream *stream)
{
}

std::shared_ptr<NetMessage> NetMessage::msgAllocator()
{
    return std::make_shared<NetMessage>();
}

int NetMessage::serializeHeader(Stream *stream/*, bool flush = true*/)
{
    stream->serializeBool(isReliable);
    if(isReliable)
        stream->serializeBits(id, 16);

    /*if(flush)
    {
        stream->flush();
        return stream->computeBytes();
    }*/
    return 0;
}

int NetMessage::serialize(Stream *stream, bool computeBytesAndFlush)
{
    this->serializeHeader(stream/*, false*/);
    this->serializeImpl(stream);
    //stream->serializeBits(serial_check, 32);
    //if(stream->isWriting()) stream->flush();
    if(computeBytesAndFlush)
        return stream->computeBytes();
    return 0;
}

NetMessagesFactory::NetMessagesFactory()
{
    auto msg = std::make_unique<NetMessage>();
    msg.get()->type = 0;
    this->addMessageModel(std::move(msg));
}

NetMessagesFactory::~NetMessagesFactory()
{
    //dtor
}

void NetMessagesFactory::addMessageModel(std::unique_ptr<NetMessage> msgModel)
{
    m_msgModels.resize(msgModel.get()->type+1);
    m_msgModels[msgModel->type] = std::move(msgModel);
}

std::shared_ptr<NetMessage> NetMessagesFactory::createMessage(int type)
{
    if(type < 0 || type >= (int)m_msgModels.size())
        return (nullptr);
    auto ptr = m_msgModels[type].get()->msgAllocator();
    ptr->type = type;
    return ptr;
}

int NetMessagesFactory::getNbrMsgTypes()
{
    return m_msgModels.size();
}

}
