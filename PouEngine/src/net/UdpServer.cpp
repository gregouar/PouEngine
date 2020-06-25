#include "PouEngine/net/UdpServer.h"


#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Hasher.h"
#include "PouEngine/net/UdpPacketTypes.h"

#include <vector>
#include <glm/gtc/random.hpp>

namespace pou
{


const float UdpServer::DEFAULT_DECONNECTIONPINGDELAY = 30.0f;
const float UdpServer::DEFAULT_PINGDELAY = .5f;

UdpServer::UdpServer() : m_pingDelay(DEFAULT_PINGDELAY), m_disconnectionPingDelay(DEFAULT_DECONNECTIONPINGDELAY)
{
    //ctor
}

UdpServer::~UdpServer()
{
    //dtor
}


bool UdpServer::start(uint16_t maxNbrClients, unsigned short port)
{
    if(maxNbrClients <= 0)
        return (false);

    if(!m_packetsExchanger.createSocket(port))
        return (false);

    m_packetsExchanger.setCompressor(std::make_unique<LZ4_Compressor>());

    m_port = m_packetsExchanger.getPort();

    m_clients.resize(maxNbrClients);

    return (true);
}

bool UdpServer::shutdown()
{
    m_packetsExchanger.destroy();

    return (true);
}

void UdpServer::update(const Time &elapsedTime)
{
    for(uint16_t i = 0 ; i < m_clients.size() ; ++i)
        if(m_clients[i].status != ConnectionStatus_Disconnected
        &&!m_clients[i].isLocalClient)
        {
            if(m_clients[i].lastPingAnswerTime + m_disconnectionPingDelay < m_curLocalTime)
            {
                Logger::write("Client timed out from: "+m_clients[i].address.getAddressString()
                              +"("+std::to_string(m_clients[i].clientSalt)+","+std::to_string(m_clients[i].serverSalt)+")");
                this->disconnectClient(i);
            }

            if(m_clients[i].lastPingTime + m_pingDelay < m_curLocalTime)
                this->sendConnectionMsg(i,ConnectionMessage_Ping);
        }

    m_packetsExchanger.update(elapsedTime);
    AbstractServer::update(elapsedTime);
}


void UdpServer::sendMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg, bool forceSend)
{
    /*if(m_connectionStatus != ConnectionStatus_Connected)
    {
        Logger::warning("Client cannot send message if not connected");
        return;
    }
    ClientAddress clientAddress = {m_serverAddress, m_serverSalt^m_salt};
    m_packetsExchanger.sendMessage(clientAddress, msg);*/

    if(clientNbr >= m_clients.size())
        return;

    auto &client = m_clients[clientNbr];

    if(client.status != ConnectionStatus_Connected)
        return;

    ClientAddress clientAddress = {client.address, client.serverSalt^client.clientSalt};
    m_packetsExchanger.sendMessage(clientAddress, msg, forceSend);

    ///if(forceSend)
      ///  m_clients[clientNbr].lastPingTime = m_curLocalTime;
}

void UdpServer::sendBigReliableMessage(uint16_t clientNbr, std::shared_ptr<NetMessage> msg)
{
    if(clientNbr >= m_clients.size())
        return;

    auto &client = m_clients[clientNbr];

    if(client.status != ConnectionStatus_Connected)
        return;

    ClientAddress clientAddress = {client.address, client.serverSalt^client.clientSalt};
    m_packetsExchanger.sendChunk(clientAddress, msg);
}


void UdpServer::receivePackets(std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages)
{
    std::list<UdpBuffer> packet_buffers;
    std::list<std::pair<ClientAddress, std::shared_ptr<NetMessage> > > reliableMessages;

    m_packetsExchanger.receivePackets(packet_buffers,reliableMessages);

    for(auto &buffer : packet_buffers)
        this->processPacket(buffer, netMessages);

    while(!m_disconnectionList.empty())
    {
        auto msg = std::dynamic_pointer_cast<NetMessage_ConnectionStatus>(pou::NetEngine::createNetMessage(0));
        msg->connectionStatus = ConnectionStatus_Disconnected;
        netMessages.push_back({m_disconnectionList.back(), msg});
        m_disconnectionList.pop_back();
    }

    for(auto &msg : reliableMessages)
    {
        auto clientNbr = findClientIndex(msg.first.address,
                                         msg.first.salt);
        if(clientNbr < m_clients.size())
            netMessages.push_back({clientNbr, msg.second});
    }
        //this->processMessage(msg);
}

