#include "PouEngine/net/NetEngine.h"

namespace pou
{

NetEngine::NetEngine() : m_impl(nullptr)
{
    m_syncDelay = 0;
    m_maxRewindAmount = 0;
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
    m_impl.reset();

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


void NetEngine::addNetMessageModel(std::unique_ptr<NetMessage> msgModel)
{
    instance()->m_netMsgFactory.addMessageModel(std::move(msgModel));
}

std::shared_ptr<NetMessage> NetEngine::createNetMessage(int type)
{
    return std::move(instance()->m_netMsgFactory.createMessage(type));
}

int NetEngine::getNbrNetMsgTypes()
{
    return instance()->m_netMsgFactory.getNbrMsgTypes();
}

void NetEngine::setSyncDelay(float delay)
{
    instance()->m_syncDelay = delay;
}

float NetEngine::getSyncDelay()
{
    return instance()->m_syncDelay;
}

void NetEngine::setMaxRewindAmount(size_t rewindAmount)
{
    instance()->m_maxRewindAmount = rewindAmount;
}

float NetEngine::getMaxRewindAmount()
{
    return instance()->m_maxRewindAmount;
}

}
