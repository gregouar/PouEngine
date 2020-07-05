#include "net/GameServer.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/types.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Profiler.h"

#include "net/GameClient.h"

const int   GameServer::TICKRATE    = 60;
const pou::Time GameServer::TICKDELAY(1.0f/GameServer::TICKRATE);
const int   GameServer::SYNCRATE    = 20;
const pou::Time GameServer::SYNCDELAY(1.0f/GameServer::SYNCRATE);
///const int   GameServer::MAX_REWIND_AMOUNT = 200;

///const bool   GameServer::USEREWIND = false;

GameServer::GameServer() :
    m_serverIsRunning(false),
    m_curWorldId(0),
    m_remainingTime(0)
//    m_isInThread(false)
{
    initializeNetMessages();
    pou::NetEngine::setSyncDelay(GameServer::TICKRATE/GameServer::SYNCRATE);

    /**if(GameServer::USEREWIND)
        pou::NetEngine::setMaxRewindAmount(GameServer::MAX_REWIND_AMOUNT);**/
}

GameServer::~GameServer()
{
    this->cleanup();
}

bool GameServer::create(unsigned short port, bool allowLocalPlayers/*, bool launchInThread*/)
{
    m_server = std::move(pou::NetEngine::createServer());
    m_server->start(GameWorld::MAX_NBR_PLAYERS,port);
    m_serverIsRunning = true;

    /*if(launchInThread)
    {
        m_isInThread = true;
        m_serverThread = std::thread(&GameServer::threading, this);
    }*/

    m_allowLocalPlayers = allowLocalPlayers;

    return (true);
}

void GameServer::shutdown()
{
    if(m_serverIsRunning)
        pou::Logger::write("Server shutdown...");

    m_serverIsRunning = false;

    {
        //std::lock_guard<std::mutex> lock(m_serverMutex);
        m_worlds.clear();

        if(m_server)
        {
            m_server->shutdown();
            m_server.reset();
        }
    }

    //if(m_serverThread.joinable())
      //  m_serverThread.join();
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

    m_remainingTime += elapsedTime;
    if(m_remainingTime < GameServer::TICKDELAY)
        return;

    float nbrTicks = (int)(m_remainingTime/GameServer::TICKDELAY);
    auto tickedElapsedTime = nbrTicks*GameServer::TICKDELAY;

    m_server->update(tickedElapsedTime);

    std::list<std::pair<int, std::shared_ptr<pou::NetMessage> > > netMessages;
    m_server->receivePackets(netMessages);

    //if(!m_isInThread)
        pou::Profiler::pushClock("Update server worlds");
    this->updateWorlds(tickedElapsedTime);
    //if(!m_isInThread)
        pou::Profiler::popClock();

    for(auto &clientAndMsg : netMessages)
        this->processMessage(clientAndMsg.first, clientAndMsg.second);

    this->syncClients(tickedElapsedTime);
}


/*void GameServer::render(pou::RenderWindow *renderWindow, size_t localClientNbr)
{
    if(localClientNbr >= m_clientInfos.size())
        return;

    auto& clientInfos = m_clientInfos[localClientNbr];

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt == m_worlds.end())
        return;

    pou::Profiler::pushClock("Render player server world");
    worldIt->second->render(renderWindow);
    pou::Profiler::popClock();
}*/


