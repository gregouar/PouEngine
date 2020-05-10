#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/UdpSocket.h"

namespace pou
{

class UdpServer : public AbstractServer
{
    public:
        UdpServer();
        virtual ~UdpServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0);
        virtual bool shutdown();

    protected:

    private:
        UdpSocket m_socket;
};

}

#endif // UDPSERVER_H
