#include "net/GameServer.h"

#include "net/ReliableMessageTypes.h"

GameServer::GameServer()
{
    auto testMsg = std::make_unique<ReliableMessage_test> ();
    testMsg.get()->type = ReliableMessageType_Test;
    pou::NetEngine::addReliableMessageModel(std::move(testMsg));
}

GameServer::~GameServer()
{
    this->cleanup();
}

bool GameServer::create(unsigned short port)
{
    m_server = std::move(pou::NetEngine::createServer());
    m_server.get()->start(8,port);

    return (true);
}

void GameServer::cleanup()
{
    if(m_server)
    {
        m_server.get()->shutdown();
        m_server.release();
    }
}

void GameServer::update(const pou::Time &elapsedTime)
{
    if(!m_server)
        return;

    m_server.get()->update(elapsedTime);
}

/*const pou::NetAddress *GameServer::getAddress() const
{
    if(!m_server)
        return (nullptr);
    return &(m_server.get()->getAddress());
}*/

unsigned short GameServer::getPort() const
{
    if(!m_server)
        return 0;
    return m_server.get()->getPort();
}

void GameServer::sendReliableMsgTest()
{

}

