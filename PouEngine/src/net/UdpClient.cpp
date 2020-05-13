#include "PouEngine/net/UdpClient.h"

#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/utils/Logger.h"
#include "PouEngine/utils/Hasher.h"


#include "PouEngine/core/VApp.h"

#include <vector>

namespace pou
{

const float UdpClient::CONNECTING_ATTEMPT_DELAY = 1.0f;
const float UdpClient::CONNECTING_MAX_TIME = 10.0f;


UdpClient::UdpClient()
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

    m_port = m_packetsExchanger.getPort();

    return (true);
}

bool UdpClient::destroy()
{
    m_packetsExchanger.destroy();

    return (true);
}

bool UdpClient::connect(const NetAddress &serverAddress)
{
    m_connectionStatus  = ConnectionStatus_Connecting;
    m_serverAddress     = serverAddress;

    m_connectingTimer.reset(CONNECTING_MAX_TIME);

    return (true);
}

bool UdpClient::disconnect()
{
    return (true);
}


void UdpClient::update(const Time &elapsedTime)
{
    if(m_connectionStatus == ConnectionStatus_Connecting)
    {
        if(m_connectingAttemptTimer.update(elapsedTime) || !m_connectingAttemptTimer.isActive())
        {
            /*const char data[] = "hello world!";
            m_socket.send(m_serverAddress, sizeof(data), data);*/
            this->tryToConnect();
            m_connectingAttemptTimer.reset(CONNECTING_ATTEMPT_DELAY);
        }

        if(m_connectingTimer.update(elapsedTime))
        {
            m_connectionStatus = ConnectionStatus_Disconnected;
            Logger::warning("Could not connect to "+m_serverAddress.getAddressString());
        }
    }

    m_packetsExchanger.update(elapsedTime);

    AbstractClient::update(elapsedTime);
}


void UdpClient::receivePackets()
{
    /*NetAddress sender;
    std::vector<uint8_t> buffer(DEFAULT_MAX_PACKETSIZE);

    while(true)
    {
        int bytes_read = m_socket.receive(sender, buffer.data(), buffer.size());
        if (!bytes_read)
            break;

        std::cout<<"Server received packet from"<<sender.getAddressString()<<std::endl;
    }*/

    std::vector<UdpBuffer> packet_buffers;
    m_packetsExchanger.receivePackets(packet_buffers);
    for(auto &buffer : packet_buffers)
        this->processMessages(buffer);
}

void UdpClient::processMessages(UdpBuffer &buffer)
{

}

void UdpClient::tryToConnect()
{
    Logger::write("Attempting to connect to "+m_serverAddress.getAddressString());

    //UdpPacket_BigPacketTest connectionPacket;
    UdpPacket_Header connectionPacket;
    connectionPacket.crc32 = m_packetsExchanger.hashPacket();
    connectionPacket.type = PacketType_Connection;

    //connectionPacket.dummy = 'h';

    UdpBuffer buffer;
    WriteStream stream;
    buffer.buffer.resize(connectionPacket.serialize(&stream));
    stream.setBuffer(buffer.buffer.data(), buffer.buffer.size());
    connectionPacket.serialize(&stream);
    buffer.address = m_serverAddress;

    /*int i = 1;
    std::cout<<(bool)(buffer[i] & (uint8_t)128);
    std::cout<<(bool)(buffer[i] & (uint8_t)64);
    std::cout<<(bool)(buffer[i] & (uint8_t)32);
    std::cout<<(bool)(buffer[i] & (uint8_t)16);
    std::cout<<(bool)(buffer[i] & (uint8_t)8);
    std::cout<<(bool)(buffer[i] & (uint8_t)4);
    std::cout<<(bool)(buffer[i] & (uint8_t)2);
    std::cout<<(bool)(buffer[i] & (uint8_t)1)<<std::endl;*/

    /*UdpPacket_Header testPacket;
    ReadStream rstream;
    rstream.setBuffer(buffer.data(), buffer.size());
    testPacket.Serialize<ReadStream>(rstream);
    std::cout<<"Buffer CRC32:"<<testPacket.crc32<<std::endl;*/

    m_packetsExchanger.sendPacket(buffer);
}


}
