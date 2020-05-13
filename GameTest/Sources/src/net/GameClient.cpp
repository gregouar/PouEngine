#include "net/GameClient.h"

#include "net/ReliableMessageTypes.h"

GameClient::GameClient()
{
    //ctor
}

GameClient::~GameClient()
{
    this->cleanup();
}

bool GameClient::create(unsigned short port)
{
    m_client = std::move(pou::NetEngine::createClient());
    m_client.get()->create(port);

    return (true);
}

void GameClient::cleanup()
{
    if(m_client)
    {
        m_client.get()->destroy();
        m_client.release();
    }
}

bool GameClient::connectToServer(const pou::NetAddress &address)
{
    if(!m_client)
        return (false);

    return m_client.get()->connectToServer(address);
}

bool GameClient::disconnectFromServer()
{
    if(!m_client)
        return (false);

    return m_client.get()->disconnectFromServer();
}


void GameClient::update(const pou::Time &elapsedTime)
{
    if(!m_client)
        return;

    m_client.get()->update(elapsedTime);
}


void GameClient::sendReliableMsgTest()
{
    auto testMsg = std::dynamic_pointer_cast<ReliableMessage_test>(pou::NetEngine::createReliableMessage(ReliableMessageType_Test));//std::make_shared<ReliableMessage_test> ();
    testMsg.get()->test_value = 42;

    m_client.get()->sendReliableMessage(std::move(testMsg));
}



