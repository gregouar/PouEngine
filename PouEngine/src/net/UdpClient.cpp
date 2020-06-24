#include "PouEngine/net/UdpClient.h"

#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/tools/Logger.h"
#include "PouEngine/tools/Hasher.h"


#include "PouEngine/core/VApp.h"

#include <vector>
#include <glm/gtc/random.hpp>

namespace pou
{

const float UdpClient::CONNECTING_ATTEMPT_DELAY = 1.0f;
const float UdpClient::CONNECTING_MAX_TIME = 10.0f;
const float UdpClient::DEFAULT_PINGDELAY = .5f;
const float UdpClient::DEFAULT_DECONNECTIONPINGDELAY = 30.0f;


UdpClient::UdpClient() :
    m_pingDelay(UdpClient::DEFAULT_PINGDELAY),
    m_disconnectionPingDelay(UdpClient::DEFAULT_DECONNECTIONPINGDELAY),
    m_disconnection(false)
{
    //ctor
}

UdpClient::~UdpClient()
{
    //dtor
}


bool UdpClient::create(unsigned short port)
{
    if(!m_packetsExchanger.createSocket(port))
        return (false);

    m_packetsExchanger.setCompressor(std::make_unique<LZ4_Compressor>());

    m_port = m_packetsExchanger.getPort();

    return (true);
}

bool UdpClient::destroy()
{
    m_packetsExchanger.destroy();

    return (true);
}

bool UdpClient::connectToServer(const NetAddress &serverAddress)
{
    if(m_connectionStatus == ConnectionStatus_Connected)
        this->disconnectFromServer();

    m_connectionStatus  = ConnectionStatus_Connecting;
    m_serverAddress     = serverAddress;
    m_salt              = glm::linearRand(0, (int)pow(2,SALT_SIZE));

    m_connectingTimer.reset(CONNECTING_MAX_TIME);

    return (true);
}

bool UdpClient::disconnectFromServer()
{
    if(m_connectionStatus != ConnectionStatus_Disconnected)
    {
        Logger::write("Disconnected from server: "+m_serverAddress.getAddressString()
                      +"("+std::to_string(m_salt)+","+std::to_string(m_serverSalt)+")");
        for(auto i = 0 ; i < 5 ; ++i)
            this->sendConnectionMsg(m_serverAddress, ConnectionMessage_Disconnection);

        m_disconnection = true;
    }

    m_connectionStatus = ConnectionStatus_Disconnected;

    return (true);
}


void UdpClient::update(const Time &elapsedTime)
{
    if(m_connectionStatus != ConnectionStatus_Connected
    && m_connectingTimer.update(elapsedTime))
    {
        m_connectionStatus = ConnectionStatus_Disconnected;
        Logger::warning("Could not connect to: "+m_serverAddress.getAddressString());
    }

    if(m_connectionStatus == ConnectionStatus_Connecting)
    {
        if(m_connectingAttemptTimer.update(elapsedTime)
        || !m_connectingAttemptTimer.isActive())
        {
            this->tryToConnect();
            m_connectingAttemptTimer.reset(CONNECTING_ATTEMPT_DELAY);
        }
    }

    if(m_connectionStatus == ConnectionStatus_Challenging)
    {
        if(m_connectingAttemptTimer.update(elapsedTime)
        || !m_connectingAttemptTimer.isActive())
        {
            this->tryToChallenge();
            m_connectingAttemptTimer.reset(CONNECTING_ATTEMPT_DELAY);
        }
    }

    if(m_connectionStatus == ConnectionStatus_Connected)
        if(m_lastServerAnswerPingTime + m_disconnectionPingDelay < m_curLocalTime)
        {
            Logger::write("Server connection timed out ("+std::to_string(m_salt)+","+std::to_string(m_serverSalt)+")");
            this->disconnectFromServer();
        }

    if(m_connectionStatus != ConnectionStatus_Disconnected
    && m_lastServerPingTime + m_pingDelay < m_curLocalTime)
        this->sendConnectionMsg(m_serverAddress, ConnectionMessage_Ping);

    m_packetsExchanger.update(elapsedTime);

    AbstractClient::update(elapsedTime);
}


void UdpClient::sendMessage(std::shared_ptr<NetMessage> msg, bool forceSend)
{
    if(m_connectionStatus != ConnectionStatus_Connected)
    {
        Logger::warning("Client cannot send message if not connected");
        return;
    }
    ClientAddress clientAddress = {m_serverAddress, m_serverSalt^m_salt};
    m_packetsExchanger.sendMessage(clientAddress, msg, forceSend);

    ///if(forceSend)
        ///m_lastServerPingTime = m_curLocalTime;
}

void UdpClient::sendReliableBigMessage(std::shared_ptr<NetMessage> msg)
{
    if(m_connectionStatus != ConnectionStatus_Connected)
    {
        Logger::warning("Client cannot send message if not connected");
        return;
    }
    ClientAddress clientAddress = {m_serverAddress, m_serverSalt^m_salt};
    m_packetsExchanger.sendChunk(clientAddress, msg);
}


void UdpClient::receivePackets(std::list<std::shared_ptr<NetMessage> > &netMessages)
{
    std::list<UdpBuffer> packet_buffers;
    std::list<std::pair<ClientAddress, std::shared_ptr<NetMessage> > > reliableMessages;

    m_packetsExchanger.receivePackets(packet_buffers,reliableMessages);

    for(auto &buffer : packet_buffers)
        this->processPacket(buffer, netMessages);

    if(m_disconnection)
    {
        auto msg = std::dynamic_pointer_cast<NetMessage_ConnectionStatus>(pou::NetEngine::createNetMessage(0));
        msg->connectionStatus = ConnectionStatus_Disconnected;
        netMessages.push_back(msg);

        m_disconnection = false;
    }

    for(auto &msg : reliableMessages)
    {
        if(msg.first.address == m_serverAddress &&
           msg.first.salt == (m_salt ^ m_serverSalt))
            netMessages.push_back(msg.second);
        //this->processMessage(msg);
    }
}

void UdpClient::processPacket(UdpBuffer &buffer,
                              std::list<std::shared_ptr<NetMessage> > &netMessages)
{
    PacketType packetType = m_packetsExchanger.readPacketType(buffer);

    if(m_connectionStatus != ConnectionStatus_Disconnected
    && buffer.address == m_serverAddress)
        m_lastServerAnswerPingTime = m_curLocalTime;

    if(packetType == PacketType_ConnectionMsg)
        this->processConnectionMessages(buffer,netMessages);
}

/*void UdpClient::processMessage(std::pair<ClientAddress, std::shared_ptr<NetMessage> > addressAndMessage)
{
    if(!(addressAndMessage.first.address == m_serverAddress) ||
       addressAndMessage.first.salt != (m_salt ^ m_serverSalt))
        return;

    std::cout<<"The client has got a reliable message dude ! Id:"<<addressAndMessage.second->id<<std::endl;
}*/

void UdpClient::processConnectionMessages(UdpBuffer &buffer,
                                          std::list<std::shared_ptr<NetMessage> > &netMessages)
{
    UdpPacket_ConnectionMsg packet;
    if(!m_packetsExchanger.readPacket(packet, buffer))
        return;

    if(packet.connectionMessage == ConnectionMessage_ConnectionAccepted &&
       (m_salt ^ m_serverSalt) == packet.salt)
    {
        if(m_connectionStatus != ConnectionStatus_Connected)
            Logger::write("Connected to server: "+buffer.address.getAddressString()
                          +"("+std::to_string(m_salt)+","+std::to_string(m_serverSalt)+")");
        m_connectionStatus = ConnectionStatus_Connected;

        auto msg = std::dynamic_pointer_cast<NetMessage_ConnectionStatus>(pou::NetEngine::createNetMessage(0));
        msg->connectionStatus = ConnectionStatus_Connected;
        netMessages.push_back(msg);

        return;
    }
    if(packet.connectionMessage == ConnectionMessage_Challenge)
    {
        m_connectionStatus = ConnectionStatus_Challenging;
        m_serverSalt       = packet.salt;
        m_connectingTimer.reset(CONNECTING_MAX_TIME);
    }
    if(packet.connectionMessage == ConnectionMessage_ConnectionDenied
    && m_connectionStatus != ConnectionStatus_Connected)
    {
        Logger::write("Connection denied to server: "+buffer.address.getAddressString());
        m_connectionStatus = ConnectionStatus_Disconnected;
        return;
    }
    if(packet.connectionMessage == ConnectionMessage_Disconnection &&
       (m_salt ^ m_serverSalt) == packet.salt)
    {
        this->disconnectFromServer();
        //Logger::write("Disconnected from server: "+buffer.address.getAddressString());
        //m_connectionStatus = ConnectionStatus_Disconnected;
        return;
    }
}

void UdpClient::sendConnectionMsg(NetAddress &address, ConnectionMessage msg)
{
    UdpPacket_ConnectionMsg packet;
    packet.type = PacketType_ConnectionMsg;
    packet.connectionMessage = msg;
    packet.salt = m_salt ^ m_serverSalt;

    if(msg == ConnectionMessage_ConnectionRequest)
        packet.salt = m_salt;

    m_packetsExchanger.sendPacket(address,packet);

    m_lastServerPingTime = m_curLocalTime;
}

void UdpClient::tryToConnect()
{
    Logger::write("Attempting to connect to: "+m_serverAddress.getAddressString()
                  +"("+std::to_string(m_salt)+")");

    UdpPacket_ConnectionMsg connectionPacket;
    connectionPacket.type = PacketType_ConnectionMsg;
    connectionPacket.connectionMessage = ConnectionMessage_ConnectionRequest;
    connectionPacket.salt = m_salt;

    m_packetsExchanger.sendPacket(m_serverAddress, connectionPacket);
}


void UdpClient::tryToChallenge()
{
    Logger::write("Attempting to challenge: "+m_serverAddress.getAddressString()
                  +"("+std::to_string(m_salt)+","+std::to_string(m_serverSalt)+")");

    UdpPacket_ConnectionMsg connectionPacket;
    connectionPacket.type = PacketType_ConnectionMsg;
    connectionPacket.connectionMessage = ConnectionMessage_Challenge;
    connectionPacket.salt = m_salt ^ m_serverSalt;

    m_packetsExchanger.sendPacket(m_serverAddress, connectionPacket);
}

float UdpClient::getRTT() const
{
    ClientAddress clientAddress = {m_serverAddress, m_serverSalt^m_salt};
    return m_packetsExchanger.getRTT(clientAddress);
}


}
