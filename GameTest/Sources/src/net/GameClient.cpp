#include "net/GameClient.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/Types.h"

//const float GameClient::CLIENTWORLD_SYNC_DELAY = 0.1;

GameClient::GameClient() :
    m_world(true),
    m_curWorldId(0),
    m_isWaitingForWorldSync(false)
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
    m_isWaitingForWorldSync = false;
    m_curWorldId = 0;

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

    bool r = true;

    r = r & m_client->disconnectFromServer();
    this->cleanup();

    return r;
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

    if(m_client->isConnected())
        this->updateWorld(elapsedTime);
}


void GameClient::sendMsgTest(bool reliable, bool forceSend)
{
    auto testMsg = std::dynamic_pointer_cast<NetMessage_Test>(pou::NetEngine::createNetMessage(NetMessageType_Test));
    testMsg->test_value = glm::linearRand(0,100);
    testMsg->isReliable = reliable;

    m_client->sendMessage(testMsg, forceSend);
    std::cout<<"Client send test message with value:"<<testMsg->test_value<<" and id: "<<testMsg->id<<std::endl;
}

void GameClient::processMessage(std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    switch(msg->type)
    {
        case NetMessageType_Test:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_Test>(msg);
            std::cout<<"Client received test message with value: "<<castMsg->test_value<<" and id: "<<castMsg->id<<std::endl;
        }break;

        case NetMessageType_WorldInitialization:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldInitialization>(msg);
            m_curWorldId = castMsg->world_id;
            //m_isWaitingForWorldSync = false;

            std::cout<<"Received world id:"<< m_curWorldId<<std::endl;
        }break;
    }
}

void GameClient::updateWorld(const pou::Time &elapsedTime)
{
    if(m_curWorldId == 0 && !m_isWaitingForWorldSync)
    {
        m_isWaitingForWorldSync = true;
        auto msg = std::dynamic_pointer_cast<NetMessage_AskForWorldSync>(pou::NetEngine::createNetMessage(NetMessageType_AskForWorldSync));
        msg->isReliable = true;
        msg->world_id = 0;
        m_client->sendMessage(msg);

        std::cout<<"I'm asking for world sync !"<<std::endl;
    }
}


