#include "net/GameServer.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/types.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Profiler.h"

#include "net/GameClient.h"

const int GameServer::TICKRATE = 60;
const float GameServer::SYNCDELAY = 1.0/30.0;

GameServer::GameServer() :
    m_serverIsRunning(false),
    m_curWorldId(0),
    m_remainingTime(0),
    m_isInThread(false)
{
    initializeNetMessages();
    pou::NetEngine::setSyncDelay(GameServer::SYNCDELAY);
    pou::NetEngine::setMaxRewindAmount(100);
}

GameServer::~GameServer()
{
    this->cleanup();
}

bool GameServer::create(unsigned short port, bool allowLocalPlayers, bool launchInThread)
{
    m_server = std::move(pou::NetEngine::createServer());
    m_server->start(GameWorld::MAX_NBR_PLAYERS,port);
    m_serverIsRunning = true;

    if(launchInThread)
    {
        m_isInThread = true;
        m_serverThread = std::thread(&GameServer::threading, this);
    }

    m_allowLocalPlayers = allowLocalPlayers;

    return (true);
}

void GameServer::shutdown()
{
    if(m_serverIsRunning)
        pou::Logger::write("Server shutdown...");

    m_serverIsRunning = false;

    {
        std::lock_guard<std::mutex> lock(m_serverMutex);
        m_worlds.clear();

        if(m_server)
        {
            m_server->shutdown();
            m_server.release();
        }
    }

    if(m_serverThread.joinable())
        m_serverThread.join();
}

void GameServer::cleanup()
{
    this->shutdown();
}

void GameServer::update(const pou::Time &elapsedTime)
{
    if(!m_serverIsRunning)
        return;

    if(!m_server)
        return;

    m_server->update(elapsedTime);

    std::list<std::pair<int, std::shared_ptr<pou::NetMessage> > > netMessages;
    m_server->receivePackets(netMessages);

    for(auto &clientAndMsg : netMessages)
        this->processMessage(clientAndMsg.first, clientAndMsg.second);

    if(!m_isInThread)
        pou::Profiler::pushClock("Update server worlds");
    this->updateWorlds(elapsedTime);
    if(!m_isInThread)
        pou::Profiler::popClock();

    this->syncClients(elapsedTime);
}


void GameServer::render(pou::RenderWindow *renderWindow, size_t localClientNbr)
{
    if(localClientNbr >= m_clientInfos.size())
        return;

    auto& clientInfos = m_clientInfos[localClientNbr];

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;

    worldIt->second.render(renderWindow);
}


void GameServer::syncClients(const pou::Time &elapsedTime)
{
    for(auto &clientInfosIt : m_clientInfos)
    {
        auto &clientInfos = clientInfosIt.second;
        auto clientNbr = clientInfosIt.first;

        if(clientInfos.isLocalPlayer)
            continue;

        if(clientInfos.world_id == 0)
        {
            auto &world = m_worlds.find(m_curWorldId)->second;
            size_t player_id = world.askToAddPlayer();

            clientInfos.localTime     = -1;
            clientInfos.world_id      = m_curWorldId;
            clientInfos.player_id     = player_id;
        }

        if(!clientInfos.playerCreated)
        {
            auto worldIt = m_worlds.find(clientInfos.world_id);
            if(worldIt == m_worlds.end())
                continue;
            auto &world = worldIt->second;

            if(!world.isPlayerCreated(clientInfos.player_id))
                continue;

            auto worldInitMsg = std::dynamic_pointer_cast<NetMessage_WorldInit>
                                    (pou::NetEngine::createNetMessage(NetMessageType_WorldInit));

            worldInitMsg->world_id  = clientInfos.world_id;
            worldInitMsg->player_id = clientInfos.player_id;

            clientInfos.playerCreated = true;
            world.createWorldInitializationMsg(worldInitMsg);
            m_server->sendReliableBigMessage(clientNbr, worldInitMsg);

            pou::Logger::write("Sending WorldInit #"+std::to_string(m_curWorldId)+" to client #"+std::to_string(clientNbr));
            continue;
        }

        if(clientInfos.localTime == -1)
            continue;

        auto worldIt = m_worlds.find(clientInfos.world_id);
        if(worldIt == m_worlds.end())
            continue;
        auto &world = worldIt->second;

        auto &syncTimer = clientInfos.syncTimer;
        if(syncTimer.update(elapsedTime) || !syncTimer.isActive())
        {
            syncTimer.reset(GameServer::SYNCDELAY);
            auto worldSyncMsg = std::dynamic_pointer_cast<NetMessage_WorldSync>
                                    (pou::NetEngine::createNetMessage(NetMessageType_WorldSync));

            world.createWorldSyncMsg(worldSyncMsg, clientInfos.player_id, clientInfos.localTime);
            m_server->sendMessage(clientNbr, worldSyncMsg, true);
        }
    }
}