/*void UdpServer::verifyMessage(std::pair<ClientAddress, std::shared_ptr<NetMessage> > addressAndMessage)
{
    auto clientNbr = findClientIndex(addressAndMessage.first.address,
                                     addressAndMessage.first.salt);
    if(clientNbr >= m_clients.size())
        return;


    std::cout<<"The server has got a reliable message dude ! Id:"<<addressAndMessage.second->id<<std::endl;
}*/


float UdpServer::getRTT(uint16_t clientNbr) const
{
    if(clientNbr >= m_clients.size())
        return (-1);

    auto &client = m_clients[clientNbr];
    if(client.status != ConnectionStatus_Connected)
        return (-1);

    ClientAddress clientAddress = {client.address, client.serverSalt^client.clientSalt};
    return m_packetsExchanger.getRTT(clientAddress);
}

///Protected

void UdpServer::processPacket(UdpBuffer &buffer, std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages)
{
    PacketType packetType = m_packetsExchanger.readPacketType(buffer);

    /*uint16_t clientNbr = findClientIndex(buffer.address);
    if(clientNbr != m_clients.size())
        m_clients[clientNbr].lastPingAnswerTime = m_curLocalTime;*/

   // std::cout<<"Server received packet of type: "<<packetType<<std::endl;

    if(packetType == PacketType_ConnectionMsg)
        this->processConnectionMessages(buffer, netMessages);
}


void UdpServer::processConnectionMessages(UdpBuffer &buffer, std::list<std::pair<int, std::shared_ptr<NetMessage> > > &netMessages)
{
    UdpPacket_ConnectionMsg packet;
    if(!m_packetsExchanger.readPacket(packet, buffer))
        return;

    uint16_t clientNbr = findClientIndex(buffer.address, packet.salt);
    if(clientNbr != m_clients.size())
        m_clients[clientNbr].lastPingAnswerTime = m_curLocalTime;


    if(packet.connectionMessage == ConnectionMessage_ConnectionRequest)
    {
        if(clientNbr != m_clients.size())
            if(m_clients[clientNbr].status != ConnectionStatus_Disconnected)
            {
                this->challengeConnexionFrom(clientNbr);
                return;
            }

        Logger::write("Client is trying to connect from: "+buffer.address.getAddressString()
                      +"("+std::to_string(packet.salt)+")");

        for(uint16_t i = 0 ; i < m_clients.size(); ++i)
            if(m_clients[i].status == ConnectionStatus_Disconnected)
            {
                clientNbr = i;
                break;
            }

        if(clientNbr == m_clients.size())
        {
            this->denyConnectionFrom(buffer.address);
            return;
        }

        m_clients[clientNbr].status     = ConnectionStatus_Challenging; //ConnectionStatus_Connecting
        m_clients[clientNbr].address    = std::move(buffer.address);
        m_clients[clientNbr].lastPingAnswerTime = m_curLocalTime;
        m_clients[clientNbr].lastPingTime       = m_curLocalTime;
        m_clients[clientNbr].clientSalt = packet.salt;
        m_clients[clientNbr].serverSalt = glm::linearRand(0, (int)pow(2,SALT_SIZE));
        m_clients[clientNbr].isLocalClient = false;

        this->challengeConnexionFrom(clientNbr);
    }

    if(packet.connectionMessage == ConnectionMessage_Challenge)
    {
        if(clientNbr == m_clients.size())
            return;

        this->allowConnectionFrom(clientNbr);


        auto msg = std::dynamic_pointer_cast<NetMessage_ConnectionStatus>(pou::NetEngine::createNetMessage(0));
        msg->connectionStatus = ConnectionStatus_Connected;
        netMessages.push_back({clientNbr, msg});
    }

    if(packet.connectionMessage == ConnectionMessage_Disconnection)
    {
        if(clientNbr != m_clients.size()
        && m_clients[clientNbr].status != ConnectionStatus_Disconnected)
        {
            /*m_clients[clientNbr].status = ConnectionStatus_Disconnected;
            Logger::write("Client disconnected from: "+buffer.address.getAddressString()
                          +"("+std::to_string(m_clients[clientNbr].clientSalt)+","+std::to_string(m_clients[clientNbr].serverSalt)+")");

            auto msg = std::dynamic_pointer_cast<NetMessage_ConnectionStatus>(pou::NetEngine::createNetMessage(0));
            msg->connectionStatus = ConnectionStatus_Disconnected;
            netMessages.push_back({clientNbr, msg});*/

            this->disconnectClient(clientNbr);
        }
    }
}