void GameServer::syncClients(const pou::Time &elapsedTime)
{
    for(auto &clientInfosIt : m_clientInfos)
    {
        auto &clientInfos = clientInfosIt.second;
        auto clientId = clientInfosIt.first;

        if(clientInfos.isLocalPlayer)
            continue;

        if(clientInfos.world_id == 0)
        {
            auto &world = m_worlds.find(m_curWorldId)->second;

            clientInfos.lastSyncTime  = (uint32_t)(-1);
            clientInfos.world_id      = m_curWorldId;

            /*size_t player_id = world->askToAddPlayer(m_playerSave);
            clientInfos.player_id     = player_id;*/
        }

        if(!clientInfos.playerCreated)
        {
            auto worldIt = m_worlds.find(clientInfos.world_id);
            if(worldIt == m_worlds.end())
                continue;
            auto &world = worldIt->second;

            if(!world->isPlayerCreated(clientInfos.player_id))
                continue;

            auto worldInitMsg = std::dynamic_pointer_cast<NetMessage_WorldInit>
                                    (pou::NetEngine::createNetMessage(NetMessageType_WorldInit));

            worldInitMsg->world_id  = clientInfos.world_id;
            worldInitMsg->player_id = clientInfos.player_id;
            worldInitMsg->clientTime = (uint32_t)(-1);

            clientInfos.playerCreated = true;
            world->createWorldInitializationMsg(worldInitMsg);
            m_server->sendBigReliableMessage(clientId, worldInitMsg);

            pou::Logger::write("Sending WorldInit #"+std::to_string(m_curWorldId)+" to client #"+std::to_string(clientId));
            continue;
        }

        if(clientInfos.lastSyncTime == (uint32_t)(-1))
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

            worldSyncMsg->clientTime = clientInfos.localTime;
            world->getSyncComponent()->createWorldSyncMsg(worldSyncMsg, clientInfos.player_id, clientInfos.lastSyncTime);
            m_server->sendMessage(clientId, worldSyncMsg, true);
        }
    }
}

int GameServer::addLocalPlayer(std::shared_ptr<PlayerSave> playerSave/*const std::string &playerName*/)
{
    if(!m_allowLocalPlayers)
        return (-1);

    auto clientId = m_server->connectLocalClient();
    if(clientId == m_server->getMaxNbrClients())
        return (-1);

    auto worldIt = m_worlds.find(m_curWorldId);
    if(worldIt == m_worlds.end())
        return (-1);

    this->addClient(clientId, true);
    auto player_id = worldIt->second->askToAddPlayer(playerSave, true);
    m_clientInfos[clientId].world_id  = m_curWorldId;
    m_clientInfos[clientId].player_id = player_id;

    return clientId;
}

size_t GameServer::generateWorld()
{
    auto world = std::make_unique<GameWorld>(m_allowLocalPlayers);
    world->generate();

    m_worlds.insert({++m_curWorldId, std::move(world)});
   //auto &world = m_worlds.emplace(std::make_pair(++m_curWorldId, GameWorld(m_allowLocalPlayers))).first->second;

    return (m_curWorldId);
}

/**void GameServer::rewindWorld(size_t world_id, uint32_t time)
{
    auto worldIt = m_worlds.find(world_id);
    if(worldIt == m_worlds.end())
        return;
    worldIt->second.rewind(worldIt->second.getLocalTime() - time);
}**/

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


Player* GameServer::getPlayer(size_t clientId)
{
    auto [clientInfos, world] = this->getClientInfosAndWorld(clientId);

    if(!world)
        return (nullptr);

    return world->getPlayer(clientInfos->player_id);
}


void GameServer::sendMsgTest(bool reliable, bool forceSend)
{
    for(auto i = 0 ; i < m_server->getMaxNbrClients() ; ++i)
    {
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
}


void GameServer::processMessage(int clientId, std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    switch(msg->type)
    {
        case NetMessageType_Test:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_Test>(msg);
            pou::Logger::write("Server received test message from client #"+std::to_string(clientId)
                          +" with value="+std::to_string(castMsg->test_value)+" and id "+std::to_string(castMsg->id));
        }break;

        case NetMessageType_ConnectionStatus:{
            auto castMsg = std::dynamic_pointer_cast<pou::NetMessage_ConnectionStatus>(msg);
            if(castMsg->connectionStatus == pou::ConnectionStatus_Connected)
                this->addClient(clientId);
            else if(castMsg->connectionStatus == pou::ConnectionStatus_Disconnected)
                this->disconnectClient(clientId);
        }break;

        case NetMessageType_PlayerSync:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_PlayerSync>(msg);
            this->updateClientSync(clientId, castMsg);
        }break;

        case NetMessageType_PlayerEvent:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_PlayerEvent>(msg);
            this->processPlayerEvent(clientId, castMsg);
        }break;

        /*case NetMessageType_AskForWorldSync:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_AskForWorldSync>(msg);
            this->updateClientSync(clientId, castMsg);
        }break;*/

        /*case NetMessageType_PlayerAction:{
            auto castMsg = std::dynamic_pointer_cast<NetMessage_PlayerAction>(msg);
            this->processPlayerAction(clientId, castMsg->playerAction,  castMsg->clientTime);
        }break;*/
    }
}

