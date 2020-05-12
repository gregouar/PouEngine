#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "PouEngine/net/AbstractServer.h"
#include "PouEngine/net/UdpPacketsExchanger.h"

#include <vector>

namespace pou
{

struct ClientInfos
{
    NetAddress          clientAddress;
    ConnectionStatus    status;
};

class UdpServer : public AbstractServer
{
    public:
        UdpServer();
        virtual ~UdpServer();

        virtual bool start(uint16_t maxNbrClients, unsigned short port = 0);
        virtual bool shutdown();

        //virtual void update(const Time &elapsedTime);

    protected:
        virtual void receivePackets();
        virtual void processMessages(UdpBuffer &buffer);

    private:
       //UdpSocket m_socket;
        UdpPacketsExchanger m_packetsExchanger;

        std::vector<ClientInfos> m_clients;
};

}

#endif // UDPSERVER_H
