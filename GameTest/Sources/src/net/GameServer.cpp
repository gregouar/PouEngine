#include "net/GameServer.h"

GameServer::GameServer()
{
    //ctor
}

GameServer::~GameServer()
{
    this->cleanup();
}

bool GameServer::create()
{
    m_server = std::move(pou::NetEngine::createServer());
    m_server.get()->start(8);

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

