#include "net/GameClient.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/Types.h"

//const float GameClient::CLIENTWORLD_SYNC_DELAY = 0.1;

const int GameClient::TICKRATE = 60;

GameClient::GameClient() :
    m_world(true),
    m_curWorldId(0),
    m_isWaitingForWorldSync(false),
    m_remainingTime(0)
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
    this->disconnectFromServer();

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

    this->disconnectFromServer();

    return m_client->connectToServer(address);
}

bool GameClient::disconnectFromServer(bool alreadyDisconnected)
{
    if(!m_client)
        return (false);

    bool r = true;

    if(!alreadyDisconnected)
        r = r & m_client->disconnectFromServer();

    m_isWaitingForWorldSync = false;
    m_curWorldId = 0;
    m_world.destroy();
    //this->cleanup();

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

void GameClient::render(pou::RenderWindow *renderWindow)
{
    m_world.render(renderWindow);
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

        case NetMessageType_ConnectionStatus:{
            auto castMsg = std::dynamic_pointer_cast<pou::NetMessage_ConnectionStatus>(msg);
            if(castMsg->connectionStatus == pou::ConnectionStatus_Connected)
            {
                // Do something, probably
            }
            else if(castMsg->connectionStatus == pou::ConnectionStatus_Disconnected)
                this->disconnectFromServer(true);
        }break;


        case NetMessageType_WorldInit:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldInit>(msg);
            m_curWorldId = castMsg->world_id;
            m_isWaitingForWorldSync = false;

            m_world.generate(castMsg);

            std::cout<<"Received world #"<< m_curWorldId<<std::endl;
        }break;
    }
}

void GameClient::updateWorld(const pou::Time &elapsedTime)
{
    if(m_curWorldId == 0 && !m_isWaitingForWorldSync)
    {
        m_isWaitingForWorldSync = true;
        auto msg = std::dynamic_pointer_cast<NetMessage_AskForWorldInit>(pou::NetEngine::createNetMessage(NetMessageType_AskForWorldInit));
        msg->isReliable = true;
        msg->world_id = 0;
        m_client->sendMessage(msg);

        return;
    }


    pou::Time tickTime(1.0f/GameClient::TICKRATE);
    pou::Time totalTime = elapsedTime+m_remainingTime;

    while(totalTime > tickTime)
    {
        m_world.update(tickTime);
        totalTime -= tickTime;
    }

    m_remainingTime = totalTime;

}


