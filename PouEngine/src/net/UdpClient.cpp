#include "PouEngine/net/UdpClient.h"

namespace pou
{

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

    //m_socket.send(serverAddress,,);

    return (true);
}

bool UdpClient::disconnect()
{
    return (true);
}

void UdpClient::receivePackets()
{

}


}