void GameServer::updateClientSync(int clientId, std::shared_ptr<NetMessage_PlayerSync> msg)
{
    auto [clientInfos, world] = this->getClientInfosAndWorld(clientId);

    if(!world)
        return;

    if(msg->lastSyncTime == (uint32_t)(-1))
    {
        size_t player_id = world->askToAddPlayer(msg->playerSave);

        auto clientInfosIt = m_clientInfos.find(clientId);
        clientInfosIt->second.player_id = player_id;

        return;
    }

    //std::cout<<"Received Sync from client #"<<clientId<<" of time: "<<msg->localTime<<std::endl;

    world->getSyncComponent()->syncPlayerFromMsg(msg, clientInfos->player_id, m_server->getRTT(clientId));

    if(uint32less(clientInfos->lastSyncTime,msg->lastSyncTime))
        clientInfos->lastSyncTime = msg->lastSyncTime;
    if(uint32less(clientInfos->localTime,msg->localTime))
    {
        clientInfos->localTime = msg->localTime;
        ///world->getPlayer(clientInfos->player_id)->setTimeShift(timeShift);
    }
}

void GameServer::processPlayerEvent(int clientId, std::shared_ptr<NetMessage_PlayerEvent> msg)
{
    auto [clientInfos, world] = this->getClientInfosAndWorld(clientId);

    if(!world)
        return;

    msg->playerId = clientInfos->player_id;
    world->getSyncComponent()->addPlayerEvent(msg/*, clientInfos->player_id*/);
}

/*void GameServer::updateClientSync(int clientId, std::shared_ptr<NetMessage_AskForWorldSync> msg)
{
    if(msg->lastSyncTime == (uint32_t)(-1))
        return;

    auto [clientInfos, world] = this->getClientInfosAndWorld(clientId);

    if(!world)
        return;

    int32_t timeShift = 0;
    ///if(!GameServer::USEREWIND)
    {
        auto minActionTime = std::max(world->getLocalTime(), clientInfos->lastActionTime);

        if(clientInfos->localTime != 0)
            timeShift = minActionTime - clientInfos->localTime;
        else if(!msg->lastPlayerActions.empty())
            timeShift = minActionTime - msg->lastPlayerActions.begin()->first;

       // timeShift = std::max(0,timeShift);

       // std::cout<<"LastActionTime:"<<clientInfos->lastActionTime<<std::endl;
       // std::cout<<"TimeShift:"<<timeShift<<std::endl;
    }
*/
    /**if(GameServer::USEREWIND )
        if(uint32less(clientInfos->localTime,msg->localTime))
            world->removeAllPlayerActions(clientInfos->player_id, clientInfos->localTime+timeShift+1);**/