void UdpServer::sendConnectionMsg(uint16_t clientNbr, ConnectionMessage msg)
{
    if(clientNbr > m_clients.size())
        return;

    int salt = m_clients[clientNbr].serverSalt ^ m_clients[clientNbr].clientSalt;

    if(msg == ConnectionMessage_Challenge)
        salt = m_clients[clientNbr].serverSalt;

    this->sendConnectionMsg(m_clients[clientNbr].address, msg, salt);

    m_clients[clientNbr].lastPingTime = m_curLocalTime;
}

void UdpServer::sendConnectionMsg(NetAddress &address, ConnectionMessage msg, int salt)
{
    UdpPacket_ConnectionMsg packet;
    packet.type = PacketType_ConnectionMsg;
    packet.connectionMessage = msg;
    packet.salt = salt;
    m_packetsExchanger.sendPacket(address,packet);
}

void UdpServer::denyConnectionFrom(NetAddress &address)
{
    this->sendConnectionMsg(address, ConnectionMessage_Disconnection, 0);
}

void UdpServer::challengeConnexionFrom(uint16_t clientNbr)
{
    this->sendConnectionMsg(clientNbr, ConnectionMessage_Challenge);
}

void UdpServer::allowConnectionFrom(uint16_t clientNbr)
{
    if(clientNbr == m_clients.size())
        return;

    m_clients[clientNbr].status = ConnectionStatus_Connected; //ConnectionStatus_Connecting
    this->sendConnectionMsg(clientNbr, ConnectionMessage_ConnectionAccepted);
}

void UdpServer::disconnectClient(uint16_t clientNbr, bool sendMsg)
{
    if(clientNbr >= m_clients.size())
        return;

    Logger::write("Client disconnected from: "+m_clients[clientNbr].address.getAddressString()
                  +"("+std::to_string(m_clients[clientNbr].clientSalt)+","+std::to_string(m_clients[clientNbr].serverSalt)+")");

    if(m_clients[clientNbr].status != ConnectionStatus_Disconnected)
    {
        m_disconnectionList.push_back(clientNbr);
        m_clients[clientNbr].status = ConnectionStatus_Disconnected;

        this->sendConnectionMsg(clientNbr, ConnectionMessage_Disconnection);
    }
}

uint16_t UdpServer::findClientIndex(NetAddress &address, int salt)
{
    for(auto i = 0 ; i < (int)m_clients.size() ; ++i)
        if(m_clients[i].address == address)
        {
            if(m_clients[i].status != ConnectionStatus_Challenging
            && (m_clients[i].clientSalt^m_clients[i].serverSalt) == salt)
                return i;

            if(m_clients[i].status == ConnectionStatus_Challenging
            && (m_clients[i].clientSalt == salt || (m_clients[i].clientSalt^m_clients[i].serverSalt) == salt) )
                return i;
        }
    return m_clients.size();
}


/*uint16_t UdpServer::getMaxNbrClients() const
{
    return m_clients.size();
}*/

}
