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
    if(!m_socket.open(port))
        return (false);

    m_port = m_socket.getPort();
    std::cout<<"PORT:"<<m_socket.getPort()<<std::endl;

    return (true);
}

bool UdpServer::shutdown()
{
    if(m_socket.isOpen())
        m_socket.close();

    return (true);
}


    /*while ( true )
    {
        Address sender;
        unsigned char buffer[256];
        int bytes_read =
            socket.Receive( sender,
                            buffer,
                            sizeof( buffer ) );
        if ( !bytes_read )
            break;

        // process packet
    }*/

}
