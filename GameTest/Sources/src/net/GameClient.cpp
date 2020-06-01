#include "net/GameClient.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/Types.h"
#include "PouEngine/utils/Profiler.h"

#include "net/GameServer.h"

//const float GameClient::CLIENTWORLD_SYNC_DELAY = 0.1;

const int GameClient::TICKRATE = 60;
const float GameClient::SYNCDELAY = .1f;
const float GameClient::INTERPOLATIONDELAY = .1f;

GameClient::GameClient() :
    m_world(true, false),
    m_curWorldId(0),
    m_isWaitingForWorldSync(false),
    m_remainingTime(0)
{
    initializeNetMessages();
    pou::NetEngine::setSyncDelay(GameServer::SYNCDELAY);
    //pou::NetEngine::setMaxRewindAmount(100);

    m_lastPlayerWalkDirection = glm::vec2(0);
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

void GameClient::disconnectionCleanup()
{
    m_isWaitingForWorldSync = false;
    m_curWorldId = 0;
    m_world.destroy();
}

bool GameClient::connectToServer(const pou::NetAddress &address)
{
    if(!m_client)
        return (false);

    this->disconnectFromServer();

    return m_client->connectToServer(address);
}

bool GameClient::disconnectFromServer()
{
    if(!m_client)
        return (false);

    bool r = true;
    r = r & m_client->disconnectFromServer();

    this->disconnectionCleanup();

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

    pou::Profiler::pushClock("Update client world");
    if(m_client->isConnected())
        this->updateWorld(elapsedTime);
    pou::Profiler::popClock();
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
    pou::Logger::write("Client send test message with value:"+std::to_string(testMsg->test_value)+" and id: "+std::to_string(testMsg->id));
}

void GameClient::playerWalk(glm::vec2 direction)
{
    if(!m_client || m_curWorldId == 0)
        return;

   // m_world.playerWalk(m_curPlayerId, direction);

    if(m_lastPlayerWalkDirection != direction)
    {
        auto walkMsg = std::dynamic_pointer_cast<NetMessage_PlayerAction>(pou::NetEngine::createNetMessage(NetMessageType_PlayerAction));
        walkMsg->isReliable = true;
        walkMsg->clientTime = m_world.getLocalTime();

        walkMsg->playerAction.actionType    = PlayerActionType_Walk;
        walkMsg->playerAction.walkDirection = direction;

        m_client->sendMessage(walkMsg);
        m_lastPlayerWalkDirection = direction;

        //m_world.playerWalk(direction);

         m_world.addPlayerAction(m_curPlayerId, walkMsg->playerAction);
    }
}

///Protected

void GameClient::processMessage(std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    switch(msg->type)
    {
        case NetMessageType_Test:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_Test>(msg);
            pou::Logger::write("Client received test message with value: "+std::to_string(castMsg->test_value)+" and id: "+std::to_string(castMsg->id));
        }break;

        case NetMessageType_ConnectionStatus:{
            auto castMsg = std::dynamic_pointer_cast<pou::NetMessage_ConnectionStatus>(msg);
            if(castMsg->connectionStatus == pou::ConnectionStatus_Connected)
            {
                // Do something, probably
            }
            else if(castMsg->connectionStatus == pou::ConnectionStatus_Disconnected)
                this->disconnectionCleanup();
        }break;


        case NetMessageType_WorldInit:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldInit>(msg);
            m_curWorldId = castMsg->world_id;
            m_curPlayerId = castMsg->player_id;
            m_isWaitingForWorldSync = false;

            m_world.generateFromMsg(castMsg);

            pou::Logger::write("Received world #"+std::to_string(m_curWorldId));
        }break;

        case NetMessageType_WorldSync:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldSync>(msg);
            m_world.syncFromMsg(castMsg, m_curPlayerId, m_client->getRTT());
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
        msg->clientTime = -1;
        m_client->sendMessage(msg, true);

        return;
    } else if(m_curWorldId != 0) {
        if(m_syncTimer.update(elapsedTime) || !m_syncTimer.isActive())
        {
            auto msg = std::dynamic_pointer_cast<NetMessage_AskForWorldSync>(pou::NetEngine::createNetMessage(NetMessageType_AskForWorldSync));
            msg->clientTime = m_world.getLastSyncTime();
            m_client->sendMessage(msg, true);
            m_syncTimer.reset(GameClient::SYNCDELAY);
        }
    }


    pou::Time tickTime(1.0f/GameClient::TICKRATE);
    pou::Time totalTime = elapsedTime+m_remainingTime;

   /* while(totalTime > tickTime)
    {
        m_world.update(tickTime);
        totalTime -= tickTime;
    } */

    m_world.update(totalTime);
    totalTime = pou::Time(0);

    m_remainingTime = totalTime;

}


