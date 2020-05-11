#include "PouEngine/net/UdpServer.h"

#include "CRCpp/CRC.h"

#include "PouEngine/core/VApp.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/net/UdpPacketTypes.h"
#include <vector>

namespace pou
{

const int UdpServer::DEFAULT_MAX_PACKETSIZE = 256;

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

    if(!m_socket.open(port))
        return (false);

    m_port = m_socket.getPort();

    m_clients.resize(maxNbrClients,{NetAddress(), ConnectionStatus_Disconnected});

    return (true);
}

bool UdpServer::shutdown()
{
    if(m_socket.isOpen())
        m_socket.close();

    return (true);
}

void UdpServer::receivePackets()
{
    NetAddress sender;
    std::vector<uint8_t> buffer(DEFAULT_MAX_PACKETSIZE);

    while(true)
    {
        int bytes_read = m_socket.receive(sender, buffer.data(), buffer.size());
        if (bytes_read < 0)
            break;

        this->processMessages(sender, buffer);

        //std::cout<<"Server received packet from "<<sender.getAddressString()<<std::endl;
    }
}

void UdpServer::processMessages(NetAddress sender, std::vector<uint8_t> &buffer)
{
    UdpPacket_Header receivedPacket;
    ReadStream stream;
    stream.setBuffer(buffer.data(), buffer.size());
    receivedPacket.Serialize<ReadStream>(stream);

    uint32_t crc32 = CRC::Calculate(VApp::APP_VERSION, sizeof(VApp::APP_VERSION), CRC::CRC_32());
    if(crc32 != (uint32_t)receivedPacket.crc32)
    {
        Logger::warning("Wrong CRC32, packet dropped");
        return;
    }

    PacketType packetType = (PacketType)receivedPacket.type;

    if(packetType == PacketType_Connection)
    {
        Logger::write("Client is trying to connect from: "+sender.getAddressString());
    }
    else if (packetType == PacketType_Diconnection)
    {
        Logger::write("Client is disconnected from: "+sender.getAddressString());
    }
}

}
