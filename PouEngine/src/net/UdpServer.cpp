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

void UdpServer::receivePackets()
{
    std::vector<UdpBuffer> packet_buffers;
    m_packetsExchanger.receivePackets(packet_buffers);
    for(auto &buffer : packet_buffers)
        this->processMessages(buffer);
}

void UdpServer::processMessages(UdpBuffer &buffer)
{
    UdpPacket_Header receivedPacket;
    ReadStream stream;
    stream.setBuffer(buffer.buffer.data(), buffer.buffer.size());
    receivedPacket.Serialize(&stream);

    /*uint32_t crc32 = Hasher::crc32(VApp::APP_VERSION);//CRC::Calculate(VApp::APP_VERSION, sizeof(VApp::APP_VERSION), CRC::CRC_32());
    if(crc32 != (uint32_t)receivedPacket.crc32)
    {
        Logger::warning("Wrong CRC32, packet dropped");
        return;
    }*/

    PacketType packetType = (PacketType)receivedPacket.type;

    if(packetType == PacketType_Connection)
    {
        Logger::write("Client is trying to connect from: "+buffer.sender.getAddressString());
    }
    else if (packetType == PacketType_Diconnection)
    {
        Logger::write("Client is disconnected from: "+buffer.sender.getAddressString());
    }
}

}
