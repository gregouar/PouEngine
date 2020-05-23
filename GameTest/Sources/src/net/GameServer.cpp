#include "net/GameServer.h"

#include "net/NetMessageTypes.h"
#include "PouEngine/types.h"

GameServer::GameServer() :
    m_curWorldId(0)
{
    auto testMsg = std::make_unique<NetMessage_test> ();
    testMsg->type = NetMessageType_Test;
    pou::NetEngine::addNetMessageModel(std::move(testMsg));
}

GameServer::~GameServer()
{
    this->cleanup();
}

bool GameServer::create(unsigned short port)
{
    m_server = std::move(pou::NetEngine::createServer());
    m_server->start(8,port);

    return (true);
}

void GameServer::shutdown()
{
    m_worlds.clear();

    if(m_server)
    {
        m_server->shutdown();
        m_server.release();
    }
}

void GameServer::cleanup()
{
    this->shutdown();
}

void GameServer::update(const pou::Time &elapsedTime)
{
    if(!m_server)
        return;

    m_server->update(elapsedTime);

    std::list<std::pair<int, std::shared_ptr<pou::NetMessage> > > netMessages;
    m_server->receivePackets(netMessages);

    for(auto &clientAndMsg : netMessages)
        this->processMessage(clientAndMsg.first, clientAndMsg.second);
}

size_t GameServer::generateWorld()
{
   auto &world = m_worlds.insert({++m_curWorldId, GameWorld(false)}).first->second;
   world.generate();
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
            auto testMsg = std::dynamic_pointer_cast<NetMessage_test>(pou::NetEngine::createNetMessage(NetMessageType_Test));//std::make_shared<ReliableMessage_test> ();
            testMsg->test_value = glm::linearRand(0,100);
            testMsg->isReliable = reliable;

            m_server->sendMessage(i,testMsg, forceSend);
            //m_server->sendReliableBigMessage(i,testMsg);
            std::cout<<"Server send test message with value: "<<testMsg->test_value<<" and id: "<<testMsg->id<<std::endl;
        }
}


void GameServer::processMessage(int clientNbr, std::shared_ptr<pou::NetMessage> msg)
{
    if(!msg)
        return;

    if(msg->type == NetMessageType_Test)
    {
        auto castMsg = std::dynamic_pointer_cast<NetMessage_test>(msg);
        std::cout<<"Server received test message from client #"<<clientNbr<<" with value="<<castMsg->test_value<<" and id "<<castMsg->id<<std::endl;
    }
}

