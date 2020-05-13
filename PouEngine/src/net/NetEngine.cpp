#include "PouEngine/net/NetEngine.h"

namespace pou
{

NetEngine::NetEngine() : m_impl(nullptr)
{
    //ctor
}

NetEngine::~NetEngine()
{
    this->cleanup();
}


bool NetEngine::init(std::unique_ptr<AbstractNetImpl> impl)
{
    instance()->m_impl = std::move(impl);
    return instance()->m_impl->init();
}

bool NetEngine::cleanup()
{
    //if(m_impl)
      //  m_impl->shutdown();
    m_impl.release();

    return (true);
}

std::unique_ptr<AbstractServer> NetEngine::createServer()
{
    if(!instance()->m_impl)
        return (nullptr);
    return std::move(instance()->m_impl.get()->createServer());
}

std::unique_ptr<AbstractClient> NetEngine::createClient()
{
    if(!instance()->m_impl)
        return (nullptr);
    return std::move(instance()->m_impl.get()->createClient());
}


void NetEngine::addReliableMessageModel(std::unique_ptr<ReliableMessage> msgModel)
{
    instance()->m_reliableMsgFactory.addReliableMessageModel(std::move(msgModel));
}

std::shared_ptr<ReliableMessage> NetEngine::createReliableMessage(int type)
{
    return std::move(instance()->m_reliableMsgFactory.createReliableMessage(type));
}

int NetEngine::getNbrReliableMsgTypes()
{
    return instance()->m_reliableMsgFactory.getNbrReliableMsgTypes();
}

}