int GameServer::addLocalPlayer()
{
    if(!m_allowLocalPlayers)
        return (-1);

    auto clientNbr = m_server->connectLocalClient();
    if(clientNbr == m_server->getMaxNbrClients())
        return (-1);

    auto worldIt = m_worlds.find(m_curWorldId);
    if(worldIt == m_worlds.end())
        return (-1);

    this->addClient(clientNbr, true);
    auto player_id = worldIt->second.askToAddPlayer(true);
    m_clientInfos[clientNbr].world_id  = m_curWorldId;
    m_clientInfos[clientNbr].player_id = player_id;

    return clientNbr;
}

size_t GameServer::generateWorld()
{
   auto &world = m_worlds.insert({++m_curWorldId, GameWorld(m_allowLocalPlayers,true)}).first->second;
   world.generate();

   return (m_curWorldId);
}

void GameServer::rewindWorld(size_t world_id, float time)
{
    auto worldIt = m_worlds.find(world_id);
    if(worldIt == m_worlds.end())
        return;
    worldIt->second.rewind(time);
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
    return m_server->getPort();
}

void GameServer::sendMsgTest(bool reliable, bool forceSend)
{
    for(auto i = 0 ; i < m_server->getMaxNbrClients() ; ++i)
        if(m_server->isClientConnected(i))
        {
            auto testMsg = std::dynamic_pointer_cast<NetMessage_Test>(pou::NetEngine::createNetMessage(NetMessageType_Test));//std::make_shared<ReliableMessage_test> ();
            testMsg->test_value = glm::linearRand(0,100);
            testMsg->isReliable = reliable;

            m_server->sendMessage(i,testMsg, forceSend);
            //m_server->sendReliableBigMessage(i,testMsg);
            pou::Logger::write("Server send test message with value: "+std::to_string(testMsg->test_value)+" and id: "+std::to_string(testMsg->id));
        }
}


void GameServer::processMessage(int clientNbr, std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    switch(msg->type)
    {
        case NetMessageType_Test:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_Test>(msg);
            pou::Logger::write("Server received test message from client #"+std::to_string(clientNbr)
                          +" with value="+std::to_string(castMsg->test_value)+" and id "+std::to_string(castMsg->id));
        }break;

        case NetMessageType_ConnectionStatus:{
            auto castMsg = std::dynamic_pointer_cast<pou::NetMessage_ConnectionStatus>(msg);
            if(castMsg->connectionStatus == pou::ConnectionStatus_Connected)
                this->addClient(clientNbr);
            else if(castMsg->connectionStatus == pou::ConnectionStatus_Disconnected)
                this->disconnectClient(clientNbr);
        }break;

        case NetMessageType_AskForWorldSync:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_AskForWorldSync>(msg);
            this->updateClientSync(clientNbr, castMsg);
        }break;


        case NetMessageType_PlayerAction:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_PlayerAction>(msg);
            this->processPlayerActions(clientNbr, castMsg);
        }break;
    }
}

