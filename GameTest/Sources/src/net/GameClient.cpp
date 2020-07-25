#include "net/GameClient.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/Types.h"
#include "PouEngine/tools/Profiler.h"
#include "PouEngine/tools/RNGesus.h"
#include "PouEngine/core/MessageBus.h"

#include "net/GameServer.h"
#include "logic/GameMessageTypes.h"

//const float GameClient::CLIENTWORLD_SYNC_DELAY = 0.1;

const int       GameClient::TICKRATE    = 60;
const pou::Time GameClient::TICKDELAY(1.0f/GameClient::TICKRATE);
const int       GameClient::SYNCRATE    = 30;
const pou::Time GameClient::SYNCDELAY(1.0f/GameClient::SYNCRATE);
const float     GameClient::INTERPOLATIONDELAY = 0.0f;
const uint32_t  GameClient::MAX_PLAYER_REWIND = 200;

GameClient::GameClient() :
    //m_world(true, false),
    m_curWorldId(0),
    m_isWaitingForWorldSync(false),
    m_isWaitingForWorldGen(false),
    m_curCursorPos(0),
    m_remainingTime(0)
{
    initializeNetMessages();
    pou::NetEngine::setSyncDelay(GameServer::TICKRATE/GameServer::SYNCRATE);
    pou::NetEngine::setMaxRewindAmount(200);

    //m_lastPlayerWalkDirection = glm::vec2(0);
    m_lastServerAckTime = (uint32_t)(-1);

    pou::MessageBus::addListener(this, GameMessageType_World_CharacterDamaged);
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
        m_client.reset();
    }
}

void GameClient::disconnectionCleanup()
{
    m_isWaitingForWorldSync = false;
    m_curWorldId = 0;

    GameMessage_Game_ChangeWorld gameMsg;
    gameMsg.clientId    = 0;
    gameMsg.world       = nullptr;
    gameMsg.playerId    = 0;
    pou::MessageBus::postMessage(GameMessageType_Game_ChangeWorld, &gameMsg);

    m_world.reset();
    //m_world.destroy();
}

bool GameClient::connectToServer(const pou::NetAddress &address, std::shared_ptr<PlayerSave> playerSave)
{
    if(!m_client)
        return (false);

    this->disconnectFromServer();

    m_playerSave = playerSave;

    return m_client->connectToServer(address);
}

bool GameClient::disconnectFromServer()
{
    bool r = true;

    if(m_client)
        r = r & m_client->disconnectFromServer();

    pou::MessageBus::postMessage(GameMessageType_Net_Disconnected);

    this->disconnectionCleanup();

    return r;
}

const pou::NetAddress &GameClient::getServerAddress() const
{
    return m_client->getServerAddress();
}

Player* GameClient::getPlayer()
{
    if(!m_client || m_curWorldId == 0)
        return (nullptr);

    if(!m_world)
        return (nullptr);

    return m_world->getPlayer(m_curPlayerId);
}

float GameClient::getRTT() const
{
    return m_client->getRTT();
}


void GameClient::update(const pou::Time &elapsedTime)
{
    if(!m_client)
        return;

    m_remainingTime += elapsedTime;

    if(m_remainingTime < GameClient::TICKDELAY)
        return;

    float nbrTicks = (int)(m_remainingTime/GameClient::TICKDELAY);
    auto tickedElapsedTime = nbrTicks*GameClient::TICKDELAY;

    m_client->update(tickedElapsedTime);

    std::list<std::shared_ptr<pou::NetMessage> > netMessages;
    m_client->receivePackets(netMessages);

    for(auto &msg : netMessages)
        this->processMessage(msg);

    pou::Profiler::pushClock("Update client world");
    if(m_client->isConnected())
        this->updateWorld(tickedElapsedTime);
    else
        m_remainingTime -= tickedElapsedTime;
    pou::Profiler::popClock();
}


