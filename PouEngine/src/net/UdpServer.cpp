#include "PouEngine/net/UdpServer.h"

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
    unsigned char buffer[256];

    while(true)
    {
        int bytes_read = m_socket.receive(sender, sizeof(buffer), buffer);
        if (bytes_read < 0)
            break;

        std::cout<<"Server received packet: "<<buffer<<std::endl;
    }
}

void UdpServer::processMessages()
{

}

}