/*
    glm::vec2 lastPlayerWalk(0);
    uint32_t lastPlayerWalkTime = 0;

    for(auto playerActionIt : msg->lastPlayerActions)
    {
        auto& [playerActionTime, playerAction] = playerActionIt;
        if(uint32less(clientInfos->localTime,playerActionTime))
        {
            world->addPlayerAction(clientInfos->player_id, playerAction,  playerActionTime+timeShift);
            if(playerAction.actionType == PlayerActionType_Walk)
            {
                lastPlayerWalk      = playerAction.direction;
                lastPlayerWalkTime  = playerActionTime;

                if(playerAction.direction != glm::vec2(0))
                if(playerActionTime + timeShift > clientInfos->lastActionTime)
                    clientInfos->lastActionTime = playerActionTime + timeShift;
            }
            //std::cout<<"Client ActionTime:"<<playerActionTime<<std::endl;
            //std::cout<<"Server ActionTime:"<<playerActionTime+timeShift<<std::endl;
        }
    }
*/
    /**if(GameServer::USEREWIND )
    {
        lastPlayerWalkTime += GameClient::TICKRATE/GameClient::SYNCRATE*2;
        if(lastPlayerWalk != glm::vec2(0) && lastPlayerWalkTime > msg->localTime)
        {
            PlayerAction tempAction;
            tempAction.actionType   = PlayerActionType_Walk;
            tempAction.direction    = glm::vec2(0);
            world->addPlayerAction(clientInfos->player_id, tempAction,  lastPlayerWalkTime+timeShift);
        }
    } else**/ /* {
        PlayerAction tempAction;
        tempAction.actionType   = PlayerActionType_Walk;
        tempAction.direction    = glm::vec2(0);
        world->addPlayerAction(clientInfos->player_id, tempAction,  lastPlayerWalkTime+timeShift+1);
    }


    if(uint32less(clientInfos->lastSyncTime,msg->lastSyncTime))
        clientInfos->lastSyncTime = msg->lastSyncTime;
    if(uint32less(clientInfos->localTime,msg->localTime))
    {
        clientInfos->localTime = msg->localTime;
        ///world->getPlayer(clientInfos->player_id)->setTimeShift(timeShift);
    }
}*/

/*void GameServer::addPlayerAction(size_t clientId, const PlayerAction &action)
{
    auto [clientInfos, world] = this->getClientInfosAndWorld(clientId);

    if(world)
        world->addPlayerAction(clientInfos->player_id, action);
}*/


void GameServer::addClient(int clientId, bool isLocalClient)
{
    GameClientInfos clientInfos;
    clientInfos.isLocalPlayer = isLocalClient;
    clientInfos.lastActionTime = 0;
    clientInfos.lastSyncTime = (uint32_t)(-1);
    clientInfos.localTime = 0;
    clientInfos.playerCreated = false;
    clientInfos.player_id = 0;
    clientInfos.world_id = 0;

    m_clientInfos.insert({clientId, clientInfos});
}

void GameServer::disconnectClient(int clientId)
{
    auto it = m_clientInfos.find(clientId);
    if(it == m_clientInfos.end())
        return;

    auto clientInfos = it->second;

    auto worldIt = m_worlds.find(clientInfos.world_id);
    if(worldIt != m_worlds.end())
        worldIt->second->askToRemovePlayer(clientInfos.player_id);

    m_clientInfos.erase(it);
}

void GameServer::updateWorlds(const pou::Time &elapsedTime)
{
    pou::Time totalTime = m_remainingTime;

    auto tickTime = GameServer::TICKDELAY;
    if(m_allowLocalPlayers)
        tickTime = GameClient::TICKDELAY;

    while(totalTime >= tickTime)
    {
        for(auto &world : m_worlds)
            world.second->update(tickTime);
        totalTime -= tickTime;
    }

    m_remainingTime = totalTime;
}

/*void GameServer::threading()
{
    pou::Clock clock;
    pou::Time totalTime = pou::Time(0);

    clock.restart();
    while(m_serverIsRunning)
    {
        pou::Time elapsedTime = clock.restart();
        totalTime += elapsedTime;

        if(totalTime >= GameServer::TICKDELAY)
        {
            std::lock_guard<std::mutex> lock(m_serverMutex);
            this->update(totalTime);
            totalTime = pou::Time(0);
        }
    }
}*/

std::pair<GameClientInfos*, GameWorld*> GameServer::getClientInfosAndWorld(size_t clientId)
{
    auto clientInfosIt = m_clientInfos.find(clientId);
    if(clientInfosIt == m_clientInfos.end())
        return {nullptr, nullptr};
    auto *clientInfos = &clientInfosIt->second;

    if(clientInfos->world_id == 0)
        return {clientInfos, nullptr};

    auto worldIt = m_worlds.find(clientInfos->world_id);
    if(worldIt == m_worlds.end())
        return {clientInfos,nullptr};

    return {clientInfos, worldIt->second.get()};
}