void GameClient::sendMsgTest(bool reliable, bool forceSend)
{
    auto testMsg = std::dynamic_pointer_cast<NetMessage_Test>(pou::NetEngine::createNetMessage(NetMessageType_Test));
    testMsg->test_value = pou::RNGesus::uniformInt(0,100);
    testMsg->isReliable = reliable;

    m_client->sendMessage(testMsg, forceSend);
    pou::Logger::write("Client send test message with value:"+std::to_string(testMsg->test_value)+" and id: "+std::to_string(testMsg->id));
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
                pou::MessageBus::postMessage(GameMessageType_Net_Connected);
            }
            else if(castMsg->connectionStatus == pou::ConnectionStatus_Disconnected)
                this->disconnectionCleanup();
        }break;


        case NetMessageType_WorldInit:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldInit>(msg);
            m_curWorldId = castMsg->world_id;
            m_curPlayerId = castMsg->player_id;
            m_isWaitingForWorldSync = false;

            m_world = std::make_unique<GameWorld>(true);
            m_world->generateFromMsg(castMsg);
            m_isWaitingForWorldGen = true;

            /*auto player = m_world->getPlayer(m_curPlayerId);
            player->setPlayerName(m_playerName);*/

            pou::Logger::write("Received world #"+std::to_string(m_curWorldId));
        }break;

        case NetMessageType_WorldSync:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_WorldSync>(msg);
            if(m_world && m_world->isReady())
                m_world->getSyncComponent()->syncWorldFromMsg(castMsg, m_curPlayerId, m_client->getRTT());

            if(uint32less(m_lastServerAckTime, castMsg->clientTime))
                m_lastServerAckTime = castMsg->clientTime;
        }break;
    }
}

void GameClient::updateWorld(const pou::Time &elapsedTime)
{
    if(m_world && m_world->isReady())
    {
       // int nbrTicks = 0;
        while(m_remainingTime > GameClient::TICKDELAY)
        {
            m_world->update(GameClient::TICKDELAY);
            m_remainingTime -= GameClient::TICKDELAY;
           // nbrTicks++;
        }
        //std::cout<<"NbrTicks:"<<nbrTicks<<std::endl;
    } else
        m_remainingTime = pou::Time(0);


    if(m_curWorldId == 0 && !m_isWaitingForWorldSync)
    {
        m_isWaitingForWorldSync = true;
        auto msg = std::dynamic_pointer_cast<NetMessage_PlayerSync>(pou::NetEngine::createNetMessage(NetMessageType_PlayerSync));
        msg->isReliable = true;
        msg->lastSyncTime = -1;
        msg->localTime = 0;

        msg->playerSave = m_playerSave;

        m_client->sendMessage(msg, true);

        return;
    }
    else if(m_curWorldId != 0 && m_world && m_world->isReady())
    {
        if(m_isWaitingForWorldGen)
        {
            m_isWaitingForWorldGen = false;

            GameMessage_Game_ChangeWorld gameMsg;
            gameMsg.clientId    = 0;
            gameMsg.world       = m_world.get();
            gameMsg.playerId    = m_curPlayerId;
            pou::MessageBus::postMessage(GameMessageType_Game_ChangeWorld, &gameMsg);
        }

        if(m_syncTimer.update(elapsedTime) || !m_syncTimer.isActive())
        {
            /*auto player = m_world->getPlayer(m_curPlayerId);
            player->setPlayerName(m_playerName);*/
            auto msg = std::dynamic_pointer_cast<NetMessage_PlayerSync>(pou::NetEngine::createNetMessage(NetMessageType_PlayerSync));

            m_world->getSyncComponent()->createPlayerSyncMsg(msg, m_curPlayerId, m_lastServerAckTime);
            m_client->sendMessage(msg, true);
            m_syncTimer.reset(GameClient::SYNCDELAY);
        }
    }
}

void GameClient::notify(pou::NotificationSender*, int notificationType, void* data)
{
    if(!m_world)
        return;

    auto player = m_world->getPlayer(m_curPlayerId);
    if(!player)
        return;

    auto playerEventMsg = std::dynamic_pointer_cast<NetMessage_PlayerEvent>(pou::NetEngine::createNetMessage(NetMessageType_PlayerEvent));

    playerEventMsg->isReliable  = true;
    playerEventMsg->localTime   = m_world->getLocalTime();

    if(notificationType == GameMessageType_World_CharacterDamaged)
    {
        auto *gameMsg = static_cast<GameMessage_World_CharacterDamaged*>(data);

        if(gameMsg->character == player)
            return;

        playerEventMsg->eventType   = PlayerEventType_CharacterDamaged;
        playerEventMsg->syncId      = gameMsg->character->getCharacterSyncId();
        playerEventMsg->direction   = gameMsg->direction;
        playerEventMsg->amount      = gameMsg->damages;

        m_client->sendMessage(playerEventMsg);
    }
}
