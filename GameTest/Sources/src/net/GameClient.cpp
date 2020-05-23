#include "net/GameClient.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/Types.h"

GameClient::GameClient() :
    m_world(true)
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
    m_client->create(port);

    return (true);
}

void GameClient::cleanup()
{
    m_world.destroy();

    if(m_client)
    {
        m_client->destroy();
        m_client.release();
    }
}

bool GameClient::connectToServer(const pou::NetAddress &address)
{
    if(!m_client)
        return (false);

    return m_client->connectToServer(address);
}

bool GameClient::disconnectFromServer()
{
    if(!m_client)
        return (false);

    return m_client->disconnectFromServer();
}


void GameClient::update(const pou::Time &elapsedTime)
{
    if(!m_client)
        return;

    m_client->update(elapsedTime);

    std::list<std::shared_ptr<pou::NetMessage> > netMessages;
    m_client->receivePackets(netMessages);

    for(auto &msg : netMessages)
        this->processMessage(msg);
}


void GameClient::sendMsgTest(bool reliable, bool forceSend)
{
    auto testMsg = std::dynamic_pointer_cast<NetMessage_test>(pou::NetEngine::createNetMessage(NetMessageType_Test));//std::make_shared<ReliableMessage_test> ();
    testMsg->test_value = glm::linearRand(0,100);
    testMsg->isReliable = reliable;

    m_client->sendMessage(testMsg, forceSend);
    std::cout<<"Client send test message with value:"<<testMsg->test_value<<" and id: "<<testMsg->id<<std::endl;
}

void GameClient::processMessage(std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    if(msg->type == NetMessageType_Test)
    {
        auto castMsg = std::dynamic_pointer_cast<NetMessage_test>(msg);
        std::cout<<"Client received test message with value: "<<castMsg->test_value<<" and id: "<<castMsg->id<<std::endl;
    }
}



