#include "PouEngine/net/UdpClient.h"

#include "PouEngine/net/UdpPacketTypes.h"
#include "PouEngine/utils/Logger.h"

#include <vector>

namespace pou
{

const float UdpClient::CONNECTING_ATTEMPT_DELAY = 1.0f;
const float UdpClient::CONNECTING_MAX_TIME = 5.0f;

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
    if(!m_socket.open(port))
        return (false);

    m_port = m_socket.getPort();

    return (true);
}

bool UdpClient::destroy()
{
    if(m_socket.isOpen())
        m_socket.close();

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
}


void UdpClient::receivePackets()
{
    NetAddress sender;
    unsigned char buffer[256];

    while(true)
    {
        int bytes_read = m_socket.receive(sender, sizeof(buffer), buffer);
        if (!bytes_read)
            break;

        std::cout<<"Client received packet:"<<buffer<<std::endl;
    }
}

void UdpClient::processMessages()
{

}

void UdpClient::tryToConnect()
{
    UdpPacket_AskConnection connectionPacket;
    connectionPacket.crc32 = 37;
    connectionPacket.test = true;
    connectionPacket.charTest = 'p';

    WriteStream stream;
    auto buffer = std::vector<uint32_t>(connectionPacket.Serialize<WriteStream>(stream));
    stream.setBuffer(buffer.data(), buffer.size());
    connectionPacket.Serialize<WriteStream>(stream);


    /*UdpPacket_AskConnection connectionPacket2;
    ReadStream stream2;
    //auto buffer2 = std::vector<uint8_t>(connectionPacket2.Serialize<ReadStream>(stream2));
    stream2.setBuffer(buffer.data(), buffer.size());
    connectionPacket2.Serialize<ReadStream>(stream2);*/

    const char data[] = "hello world!";
    m_socket.send(m_serverAddress, sizeof(data), data);
}


}