void GameServer::updateClientSync(int clientNbr, std::shared_ptr<NetMessage_AskForWorldSync> msg)
{
    auto clientInfosIt = m_clientInfos.find(clientNbr);
    if(clientInfosIt == m_clientInfos.end())
        return;
    auto &clientInfos = clientInfosIt->second;

    if(clientInfos.world_id == 0)
        return;

    if(msg->clientTime < 0)
        return;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;
    auto &world = worldIt->second;

    if(msg->clientTime > clientInfos.localTime)
        clientInfos.localTime = msg->clientTime;
}

void GameServer::processPlayerActions(int clientNbr, std::shared_ptr<NetMessage_PlayerAction> msg)
{
    /*auto clientInfosIt = m_clientInfos.find(clientNbr);
    if(clientInfosIt == m_clientInfos.end())
        return;
    auto &clientInfos = clientInfosIt->second;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;
    auto &world = worldIt->second;*/

    /*switch(msg->playerAction.actionType)
    {
        case PlayerActionType_Walk:{
            this->playerWalk(clientNbr, msg->walkDirection, msg->clientTime);
            //world.playerWalk(clientInfos.player_id, msg->walkDirection, msg->clientTime);
        }break;

    }*/

    auto clientInfosIt = m_clientInfos.find(clientNbr);
    if(clientInfosIt == m_clientInfos.end())
        return;
    auto &clientInfos = clientInfosIt->second;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;
    auto &world = worldIt->second;

    world.addPlayerAction(clientInfos.player_id, msg->playerAction, msg->clientTime);
}

void GameServer::playerWalk(size_t clientNbr, glm::vec2 direction, float localTime)
{
    auto clientInfosIt = m_clientInfos.find(clientNbr);
    if(clientInfosIt == m_clientInfos.end())
        return;
    auto &clientInfos = clientInfosIt->second;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;
    auto &world = worldIt->second;

    PlayerAction playerAction;
    playerAction.actionType = PlayerActionType_Walk;
    playerAction.walkDirection = direction;

    world.addPlayerAction(clientInfos.player_id, playerAction, localTime);

    //world.playerWalk(clientInfos.player_id, direction, localTime);
}

void GameServer::addClient(int clientNbr, bool isLocalClient)
{
    GameClientInfos clientInfos = {0,0,-1,false,isLocalClient};
    m_clientInfos.insert({clientNbr, clientInfos});
}

void GameServer::disconnectClient(int clientNbr)
{
    auto it = m_clientInfos.find(clientNbr);
    if(it == m_clientInfos.end())
        return;

    auto clientInfos = it->second;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt != m_worlds.end())
        worldIt->second.askToRemovePlayer(clientInfos.player_id);

    m_clientInfos.erase(it);
}

void GameServer::updateWorlds(const pou::Time &elapsedTime)
{
    pou::Time tickTime(1.0f/GameServer::TICKRATE);
    pou::Time totalTime = elapsedTime+m_remainingTime;

    if(m_allowLocalPlayers)
        tickTime = pou::Time(1.0f/GameClient::TICKRATE);

    while(totalTime > tickTime)
    {
        for(auto &world : m_worlds)
            world.second.update(tickTime);
        totalTime -= tickTime;
    }

    m_remainingTime = totalTime;
}

void GameServer::threading()
{
    pou::Clock clock;
    pou::Time tickTime(1.0f/GameServer::TICKRATE);
    pou::Time totalTime = pou::Time(0);

    clock.restart();
    while(m_serverIsRunning)
    {
        pou::Time elapsedTime = clock.restart();
        totalTime += elapsedTime;

        if(totalTime >= tickTime)
        {
            std::lock_guard<std::mutex> lock(m_serverMutex);
            this->update(totalTime);
            totalTime = pou::Time(0);
        }
    }
}

