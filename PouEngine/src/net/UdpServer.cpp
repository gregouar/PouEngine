#include "PouEngine/net/UdpServer.h"


#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include <vector>

namespace pou
{

UdpServer::UdpServer()
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

    m_clients.resize(maxNbrClients,{NetAddress(), ConnectionStatus_Disconnected});

    return (true);
}

bool UdpServer::shutdown()
{
    m_packetsExchanger.destroy();

    return (true);
}

void UdpServer::update(const Time &elapsedTime)
{
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

    if(packetType == PacketType_Connection)
    {
        Logger::write("Client is trying to connect from: "+buffer.address.getAddressString());
    }
    else if (packetType == PacketType_Diconnection)
    {
        Logger::write("Client is disconnected from: "+buffer.address.getAddressString());
    }
}

}
