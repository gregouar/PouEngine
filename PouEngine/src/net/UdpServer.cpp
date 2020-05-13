#include "PouEngine/net/UdpServer.h"


#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include <vector>

namespace pou
{


const float UdpServer::DEFAULT_DECONNECTIONPINGDELAY = 60.0f * 5;

UdpServer::UdpServer() : m_deconnectionPingDelay(DEFAULT_DECONNECTIONPINGDELAY)
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

    m_port = m_packetsExchanger.getPort();

    m_clients.resize(maxNbrClients,{NetAddress(), ConnectionStatus_Disconnected, 0});

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
        && m_clients[i].lastPingTime + m_deconnectionPingDelay < m_curLocalTime)
        {
            Logger::write("Client timed out from: "+m_clients[i].address.getAddressString());
            this->disconnectClient(i);
        }

    m_packetsExchanger.update(elapsedTime);
    AbstractServer::update(elapsedTime);
}

void UdpServer::receivePackets()
{
    std::vector<UdpBuffer> packet_buffers;
    m_packetsExchanger.receivePackets(packet_buffers);
    for(auto &buffer : packet_buffers)
        this->processMessages(buffer);
}

void UdpServer::processMessages(UdpBuffer &buffer)
{
    PacketType packetType = m_packetsExchanger.readPacketType(buffer);

    uint16_t clientNbr = findClientIndex(buffer.address);
    if(clientNbr != m_clients.size())
        m_clients[clientNbr].lastPingTime = m_curLocalTime;

    if(packetType == PacketType_ConnectionMsg)
        this->processConnectionMessages(buffer);
}


void UdpServer::processConnectionMessages(UdpBuffer &buffer)
{
    UdpPacket_ConnectionMsg packet;
    if(!m_packetsExchanger.readPacket(packet, buffer))
        return;

    if(packet.connectionMessage == ConnectionMessage_ConnectionRequest)
    {
        Logger::write("Client is trying to connect from: "+buffer.address.getAddressString());

        uint16_t clientNbr = findClientIndex(buffer.address);

        if(clientNbr != m_clients.size())
            if(m_clients[clientNbr].status != ConnectionStatus_Disconnected)
            {
                this->allowConnectionFrom(buffer.address);
                return;
            }

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

        m_clients[clientNbr].status     = ConnectionStatus_Connected; //ConnectionStatus_Connecting
        m_clients[clientNbr].address    = std::move(buffer.address);
        m_clients[clientNbr].lastPingTime = m_curLocalTime;

        this->allowConnectionFrom(buffer.address);
    }
    if(packet.connectionMessage == ConnectionMessage_Disconnection)
    {
        uint16_t clientNbr = findClientIndex(buffer.address);
        if(clientNbr != m_clients.size()
        && m_clients[clientNbr].status != ConnectionStatus_Disconnected)
        {
            m_clients[clientNbr].status = ConnectionStatus_Disconnected;
            Logger::write("Client disconnected from: "+buffer.address.getAddressString());
        }

    }
}

void UdpServer::sendConnectionMsg(NetAddress &address, ConnectionMessage msg)
{
    UdpPacket_ConnectionMsg packet;
    packet.type = PacketType_ConnectionMsg;
    packet.connectionMessage = msg;
    m_packetsExchanger.sendPacket(address,packet);
}

void UdpServer::denyConnectionFrom(NetAddress &address)
{
    this->sendConnectionMsg(address, ConnectionMessage_Disconnection);
}

void UdpServer::allowConnectionFrom(NetAddress &address)
{
    this->sendConnectionMsg(address, ConnectionMessage_ConnectionAccepted);
}

void UdpServer::disconnectClient(uint16_t clientNbr)
{
    if(clientNbr >= m_clients.size())
        return;

    m_clients[clientNbr].status = ConnectionStatus_Disconnected;
    this->sendConnectionMsg(m_clients[clientNbr].address, ConnectionMessage_Disconnection);
}

uint16_t UdpServer::findClientIndex(NetAddress &address)
{
    for(auto i = 0 ; i < (int)m_clients.size() ; ++i)
        if(m_clients[i].address == address)
            return i;
    return m_clients.size();
}

}
