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
const float UdpClient::DEFAULT_DECONNECTIONPINGDELAY = 60.0f * 5;


UdpClient::UdpClient() : m_deconnectionPingDelay(UdpClient::DEFAULT_DECONNECTIONPINGDELAY)
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

bool UdpClient::connectToServer(const NetAddress &serverAddress)
{
    m_connectionStatus  = ConnectionStatus_Connecting;
    m_serverAddress     = serverAddress;

    m_connectingTimer.reset(CONNECTING_MAX_TIME);

    return (true);
}

bool UdpClient::disconnectFromServer()
{
    m_connectionStatus = ConnectionStatus_Disconnected;

    for(auto i = 0 ; i < 5 ; ++i)
        this->sendConnectionMsg(m_serverAddress, ConnectionMessage_Disconnection);

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
            Logger::warning("Could not connect to: "+m_serverAddress.getAddressString());
        }
    }

    if(m_connectionStatus == ConnectionStatus_Connected)
        if(m_lastServerPingTime + m_deconnectionPingDelay < m_curLocalTime)
            this->disconnectFromServer();

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
    PacketType packetType = m_packetsExchanger.readPacketType(buffer);

    if(m_connectionStatus != ConnectionStatus_Disconnected
    && buffer.address == m_serverAddress)
        m_lastServerPingTime = m_curLocalTime;

    if(packetType == PacketType_ConnectionMsg)
        this->processConnectionMessages(buffer);
}


void UdpClient::processConnectionMessages(UdpBuffer &buffer)
{
    UdpPacket_ConnectionMsg packet;
    if(!m_packetsExchanger.readPacket(packet, buffer))
        return;

    if(packet.connectionMessage == ConnectionMessage_ConnectionAccepted)
    {
        Logger::write("Connected to server: "+buffer.address.getAddressString());
        m_connectionStatus = ConnectionStatus_Connected;
        return;
    }
    if(packet.connectionMessage == ConnectionMessage_ConnectionDenied)
    {
        Logger::write("Connection denied to server: "+buffer.address.getAddressString());
        m_connectionStatus = ConnectionStatus_Disconnected;
        return;
    }
    if(packet.connectionMessage == ConnectionMessage_Disconnection)
    {
        Logger::write("Disconnected from server: "+buffer.address.getAddressString());
        m_connectionStatus = ConnectionStatus_Disconnected;
        return;
    }
}

void UdpClient::sendConnectionMsg(NetAddress &address, ConnectionMessage msg)
{
    UdpPacket_ConnectionMsg packet;
    packet.type = PacketType_ConnectionMsg;
    packet.connectionMessage = msg;
    m_packetsExchanger.sendPacket(address,packet);
}

void UdpClient::tryToConnect()
{
    Logger::write("Attempting to connect to "+m_serverAddress.getAddressString());

    UdpPacket_ConnectionMsg connectionPacket;
    //m_packetsExchanger.generatePacketHeader(connectionPacket, PacketType_Connection);
    connectionPacket.type = PacketType_ConnectionMsg;
    connectionPacket.connectionMessage = ConnectionMessage_ConnectionRequest;

    /*UdpBuffer buffer;
    WriteStream stream;
    buffer.buffer.resize(connectionPacket.serialize(&stream));
    stream.setBuffer(buffer.buffer.data(), buffer.buffer.size());
    connectionPacket.serialize(&stream);
    buffer.address = m_serverAddress;*/

    m_packetsExchanger.sendPacket(m_serverAddress, connectionPacket);
}


}
